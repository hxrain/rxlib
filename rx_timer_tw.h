#ifndef _RX_TIMER_TW_H_
#define _RX_TIMER_TW_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list.h"
#include "rx_dtl_queue.h"
#include "rx_mem_alloc.h"
#include "rx_ct_util.h"
#include "rx_ct_delegate.h"

namespace rx
{
	//-------------------------------------------------
	//标准定时器回调函数类型:定时器句柄;外部绑定的事件码;定时器剩余重复次数,0时将被删除;外部绑定的用户指针.
	typedef void(*timer_tw_func_t)(size_t handle, uint32_t event_code, uint32_t repeat, void *usrdat);

	//成员函数回调类型:定时器句柄;外部绑定的事件码;定时器剩余重复次数,0时将被删除;
	//void xx::on_timer(size_t handle, uint32_t event_code, uint32_t repeat)


	namespace tw
	{
		//-------------------------------------------------
		//定时器事件回调委托类型
		typedef delegate3_rt<size_t, uint32_t, uint32_t, void> timer_tw_cb_t;

		//定义内部定时器信息列表类型,用于记录每个时槽中的全部条目
		struct item_t;
		typedef list_t<item_t*>  item_list_t;

		//轮子指针行走的事件委托
		typedef delegate1_rt<item_t*, bool> item_delegate_t;

		//轮子转动一周的事件委托
		typedef delegate1_rt<uint32_t, uint32_t> round_delegate_t;

		const uint32_t wheel_slots = 256;                   //每个轮子上的槽位数量
		const uint32_t slot_mask = wheel_slots - 1;         //槽位索引掩码,用于取模计算
		static const uint32_t max_wheel_count = 4;          //最大允许的轮子级数
		
		//定时条目信息指针的掩码,用于进行额外校验(32位和64位指针的低位对齐空间不同.)
	#if RX_CC_BIT==32
		static const uint8_t item_ptr_mask = 0x03;          
	#else
		static const uint8_t item_ptr_mask = 0x07;          
	#endif

		//-------------------------------------------------
		//获取各层级轮子的满轮滴答数,也是其上层单槽位滴答数
		static rx_inline uint32_t ticks_level(uint32_t i)
		{
			rx_assert(i < max_wheel_count - 1);
			static const uint32_t tbl[max_wheel_count - 1] = { 0x100, 0x10000, 0x1000000 };
			return tbl[i];
		}
		//获取每个层级槽位对应的滴答数
		static rx_inline uint32_t ticks_slot(uint32_t i)
		{
			rx_assert(i < max_wheel_count);
			static const uint32_t tbl[max_wheel_count] = { 0x1,0x100, 0x10000, 0x1000000 };
			return tbl[i];
		}
		//-------------------------------------------------
		//各级轮子对应的满轮滴答数移位(可用>>移位法取商)
		static rx_inline uint8_t ticks_shift(uint32_t i)
		{
			rx_assert(i < max_wheel_count - 1);
			static const uint8_t tbl[max_wheel_count - 1] = { 8, 16, 24 };
			return tbl[i];
		}


		//-------------------------------------------------
		//内部定时器信息项
		typedef struct item_t
		{
			//---------------------------------------------
			timer_tw_cb_t         u_cb_func;                //定时器委托回调
			item_list_t::iterator w_slot_link;              //记录本节点在时间槽链表中的位置,结合slot_idx便于反向查找
			uint64_t              c_dst_tick;               //应该触发的目标滴答数
			uint32_t              u_cycle_tick;             //定时周期间隔滴答数
			uint32_t              u_event_code;             //用户给定的事件码
			uint32_t              u_repeat;                 //重复执行次数,-1为永远执行
			uint8_t               w_wheel_idx;              //本条目所属的轮子编号
			uint8_t               w_slot_idx;               //本条目所属的槽位索引
			uint8_t               c_ttl;                    //本条目的生存计数
			uint8_t               dummy;
			//---------------------------------------------
			//内部信息清除,准备再次复用
			void reset()
			{
				u_cb_func.reset();
				u_cycle_tick = 0;
				u_event_code = 0;
				u_repeat = 0;
				c_dst_tick = 0;
				c_ttl = (c_ttl + 1)&item_ptr_mask;          //生存计数不清零,递增取模

				w_wheel_idx = -1;
				w_slot_idx = 0;
				w_slot_link = NULL;
			}
			//---------------------------------------------
			//获取本条目的对外句柄(低5bit用于在一定程度上避免访问到已经被超时删除的条目)
			size_t handle()
			{
				size_t ret = (size_t)this;
				rx_assert((ret&item_ptr_mask) == 0);
				return ret | c_ttl;                         //借用指针低5bit存放ttl,等待访问时来验证
			}
			//---------------------------------------------
			//校验对外句柄的有效性
			bool check(size_t handle)
			{
				return (handle&item_ptr_mask) == c_ttl;
			}
			//---------------------------------------------
			//静态方法,根据定时器句柄计算内部信息对象指针
			static item_t* to_ptr(size_t handle)
			{
				item_t *ret = (item_t*)(handle&(~item_ptr_mask));
				if (ret->check(handle))
					return ret;
				return NULL;
			}
			//---------------------------------------------
			item_t() :c_ttl(0) { reset(); }
		}item_t;

		//-------------------------------------------------
		//扩展简单缓存管理器
		class entry_cache_t :protected object_cache_t<item_t>
		{
			typedef object_cache_t<item_t> super_t;
			uint32_t m_item_count;
		public:
			entry_cache_t() :m_item_count(0) {}
			bool bind(mem_allotter_i& ma) { return super_t::bind(ma); }
			//---------------------------------------------
			item_t* get()
			{
				item_t *ret = super_t::get();
				rx_assert((((size_t)ret)&item_ptr_mask) == 0);
				if (ret)
					++m_item_count;
				return ret;
			}
			//---------------------------------------------
			void put(item_t *item)
			{
				rx_assert(item != NULL);
				item->reset();
				super_t::put(item);
				--m_item_count;
			}
			//---------------------------------------------
			uint32_t item_count() { return m_item_count; }
			//---------------------------------------------
			void clear() { return super_t::clear(); }
		};

		//-------------------------------------------------
		//基础的时间轮功能封装
		class wheel_t
		{
			item_list_t   m_slots[wheel_slots];             //时间轮中的槽位数组.每个槽位对应着一个定时器信息指针列表
			uint32_t      m_slot_idx;                       //时间轮的槽指针,指向当前被处理过的时间槽
			uint32_t      m_wheel_idx;                      //轮子序号
			//---------------------------------------------
			//循环调用当前时间槽位中的条目事件
			uint32_t m_step_loop(uint64_t curr_tick)
			{
				uint32_t rc = 0;

				item_list_t &slot = m_slots[m_slot_idx];
				for (item_list_t::iterator I = slot.begin(); I != slot.end();)
				{
					item_t *item = *I;
					rx_assert(item != NULL);

					++I;                                    //迭代器预先后移,避免在cb_step中item被删除时产生干扰.

					//下一层级轮子的满轮滴答数,就是当前层级轮子的单时槽滴答数
					uint32_t lvl = m_wheel_idx ? m_wheel_idx - 1 : 0;
					if ((item->c_dst_tick - curr_tick) >= ticks_level(lvl))
						continue;                           //如果当前定时器条目的触发时间还没到达.

					if (cb_item(item))                      //触发定时器事件动作
						++rc;
				}

				return rc;
			}

		public:
			//---------------------------------------------
			wheel_t() :m_slot_idx(0), m_wheel_idx(-1) {}
			item_delegate_t     cb_item;                    //指针转动事件,需要处理一个条目
			round_delegate_t    cb_upon;                    //轮子转动一周事件,可以驱动上层轮子
			//---------------------------------------------
			//初始化
			bool init(uint32_t wheel_idx, mem_allotter_i& mem, uint32_t slot_ptr = (uint32_t)0)
			{
				if (m_wheel_idx != (uint32_t)-1)
					return false;
				m_slot_idx = slot_ptr;
				m_wheel_idx = wheel_idx;
				ct::AC(m_slots, wheel_slots, mem);          //!!强制!!"重新构造初始化"槽位数组,绑定内存分配器
				return true;
			}
			//---------------------------------------------
			//解除
			void uninit(entry_cache_t& ec)
			{
				for (uint32_t i = 0; i < wheel_slots; ++i)
				{
					item_list_t &slot = m_slots[i];
					for (item_list_t::iterator I = slot.begin(); I != slot.end();)
					{
						item_t *item = *I;
						slot.earse(I);
						ec.put(item);
					}
				}

				m_wheel_idx = -1;
				m_slot_idx = -1;
				cb_upon.reset();
				cb_item.reset();
			}
			//---------------------------------------------
			//获取当前轮子的槽位指向位置
			uint32_t& slot_idx() { return m_slot_idx; }
			//---------------------------------------------
			//将给定的item安装在给定的槽位中
			//返回值:是否在新槽位插入成功
			bool put(item_t& item)
			{
				rx_assert_ret(item.w_wheel_idx == m_wheel_idx);

				//在指定的槽位上插入条目
				item_list_t &slot = m_slots[item.w_slot_idx];
				item.w_slot_link = slot.push_back(&item);
				if (item.w_slot_link == slot.end())
				{
					item.w_slot_link = NULL;
					return false;
				}
				return true;
			}
			//---------------------------------------------
			//将给定的item从槽位中摘除
			//返回值:是否从旧槽位删除完成.
			bool pick(item_t& item)
			{
				rx_assert_ret(item.w_wheel_idx == m_wheel_idx);

				//将条目从旧槽中移除
				item_list_t &slot = m_slots[item.w_slot_idx];
				rx_assert_ret(slot.size());

				item_list_t::iterator I = item.w_slot_link;
				rx_check(slot.earse(I));

				//抹除条目的轮槽信息
				item.w_wheel_idx = -1;
				item.w_slot_idx = -1;
				item.w_slot_link = NULL;

				return true;
			}
			//---------------------------------------------
			//将指定的条目移动到新的槽位
			//返回值:是否在新槽位插入成功(失败时当前条目需释放)
			bool move(item_t& item, uint8_t slot_idx)
			{
				rx_assert_ret(item.w_wheel_idx == m_wheel_idx);

				{//将条目从旧槽移除
					item_list_t &slot = m_slots[item.w_slot_idx];
					item_list_t::iterator I = item.w_slot_link;
					rx_check(slot.earse(I));
				}

				//条目放入新槽
				item_list_t &slot = m_slots[slot_idx];
				item.w_slot_link = slot.push_back(&item);
				if (item.w_slot_link == slot.end())
				{
					item.w_slot_link = NULL;
					return false;
				}
				item.w_slot_idx = slot_idx;
				return true;
			}
			//---------------------------------------------
			//驱动当前轮子中的槽位指针前进一步,遍历新槽位中的全部条目
			//返回值:被驱动的条目数量
			uint32_t step(uint64_t curr_tick)
			{
				uint32_t rc = 0;

				//槽位指针前移一步
				m_slot_idx = (m_slot_idx + 1) & slot_mask;
				if (m_slot_idx == 0)
					rc += cb_upon(m_wheel_idx);              //一轮结束了,给出新一轮开始事件,可用于驱动上层轮子的转动

				rc += m_step_loop(curr_tick);

				return rc;
			}
		};

		//-------------------------------------------------
		//多级时间轮管理器
		template<uint32_t wheel_count>
		class wheel_group_t
		{
		protected:
			rx_static_assert(wheel_count >= 1 && wheel_count <= max_wheel_count);
			static const uint32_t min_level_wheel = 0;          //最底层的轮子序号
			static const uint32_t max_level_wheel = wheel_count - 1;//最顶层的轮子序号
			tw::wheel_t         m_wheels[wheel_count];          //定义多级(wheel_count)轮子数组,min_level_wheel(0)为最低级;每级轮子间时间速率相差256倍(wheel_slots)
		public:
			virtual ~wheel_group_t() {}
			//-------------------------------------------------
			//获取指定轮子层级的槽位索引
			uint8_t slot_idx(uint8_t wheel_lvl)
			{
				rx_assert(wheel_lvl < wheel_count);
				return m_wheels[wheel_lvl].slot_idx();
			}
			//-------------------------------------------------
			//根据给定的目标时间,调整轮组位置:滴答数
			void set_abs_pos(size_t dst_tick)
			{
				//在最底层轮子上设置
				m_wheels[0].slot_idx() = uint8_t(dst_tick);
				//从下至上逐层设置正确的位置
				for (uint32_t lvl = 1; lvl <= max_level_wheel; ++lvl)
					m_wheels[lvl].slot_idx() = uint8_t(dst_tick >> tw::ticks_shift(lvl - 1));
			}
			//-------------------------------------------------
			//计算当前轮组的记时滴答数(可选是否精确到最低一级)
			size_t calc_curr_tick(bool exact = false)
			{
				size_t rc = 0;

				for (uint32_t lvl = max_level_wheel; lvl >= 1; --lvl)
					rc += m_wheels[lvl].slot_idx()*tw::ticks_level(lvl - 1);

				if (exact)
					rc += m_wheels[0].slot_idx();

				return rc;
			}
			//-------------------------------------------------
			//计算当前轮组时间点的相对增量后的插入位置
			void calc_rel_pos(size_t inc_tick, uint8_t &wheel_idx, uint8_t &slt_idx)
			{
				if (inc_tick < tw::wheel_slots || max_level_wheel == 0)
				{//增量在最低层能够直接处理
					wheel_idx = 0;
					uint8_t ws = m_wheels[0].slot_idx();
					slt_idx = uint8_t(ws + inc_tick);
					return;
				}

				//从轮组底层向上遍历查找
				for (uint8_t lvl = 1; lvl <= max_level_wheel; ++lvl)
				{
					uint32_t rond_ticks = tw::ticks_level(lvl);
					wheel_idx = lvl;
					//如果时间增量小于当前层级的满轮时长,则将当前层作为插入目标
					if (inc_tick < rond_ticks)
						break;
				}

				//获取当前层槽位序号
				uint8_t ws = slot_idx(wheel_idx);

				//获取当前层时间增量对应的槽位数(带着低层尾数)
				uint8_t ls = slot_idx(wheel_idx - 1);
				size_t is = (inc_tick + ls*tw::ticks_slot(wheel_idx - 1)) >> tw::ticks_shift(wheel_idx - 1);
				//得到最终槽位序号
				slt_idx = uint8_t(ws + is);
			}
		};
	}

	//-----------------------------------------------------
	//时间轮定时器
	//-----------------------------------------------------
	template<uint32_t wheel_count = 4>
	class timer_mgr_t :protected tw::wheel_group_t<wheel_count>
	{
		typedef typename tw::wheel_group_t<wheel_count> super_t;
		tw::entry_cache_t   m_items_cache;                  //定时器条目对象指针缓存
		uint64_t            m_curr_tick;                    //当前滴答数,tick.

		//-------------------------------------------------
		//创建定时器条目
		tw::item_t* m_timer_create(uint32_t cycle_tick, uint32_t event_code, uint32_t repeat)
		{
			tw::item_t *item = m_items_cache.get();
			if (!item) return NULL;

			//初始记录应该触发的目标时刻
			item->c_dst_tick = m_curr_tick + cycle_tick;
			//根据目标时刻计算定时器条目的存放位置
			super_t::calc_rel_pos(cycle_tick, item->w_wheel_idx, item->w_slot_idx);

			if (!super_t::m_wheels[item->w_wheel_idx].put(*item))
			{//新位置插入失败,归还资源
				m_items_cache.put(item);
				return NULL;
			}

			//记录用户侧给定的初始值
			item->u_cycle_tick = cycle_tick;
			item->u_event_code = event_code;
			item->u_repeat = repeat;
			return item;
		}

		//-------------------------------------------------
		//删除一个定时器
		bool m_timer_remove(tw::item_t *item)
		{
			rx_assert(item->w_slot_idx < tw::wheel_slots);
			rx_assert(item->w_slot_link != NULL);
			rx_assert(item->w_wheel_idx < wheel_count);

			if (!super_t::m_wheels[item->w_wheel_idx].pick(*item))
				return false;
			m_items_cache.put(item);
			return true;
		}

		//-------------------------------------------------
		//刷新定时器条目,等待新的开始或轮子降级
		bool m_timer_update(tw::item_t *item, bool is_rep)
		{
			rx_assert(item->w_slot_idx < tw::wheel_slots);
			rx_assert(item->w_slot_link != NULL);
			rx_assert(item->w_wheel_idx < wheel_count);

			size_t dst_tick;
			if (is_rep)
			{//定时器重置,准备下一个周期触发
				item->c_dst_tick = m_curr_tick + item->u_cycle_tick;
				dst_tick = item->u_cycle_tick;
			}
			else
			{//定时器降级
				dst_tick = size_t(item->c_dst_tick - m_curr_tick);
			}

			uint8_t wheel_idx;
			uint8_t slot_idx;
			//计算新位置
			super_t::calc_rel_pos(dst_tick, wheel_idx, slot_idx);

			if (wheel_idx == item->w_wheel_idx)
			{//如果新位置和旧位置都在一个轮子上,则直接进行移动处理
				if (slot_idx == item->w_slot_idx)
					return true;                            //特例,无需移动

				if (!super_t::m_wheels[item->w_wheel_idx].move(*item, slot_idx))
				{//移动不成功,回收当前定时器,不再使用
					m_items_cache.put(item);
					return false;
				}
				return true;
			}

			//从旧轮子上摘除(gcc发神经,需要进行最大值限定才没有警告)
			super_t::m_wheels[(item->w_wheel_idx)&(tw::max_wheel_count - 1)].pick(*item);

			item->w_slot_idx = slot_idx;
			item->w_wheel_idx = wheel_idx;

			if (!super_t::m_wheels[item->w_wheel_idx].put(*item))
			{//新位置插入不成功,回收当前定时器,不再使用
				m_items_cache.put(item);
				return false;
			}
			return true;
		}
		//-------------------------------------------------
		//处理一次轮子转动(级联触发时此方法可能会被递归调用wheel_count层)
		uint32_t on_cb_round(uint32_t wi)
		{
			uint32_t rc = 0;
			if (wi < super_t::max_level_wheel)
				rc = super_t::m_wheels[wi + 1].step(m_curr_tick);        //级联驱动上层时间轮
			return rc;
		}
		//-------------------------------------------------
		//处理一个定时器条目
		//返回值:是否执行了定时器回调事件
		bool on_cb_item(tw::item_t* item)
		{
			rx_assert(item != NULL);

			if (item->w_wheel_idx == super_t::min_level_wheel)
			{//最低层轮子上的定时器被触发了
				if (item->u_repeat != (uint16_t)-1)
					--item->u_repeat;                       //减少重复次数

				rx_assert(item->c_dst_tick == m_curr_tick);

				//调用用户绑定的回调函数
				item->u_cb_func(item->handle(), item->u_event_code, item->u_repeat);

				if (item->u_repeat == 0)
					m_timer_remove(item);                   //如果重复计数为0则删除当前定时器条目
				else
					m_timer_update(item, true);              //需要重新调整当前定时器条目的新位置了
				return true;
			}
			else
			{//上层轮子上的定时器需要进行降级处理
				m_timer_update(item, false);
				return false;
			}
		}

	public:
		//-------------------------------------------------
		~timer_mgr_t() { wheels_uninit(); }
		//-------------------------------------------------
		//时间轮初始化:给出初始时间点;内部时间槽的时间单位(默认1ms);时间轮使用的内存分配器.
		bool wheels_init(mem_allotter_i& mem = rx_global_mem_allotter())
		{
			if (!m_items_cache.bind(mem))
				return false;

			for (uint32_t i = 0; i < wheel_count; ++i)
			{
				tw::wheel_t  &w = super_t::m_wheels[i];
				if (!w.init(i, mem))                        //逐一初始化各级轮子
					return false;

				//给当前时间轮挂接轮子转动事件
				w.cb_upon.bind(*this, cf_ptr(timer_mgr_t, on_cb_round));

				//给当前时间轮挂接条目处理事件
				w.cb_item.bind(*this, cf_ptr(timer_mgr_t, on_cb_item));
			}

			m_curr_tick = 0;
			return true;
		}

		//-------------------------------------------------
		//解除
		void wheels_uninit()
		{
			for (uint32_t i = 0; i < wheel_count; ++i)
			{
				tw::wheel_t &w = super_t::m_wheels[i];
				w.uninit(m_items_cache);
			}
			m_items_cache.clear();
		}

		//-------------------------------------------------
		//驱动当前时间轮:给出行走步数
		//返回值:真正触发的定时器条目数量;-1时发生时间突变了,等待下次过程.
		uint32_t wheels_step(uint32_t step_count = 1)
		{
			uint32_t rc = 0;
			//驱动最低级时间轮行走(在round满圈回调中会同步驱动上级时间轮)
			for (uint32_t i = 0; i < step_count; ++i)
			{
				++m_curr_tick;
				rc += super_t::m_wheels[super_t::min_level_wheel].step(m_curr_tick);
			}

			return rc;
		}

		//-------------------------------------------------
		//创建一个新的定时器:回调函数指针;回调参数;间隔滴答数(*tick_unit_ms后为间隔时间ms);事件码;重复次数
		//返回值:0失败;其他为定时器句柄.
		size_t timer_insert(timer_tw_func_t func, void* usrdat, uint32_t inv_tick, uint32_t event_code = 0, uint32_t repeat = -1)
		{
			if (repeat == 0) return 0;
			//创建内部定时器条目
			tw::item_t *item = m_timer_create(inv_tick, event_code, repeat);
			if (!item) return 0;

			//定时器条目绑定外部事件回调函数
			item->u_cb_func.bind(func, usrdat);
			//返回定时器句柄
			return item->handle();
		}
		template<class H>
		size_t timer_insert(H& owner, void(H::*member_func)(size_t handle, uint32_t event_code, uint32_t repeat), uint32_t inv_tick, uint32_t event_code = 0, uint32_t repeat = -1)
		{
			if (repeat == 0) return 0;
			//创建内部定时器条目
			tw::item_t *item = m_timer_create(inv_tick, event_code, repeat);
			if (!item) return 0;

			//定时器条目绑定外部对象回调函数
			item->u_cb_func.bind(owner, member_func);
			//返回定时器句柄
			return item->handle();
		}
		//-------------------------------------------------
		//查询内部定时器数量
		uint32_t timer_count() { return m_items_cache.item_count(); }
		//-------------------------------------------------
		//删除一个定时器
		bool timer_remove(size_t timer)
		{
			tw::item_t *item = tw::item_t::to_ptr(timer);
			rx_assert_ret(item);
			return m_timer_remove(item);
		}

		//-------------------------------------------------
		//更新一个定时器(以当前时间点和触发间隔,更新下一次的触发时刻)
		bool timer_update(size_t timer)
		{
			tw::item_t *item = tw::item_t::to_ptr(timer);
			rx_assert_ret(item);
			return m_timer_update(item, true);
		}
	};
}

#endif

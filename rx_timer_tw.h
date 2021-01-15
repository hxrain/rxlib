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
	//��׼��ʱ���ص���������:��ʱ�����;�ⲿ�󶨵��¼���;��ʱ��ʣ���ظ�����,0ʱ����ɾ��;�ⲿ�󶨵��û�ָ��.
	typedef void(*timer_tw_func_t)(size_t handle, uint32_t event_code, uint32_t repeat, void *usrdat);

	//��Ա�����ص�����:��ʱ�����;�ⲿ�󶨵��¼���;��ʱ��ʣ���ظ�����,0ʱ����ɾ��;
	//void xx::on_timer(size_t handle, uint32_t event_code, uint32_t repeat)


	namespace tw
	{
		//-------------------------------------------------
		//��ʱ���¼��ص�ί������
		typedef delegate3_rt<size_t, uint32_t, uint32_t, void> timer_tw_cb_t;

		//�����ڲ���ʱ����Ϣ�б�����,���ڼ�¼ÿ��ʱ���е�ȫ����Ŀ
		struct item_t;
		typedef list_t<item_t*>  item_list_t;

		//����ָ�����ߵ��¼�ί��
		typedef delegate1_rt<item_t*, bool> item_delegate_t;

		//����ת��һ�ܵ��¼�ί��
		typedef delegate1_rt<uint32_t, uint32_t> round_delegate_t;

		const uint32_t wheel_slots = 256;                   //ÿ�������ϵĲ�λ����
		const uint32_t slot_mask = wheel_slots - 1;         //��λ��������,����ȡģ����
		static const uint32_t max_wheel_count = 4;          //�����������Ӽ���
		#if RX_CC_BIT==32
		static const uint8_t item_ptr_mask = 0x03;          //��ʱ��Ŀ��Ϣָ�������,���ڽ��ж���У��
		#else
		static const uint8_t item_ptr_mask = 0x07;          //��ʱ��Ŀ��Ϣָ�������,���ڽ��ж���У��
		#endif


			//-------------------------------------------------
			//��ȡ���㼶���ӵ����ֵδ���,Ҳ�����ϲ㵥��λ�δ���
		static rx_inline uint32_t ticks_level(uint32_t i)
		{
			rx_assert(i < max_wheel_count - 1);
			static const uint32_t tbl[max_wheel_count - 1] = { 0x100, 0x10000, 0x1000000 };
			return tbl[i];
		}
		//��ȡÿ���㼶��λ��Ӧ�ĵδ���
		static rx_inline uint32_t ticks_slot(uint32_t i)
		{
			rx_assert(i < max_wheel_count);
			static const uint32_t tbl[max_wheel_count] = { 0x1,0x100, 0x10000, 0x1000000 };
			return tbl[i];
		}
		//-------------------------------------------------
		//�������Ӷ�Ӧ�����ֵδ�����λ(����>>��λ��ȡ��)
		static rx_inline uint8_t ticks_shift(uint32_t i)
		{
			rx_assert(i < max_wheel_count - 1);
			static const uint8_t tbl[max_wheel_count - 1] = { 8, 16, 24 };
			return tbl[i];
		}


		//-------------------------------------------------
		//�ڲ���ʱ����Ϣ��
		typedef struct item_t
		{
			//---------------------------------------------
			timer_tw_cb_t         u_cb_func;                //��ʱ��ί�лص�
			item_list_t::iterator w_slot_link;              //��¼���ڵ���ʱ��������е�λ��,���slot_idx���ڷ������
			uint64_t              c_dst_tick;               //Ӧ�ô�����Ŀ��δ���
			uint32_t              u_cycle_tick;             //��ʱ���ڼ���δ���
			uint32_t              u_event_code;             //�û��������¼���
			uint32_t              u_repeat;                 //�ظ�ִ�д���,-1Ϊ��Զִ��
			uint8_t               w_wheel_idx;              //����Ŀ���������ӱ��
			uint8_t               w_slot_idx;               //����Ŀ�����Ĳ�λ����
			uint8_t               c_ttl;                    //����Ŀ���������
			uint8_t               dummy;
			//---------------------------------------------
			//�ڲ���Ϣ���,׼���ٴθ���
			void reset()
			{
				u_cb_func.reset();
				u_cycle_tick = 0;
				u_event_code = 0;
				u_repeat = 0;
				c_dst_tick = 0;
				c_ttl = (c_ttl + 1)&item_ptr_mask;          //�������������,����ȡģ

				w_wheel_idx = -1;
				w_slot_idx = 0;
				w_slot_link = NULL;
			}
			//---------------------------------------------
			//��ȡ����Ŀ�Ķ�����(��5bit������һ���̶��ϱ�����ʵ��Ѿ�����ʱɾ������Ŀ)
			size_t handle()
			{
				size_t ret = (size_t)this;
				rx_assert((ret&item_ptr_mask) == 0);
				return ret | c_ttl;                         //����ָ���5bit���ttl,�ȴ�����ʱ����֤
			}
			//---------------------------------------------
			//У�����������Ч��
			bool check(size_t handle)
			{
				return (handle&item_ptr_mask) == c_ttl;
			}
			//---------------------------------------------
			//��̬����,���ݶ�ʱ����������ڲ���Ϣ����ָ��
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
		//��չ�򵥻��������
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
		//������ʱ���ֹ��ܷ�װ
		class wheel_t
		{
			item_list_t   m_slots[wheel_slots];             //ʱ�����еĲ�λ����.ÿ����λ��Ӧ��һ����ʱ����Ϣָ���б�
			uint32_t      m_slot_idx;                       //ʱ���ֵĲ�ָ��,ָ��ǰ���������ʱ���
			uint32_t      m_wheel_idx;                      //�������
			//---------------------------------------------
			//ѭ�����õ�ǰʱ���λ�е���Ŀ�¼�
			uint32_t m_step_loop(uint64_t curr_tick)
			{
				uint32_t rc = 0;

				item_list_t &slot = m_slots[m_slot_idx];
				for (item_list_t::iterator I = slot.begin(); I != slot.end();)
				{
					item_t *item = *I;
					rx_assert(item != NULL);

					++I;                                    //������Ԥ�Ⱥ���,������cb_step��item��ɾ��ʱ��������.

					//��һ�㼶���ӵ����ֵδ���,���ǵ�ǰ�㼶���ӵĵ�ʱ�۵δ���
					uint32_t lvl = m_wheel_idx ? m_wheel_idx - 1 : 0;
					if ((item->c_dst_tick - curr_tick) >= ticks_level(lvl))
						continue;                           //�����ǰ��ʱ����Ŀ�Ĵ���ʱ�仹û����.

					if (cb_item(item))                      //������ʱ���¼�����
						++rc;
				}

				return rc;
			}

		public:
			//---------------------------------------------
			wheel_t() :m_slot_idx(0), m_wheel_idx(-1) {}
			item_delegate_t     cb_item;                    //ָ��ת���¼�,��Ҫ����һ����Ŀ
			round_delegate_t    cb_upon;                    //����ת��һ���¼�,���������ϲ�����
			//---------------------------------------------
			//��ʼ��
			bool init(uint32_t wheel_idx, mem_allotter_i& mem, uint32_t slot_ptr = (uint32_t)0)
			{
				if (m_wheel_idx != (uint32_t)-1)
					return false;
				m_slot_idx = slot_ptr;
				m_wheel_idx = wheel_idx;
				ct::AC(m_slots, wheel_slots, mem);          //!!ǿ��!!"���¹����ʼ��"��λ����,���ڴ������
				return true;
			}
			//---------------------------------------------
			//���
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
			//��ȡ��ǰ���ӵĲ�λָ��λ��
			uint32_t& slot_idx() { return m_slot_idx; }
			//---------------------------------------------
			//��������item��װ�ڸ����Ĳ�λ��
			//����ֵ:�Ƿ����²�λ����ɹ�
			bool put(item_t& item)
			{
				rx_assert_ret(item.w_wheel_idx == m_wheel_idx);

				//��ָ���Ĳ�λ�ϲ�����Ŀ
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
			//��������item�Ӳ�λ��ժ��
			//����ֵ:�Ƿ�Ӿɲ�λɾ�����.
			bool pick(item_t& item)
			{
				rx_assert_ret(item.w_wheel_idx == m_wheel_idx);

				//����Ŀ�Ӿɲ����Ƴ�
				item_list_t &slot = m_slots[item.w_slot_idx];
				rx_assert_ret(slot.size());

				item_list_t::iterator I = item.w_slot_link;
				rx_check(slot.earse(I));

				//Ĩ����Ŀ���ֲ���Ϣ
				item.w_wheel_idx = -1;
				item.w_slot_idx = -1;
				item.w_slot_link = NULL;

				return true;
			}
			//---------------------------------------------
			//��ָ������Ŀ�ƶ����µĲ�λ
			//����ֵ:�Ƿ����²�λ����ɹ�(ʧ��ʱ��ǰ��Ŀ���ͷ�)
			bool move(item_t& item, uint8_t slot_idx)
			{
				rx_assert_ret(item.w_wheel_idx == m_wheel_idx);

				{//����Ŀ�Ӿɲ��Ƴ�
					item_list_t &slot = m_slots[item.w_slot_idx];
					item_list_t::iterator I = item.w_slot_link;
					rx_check(slot.earse(I));
				}

				//��Ŀ�����²�
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
			//������ǰ�����еĲ�λָ��ǰ��һ��,�����²�λ�е�ȫ����Ŀ
			//����ֵ:����������Ŀ����
			uint32_t step(uint64_t curr_tick)
			{
				uint32_t rc = 0;

				//��λָ��ǰ��һ��
				m_slot_idx = (m_slot_idx + 1) & slot_mask;
				if (m_slot_idx == 0)
					rc += cb_upon(m_wheel_idx);              //һ�ֽ�����,������һ�ֿ�ʼ�¼�,�����������ϲ����ӵ�ת��

				rc += m_step_loop(curr_tick);

				return rc;
			}
		};
		//-------------------------------------------------
		template<uint32_t wheel_count>
		class wheel_group_t
		{
		protected:
			rx_static_assert(wheel_count >= 1 && wheel_count <= max_wheel_count);
			static const uint32_t min_level_wheel = 0;          //��ײ���������
			static const uint32_t max_level_wheel = wheel_count - 1;//�����������
			tw::wheel_t         m_wheels[wheel_count];          //����༶(wheel_count)��������,min_level_wheel(0)Ϊ��ͼ�;ÿ�����Ӽ�ʱ���������256��(wheel_slots)
		public:
			//-------------------------------------------------
			//��ȡָ�����Ӳ㼶�Ĳ�λ����
			uint8_t slot_idx(uint8_t wheel_lvl)
			{
				rx_assert(wheel_lvl < wheel_count);
				return m_wheels[wheel_lvl].slot_idx();
			}
			//-------------------------------------------------
			//���ݸ�����Ŀ��ʱ��,��������λ��:�δ���
			void set_abs_pos(size_t dst_tick)
			{
				//����ײ�����������
				m_wheels[0].slot_idx() = uint8_t(dst_tick);
				//�����������������ȷ��λ��
				for (uint32_t lvl = 1; lvl <= max_level_wheel; ++lvl)
					m_wheels[lvl].slot_idx() = uint8_t(dst_tick >> tw::ticks_shift(lvl - 1));
			}
			//-------------------------------------------------
			//���㵱ǰ����ļ�ʱ�δ���
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
			//���㵱ǰ����ʱ�������������Ĳ���λ��
			void calc_rel_pos(size_t inc_tick, uint8_t &wheel_idx, uint8_t &slt_idx)
			{
				if (inc_tick < tw::wheel_slots || max_level_wheel == 0)
				{//��������Ͳ��ܹ�ֱ�Ӵ���
					wheel_idx = 0;
					uint8_t ws = m_wheels[0].slot_idx();
					slt_idx = uint8_t(ws + inc_tick);
					return;
				}

				//������ײ����ϱ�������
				for (uint8_t lvl = 1; lvl <= max_level_wheel; ++lvl)
				{
					uint32_t rond_ticks = tw::ticks_level(lvl);
					wheel_idx = lvl;
					//���ʱ������С�ڵ�ǰ�㼶������ʱ��,�򽫵�ǰ����Ϊ����Ŀ��
					if (inc_tick < rond_ticks)
						break;
				}

				//��ȡ��ǰ���λ���
				uint8_t ws = slot_idx(wheel_idx);

				//��ȡ��ǰ��ʱ��������Ӧ�Ĳ�λ��(���ŵͲ�β��)
				uint8_t ls = slot_idx(wheel_idx - 1);
				size_t is = (inc_tick + ls*tw::ticks_slot(wheel_idx - 1)) >> tw::ticks_shift(wheel_idx - 1);
				//�õ����ղ�λ���
				slt_idx = uint8_t(ws + is);
			}
		};
	}

	//-----------------------------------------------------
	//ʱ���ֶ�ʱ��
	//-----------------------------------------------------
	template<uint32_t wheel_count = 4>
	class tw_timer_mgr_t :protected tw::wheel_group_t<wheel_count>
	{
		typedef typename tw::wheel_group_t<wheel_count> super_t;
		tw::entry_cache_t   m_items_cache;                  //��ʱ����Ŀ����ָ�뻺��
		uint64_t            m_curr_tick;                    //��ǰ�δ���,tick.

		//-------------------------------------------------
		//������ʱ����Ŀ
		tw::item_t* m_timer_create(uint32_t cycle_tick, uint32_t event_code, uint32_t repeat)
		{
			tw::item_t *item = m_items_cache.get();
			if (!item) return NULL;

			//��ʼ��¼Ӧ�ô�����Ŀ��ʱ��
			item->c_dst_tick = m_curr_tick + cycle_tick;
			//����Ŀ��ʱ�̼��㶨ʱ����Ŀ�Ĵ��λ��
			super_t::calc_rel_pos(cycle_tick, item->w_wheel_idx, item->w_slot_idx);

			if (!super_t::m_wheels[item->w_wheel_idx].put(*item))
			{//��λ�ò���ʧ��,�黹��Դ
				m_items_cache.put(item);
				return NULL;
			}

			//��¼�û�������ĳ�ʼֵ
			item->u_cycle_tick = cycle_tick;
			item->u_event_code = event_code;
			item->u_repeat = repeat;
			return item;
		}

		//-------------------------------------------------
		//ɾ��һ����ʱ��
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
		//ˢ�¶�ʱ����Ŀ,�ȴ��µĿ�ʼ�����ӽ���
		bool m_timer_update(tw::item_t *item, bool is_rep)
		{
			rx_assert(item->w_slot_idx < tw::wheel_slots);
			rx_assert(item->w_slot_link != NULL);
			rx_assert(item->w_wheel_idx < wheel_count);

			size_t dst_tick;
			if (is_rep)
			{//��ʱ������,׼����һ�����ڴ���
				item->c_dst_tick = m_curr_tick + item->u_cycle_tick;
				dst_tick = item->u_cycle_tick;
			}
			else
			{//��ʱ������
				dst_tick = size_t(item->c_dst_tick - m_curr_tick);
			}

			uint8_t wheel_idx;
			uint8_t slot_idx;
			//������λ��
			super_t::calc_rel_pos(dst_tick, wheel_idx, slot_idx);

			if (wheel_idx == item->w_wheel_idx)
			{//�����λ�ú;�λ�ö���һ��������,��ֱ�ӽ����ƶ�����
				if (slot_idx == item->w_slot_idx)
					return true;                            //����,�����ƶ�

				if (!super_t::m_wheels[item->w_wheel_idx].move(*item, slot_idx))
				{//�ƶ����ɹ�,���յ�ǰ��ʱ��,����ʹ��
					m_items_cache.put(item);
					return false;
				}
				return true;
			}

			//�Ӿ�������ժ��(gcc����,��Ҫ�������ֵ�޶���û�о���)
			super_t::m_wheels[(item->w_wheel_idx)&(tw::max_wheel_count - 1)].pick(*item);

			item->w_slot_idx = slot_idx;
			item->w_wheel_idx = wheel_idx;

			if (!super_t::m_wheels[item->w_wheel_idx].put(*item))
			{//��λ�ò��벻�ɹ�,���յ�ǰ��ʱ��,����ʹ��
				m_items_cache.put(item);
				return false;
			}
			return true;
		}
		//-------------------------------------------------
		//����һ������ת��(��������ʱ�˷������ܻᱻ�ݹ����wheel_count��)
		uint32_t on_cb_round(uint32_t wi)
		{
			uint32_t rc = 0;
			if (wi < super_t::max_level_wheel)
				rc = super_t::m_wheels[wi + 1].step(m_curr_tick);        //���������ϲ�ʱ����
			return rc;
		}
		//-------------------------------------------------
		//����һ����ʱ����Ŀ
		//����ֵ:�Ƿ�ִ���˶�ʱ���ص��¼�
		bool on_cb_item(tw::item_t* item)
		{
			rx_assert(item != NULL);

			if (item->w_wheel_idx == super_t::min_level_wheel)
			{//��Ͳ������ϵĶ�ʱ����������
				if (item->u_repeat != (uint16_t)-1)
					--item->u_repeat;                       //�����ظ�����

				rx_assert(item->c_dst_tick == m_curr_tick);

				//�����û��󶨵Ļص�����
				item->u_cb_func(item->handle(), item->u_event_code, item->u_repeat);

				if (item->u_repeat == 0)
					m_timer_remove(item);                   //����ظ�����Ϊ0��ɾ����ǰ��ʱ����Ŀ
				else
					m_timer_update(item, true);              //��Ҫ���µ�����ǰ��ʱ����Ŀ����λ����
				return true;
			}
			else
			{//�ϲ������ϵĶ�ʱ����Ҫ���н�������
				m_timer_update(item, false);
				return false;
			}
		}

	public:
		//-------------------------------------------------
		~tw_timer_mgr_t() { wheels_uninit(); }
		//-------------------------------------------------
		//ʱ���ֳ�ʼ��:������ʼʱ���;�ڲ�ʱ��۵�ʱ�䵥λ(Ĭ��1ms);ʱ����ʹ�õ��ڴ������.
		bool wheels_init(mem_allotter_i& mem = rx_global_mem_allotter())
		{
			if (!m_items_cache.bind(mem))
				return false;

			for (uint32_t i = 0; i < wheel_count; ++i)
			{
				tw::wheel_t  &w = super_t::m_wheels[i];
				if (!w.init(i, mem))                        //��һ��ʼ����������
					return false;

				//����ǰʱ���ֹҽ�����ת���¼�
				w.cb_upon.bind(*this, cf_ptr(tw_timer_mgr_t, on_cb_round));

				//����ǰʱ���ֹҽ���Ŀ�����¼�
				w.cb_item.bind(*this, cf_ptr(tw_timer_mgr_t, on_cb_item));
			}

			m_curr_tick = 0;
			return true;
		}

		//-------------------------------------------------
		//���
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
		//������ǰʱ����:�������߲���
		//����ֵ:���������Ķ�ʱ����Ŀ����;-1ʱ����ʱ��ͻ����,�ȴ��´ι���.
		uint32_t wheels_step(uint32_t step_count = 1)
		{
			uint32_t rc = 0;
			//������ͼ�ʱ��������(��round��Ȧ�ص��л�ͬ�������ϼ�ʱ����)
			for (uint32_t i = 0; i < step_count; ++i)
			{
				++m_curr_tick;
				rc += super_t::m_wheels[super_t::min_level_wheel].step(m_curr_tick);
			}

			return rc;
		}

		//-------------------------------------------------
		//����һ���µĶ�ʱ��:�ص�����ָ��;�ص�����;����δ���(*tick_unit_ms��Ϊ���ʱ��ms);�¼���;�ظ�����
		//����ֵ:0ʧ��;����Ϊ��ʱ�����.
		size_t timer_insert(timer_tw_func_t func, void* usrdat, uint32_t inv_tick, uint32_t event_code = 0, uint32_t repeat = -1)
		{
			if (repeat == 0) return 0;
			//�����ڲ���ʱ����Ŀ
			tw::item_t *item = m_timer_create(inv_tick, event_code, repeat);
			if (!item) return 0;

			//��ʱ����Ŀ���ⲿ�¼��ص�����
			item->u_cb_func.bind(func, usrdat);
			//���ض�ʱ�����
			return item->handle();
		}
		template<class H>
		size_t timer_insert(H& owner, void(H::*member_func)(size_t handle, uint32_t event_code, uint32_t repeat), uint32_t inv_tick, uint32_t event_code = 0, uint32_t repeat = -1)
		{
			if (repeat == 0) return 0;
			//�����ڲ���ʱ����Ŀ
			tw::item_t *item = m_timer_create(inv_tick, event_code, repeat);
			if (!item) return 0;

			//��ʱ����Ŀ���ⲿ����ص�����
			item->u_cb_func.bind(owner, member_func);
			//���ض�ʱ�����
			return item->handle();
		}
		//-------------------------------------------------
		//��ѯ�ڲ���ʱ������
		uint32_t timer_count() { return m_items_cache.item_count(); }
		//-------------------------------------------------
		//ɾ��һ����ʱ��
		bool timer_remove(size_t timer)
		{
			tw::item_t *item = tw::item_t::to_ptr(timer);
			rx_assert_ret(item);
			return m_timer_remove(item);
		}

		//-------------------------------------------------
		//����һ����ʱ��(�Ե�ǰʱ���ʹ������,������һ�εĴ���ʱ��)
		bool timer_update(size_t timer)
		{
			tw::item_t *item = tw::item_t::to_ptr(timer);
			rx_assert_ret(item);
			return m_timer_update(item, true);
		}
	};
}

#endif

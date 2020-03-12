#ifndef _RX_DTL_HAT_RAW_H_
#define _RX_DTL_HAT_RAW_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_hash_data.h"
#include "rx_cc_base.h"
#include "rx_dtl_alg.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"

/*
	hash array table(hat)
	构建一个用于快速检索的轻量级关联容器,基于array作为底层,可实现hash/bst等查找方法
	key与val的类型为数组(char*,wchar_t*,uint8_t*,uint32_t*,struct*等)
	为了简单起见,val的size需要固定,或者没有val的部分;不提供删除元素的功能;
	底层数据布局格式为:head|key偏移数组|值数组|key数组
		head|keyoff1|keyoff2|...|val1|val2|...|key1/0|key2/0|.../0
	head中的信息有:最大元素容量/已有元素数量/值尺寸/空间总量/已用空间最后位置
	容器内部支持两种查找方法:默认为hash表,未排序;排序后,升序,二分法;排序后进行前缀搜索比较快.
*/
namespace rx
{
	//-----------------------------------------------------
	//hat运算器
	class hat_baseop_t
	{
	public:
		//-------------------------------------------------
		//hash函数
		template<class KT>
		static uint32_t hash(const KT *k, uint32_t cnt) { return rx_hash_murmur(k, sizeof(k)*cnt); }
		static uint32_t hash(const char *k, uint32_t cnt) { return rx_hash_zob(k); }
		static uint32_t hash(const wchar_t *k, uint32_t cnt) { return rx_hash_zob(k); }

		//-------------------------------------------------
		//不定长key的比较函数:是否相同
		template<class KT>
		static bool equ(const KT* k1, uint16_t k1_cnt, const KT* k2, uint16_t k2_cnt)
		{
			if (k1_cnt != k2_cnt) return false;
			for (uint16_t i = 0;i < k1_cnt;++i)
			{
				if (k1[i] != k2[i])
					return false;
			}
			return true;
		}

		//-------------------------------------------------
		//按元素内容比较不定长的两个key,返回值:<0,k1<k2;=0,k1=k2;>0,k1>k2
		template<class KT>
		static int cmp(const KT* k1, uint16_t k1_cnt, const KT* k2, uint16_t k2_cnt)
		{
			uint16_t cnt = Min(k1_cnt, k2_cnt);				//先按两个key的最短长度进行内容比较
			for (uint16_t i = 0;i < cnt;++i)
			{
				if (k1[i] == k2[i])
					continue;
				if (k1[i] < k2[i])
					return -1;
				else
					return 1;
			}
			if (k1_cnt == k2_cnt)
				return 0;									//如果两个key的长度相同且内容也相同,则二者是真正的相同.
			return k1_cnt < k2_cnt ? -2 : 2;				//两个key的前缀完全相同时,则短的key是小的.
		}
	};

	//-----------------------------------------------------
	//结合了基础功能,并扩展了生存期事件的hat操作函数
	class hat_op_t:public hat_baseop_t
	{
	public:
		//-------------------------------------------------
		//子类实现,用于在清理容器之前进行k/v的析构动作
		template<class KT,class VT>
		static void on_key_clear(uint16_t idx, const KT* key, uint16_t key_cnt, VT *val, uint16_t val_cnt) {}
		//子类实现,告知一个新key产生了,便于进行k/v的初始化构造
		template<class KT, class VT>
		static void on_key_make(uint16_t idx, const KT* key, uint16_t key_cnt, VT *val, uint16_t val_cnt) {}
	};

	//-----------------------------------------------------
	//计算hat占用的空间:头部占用空间 + key偏移数组占用空间 + val占用总空间 + key占用总空间
	#define calc_hat_space(head_t,keyoff_t,capacity,key_t,key_count,val_t,val_cnt) \
		(sizeof(head_t)+ sizeof(keyoff_t)*capacity + sizeof(val_t)*val_cnt*capacity + sizeof(key_t)*(key_count+1)*capacity)

	//-----------------------------------------------------
	//轻量级紧凑查找表
	template<class key_t, class val_t, class hat_op = hat_op_t>
	class hat_raw_t
	{
	public:
		//-------------------------------------------------
		struct keyoff_t;
	protected:
		//-------------------------------------------------
		//底层空间指针,唯一存放数据的地方
		void*				m_buff;

		//二分搜索需要的,==和>比较器,需要对keyoff_t和指定的key进行比较
		class bs_cmp_t
		{
			const hat_raw_t &parent;						//hat对象,用于访问其内部方法与数据
			const key_t		*key;							//待比较的key
			uint16_t		key_cnt;						//待比较key的长度
			uint16_t		is_pre;							//是否为前缀模式:0-key全等模式;1-key前缀查找;2-key前缀增量查找
		public:
			const key_t		*item;							//前缀增量查找时的增量条目
			bs_cmp_t(const hat_raw_t &p, const key_t *k, uint16_t kc, uint16_t is_pre = 0) :parent(p), key(k), key_cnt(kc), is_pre(is_pre), item(NULL) {}
			//判断ko与*this的关系:<0,0,>0
			int operator()(const keyoff_t& ko) const;
		};
		//-------------------------------------------------
		//定义key偏移量所需类型
		typedef struct keyoff_t
		{
			uint32_t		offset;							//key所在的偏移位置
			uint16_t		val_idx;						//key对应的val索引
			uint16_t		key_cnt;						//key的元素数量
			bool operator==(const bs_cmp_t& cmp) const { return cmp(*this)==0; }
			bool operator>(const bs_cmp_t& cmp) const { return cmp(*this)>0; }
			bool operator<(const bs_cmp_t& cmp) const { return cmp(*this)<0; }
			bool operator<=(const bs_cmp_t& cmp) const { return cmp(*this)<=0; }
		}keyoff_t;
		//-------------------------------------------------
		//定义容器头部信息结构
		typedef struct head_t
		{
			uint32_t		buff_size;                      //底层空间总容量
			uint32_t		buff_last;                      //底层空间最后可以拼装key的位置
			uint16_t		val_cnt;						//每个值占用的元素val_t的数量
			uint16_t		capacity;						//最大可放置的元素数量
			uint16_t		sorted;							//当前容器是否已经排序
			uint16_t		size;							//已经存在的元素数量
		}head_t;

		//-------------------------------------------------
		//排序需要的,小于比较器,如果a<b则返回真
		template<class DT>
		class qs_cmp_t
		{
			hat_raw_t &parent;
		public:
			qs_cmp_t(hat_raw_t &p) :parent(p) {}
			bool operator()(const DT &a, const DT &b) const
			{
				uint8_t ei = a.offset == 0 ? 0 : 2;			//a元素作用在ei上,要么是0b00要么是0b10
				ei |= b.offset == 0 ? 0 : 1;				//b元素叠加在ei上,组合是0b00/0b10/0b01/0b11
				switch (ei)
				{
				case 0:return true;							//a和b都是空槽位,认为a<b
				case 1:return false;						//a是空槽位,b有值,认为a>=b
				case 2:return true;							//a有值,b是空槽位,认为a<b
				}
				//现在,a和b都有值,需要比较key了
				key_t *k1 = parent.key(a);
				key_t *k2 = parent.key(b);
				return hat_op::cmp(k1, a.key_cnt, k2, b.key_cnt) < 0;
			}
		};
		//-------------------------------------------------
		head_t& head() const { rx_assert(m_buff != NULL); return *(head_t*)m_buff; }
		//-------------------------------------------------
		//绑定所需空间,并进行初始化
		bool bind(void *buff, uint32_t buff_size, uint16_t cap, uint32_t val_cnt)
		{
			if (m_buff)
				return false;

			m_buff = buff;

			head().buff_size = buff_size;
			head().capacity = cap;
			head().val_cnt = val_cnt;
			return reset();
		}
		//-------------------------------------------------
		//状态复位
		bool reset()
		{
			if (!m_buff || !head().buff_size || !head().capacity)
				return false;
			head().sorted = 0;
			head().size = 0;
			head().buff_last = hov_space();
			memset((uint8_t*)m_buff + sizeof(head_t), 0, head().buff_size - sizeof(head_t));
			return true;
		}
		//-------------------------------------------------
		//计算存放head/offset/values的空间尺寸,返回字节长度.
		uint32_t hov_space() const
		{
			if (!m_buff || !head().buff_size || !head().capacity)
				return 0;
			return sizeof(head_t) + sizeof(keyoff_t)*head().capacity + sizeof(val_t)*head().val_cnt*head().capacity;
		}
		//-------------------------------------------------
		//计算存放key的空间尺寸,返回字节长度.
		uint32_t key_space() const
		{
			if (!m_buff || !head().buff_size || !head().capacity)
				return 0;
			uint32_t hov = hov_space();
			if (hov == 0 || hov >= head().buff_size)
				return 0;
			return head().buff_size - hov;
		}
		//-------------------------------------------------
		//子类实现,用于空间增长.返回值告知是否扩容成功
		virtual bool on_make_grow() { return false; }
		//-------------------------------------------------
		//子类使用,便于计算所需空间
		static uint32_t calc_space(const uint16_t caps,const uint16_t key_cnt,const uint16_t val_cnt)
		{
			return calc_hat_space(head_t, keyoff_t, caps, key_t, key_cnt, val_t, val_cnt);
		}
	public:
		//-------------------------------------------------
		hat_raw_t() :m_buff(NULL) {}
		virtual ~hat_raw_t() { clear(); }
		//-------------------------------------------------
		//判断查找表是否有效
		bool is_valid() const { return m_buff != NULL && head().capacity != 0; }
		//-------------------------------------------------
		void clear()
		{
			if (m_buff == NULL)
				return;

			if (size())
			{//存在有效元素,进行遍历清理
				uint16_t val_cnt = head().val_cnt;			//得到val值长度
				if (sorted())
				{
					uint16_t s = size();					//排序过的容器,进行有效key范围遍历即可
					for (uint16_t i = 0;i < s; ++i)
					{
						keyoff_t& ko = offset(i);			//得到key偏移,就可得到所需k/v信息
						hat_op::on_key_clear(i, key(ko), ko.key_cnt, value(ko), val_cnt);
					}
				}
				else
				{
					uint16_t s = capacity();				//未排序的容器,需要进行全范围遍历
					for (uint16_t i = 0;i < s; ++i)
					{
						keyoff_t& ko = offset(i);
						if (ko.key_cnt == 0)
							continue;						//无效key跳过不处理
						hat_op::on_key_clear(i, key(ko), ko.key_cnt, value(ko), val_cnt);
					}
				}
			}
			reset();										//状态与数据全部复位
		}
		//-------------------------------------------------
		//获取最大容量
		uint16_t capacity() const { rx_assert(is_valid()); return head().capacity; }
		//-------------------------------------------------
		//获取已有元素数量
		uint16_t size() const { rx_assert(is_valid()); return head().size; }
		//-------------------------------------------------
		//每个val的元素数量
		uint16_t value_cnt() const { rx_assert(is_valid()); return head().val_cnt; }
		//-------------------------------------------------
		//剩余可用的key存储空间
		uint32_t remain() const { rx_assert(is_valid()); return head().buff_size - head().buff_last; }
		//-------------------------------------------------
		//获取key偏移数组
		keyoff_t *offset() const
		{
			rx_assert(is_valid());
			return (keyoff_t*)((uint8_t*)m_buff + sizeof(head_t));
		}
		//按序号获取key偏移信息
		keyoff_t &offset(uint16_t idx) const
		{
			rx_assert(idx < capacity());
			return offset()[idx];
		}
		//-------------------------------------------------
		//按key偏移获取key内容指针
		key_t* key(const keyoff_t &ko) const
		{
			rx_assert(is_valid());
			if (ko.offset == 0)
				return NULL;
			return (key_t*)((uint8_t*)m_buff + ko.offset);
		}
		//-------------------------------------------------
		//按序号获取key内容指针与长度
		//返回值:NULL此位置没有key;其他为key_t数组缓冲区
		key_t* key(uint16_t idx, uint16_t *key_cnt = NULL) const
		{
			rx_assert(is_valid());
			rx_assert(idx < capacity());
			keyoff_t &ko = offset(idx);
			if (key_cnt)
				*key_cnt = ko.key_cnt;
			return key(ko);
		}
		//-------------------------------------------------
		//按key偏移访问val值缓冲区
		//返回值:NULL没有val值;其他为val_t数组缓冲区
		val_t* value(const keyoff_t &ko) const
		{
			rx_assert(is_valid());
			rx_assert(ko.val_idx < capacity());
			if (head().val_cnt == 0)
				return NULL;
			uint8_t *vs = (uint8_t*)m_buff + sizeof(head_t) + sizeof(keyoff_t)*capacity();
			return (val_t*)(vs + ko.val_idx * sizeof(val_t)*head().val_cnt);
		}
		//-------------------------------------------------
		//追加key到容器;出参exist告知key是否已经存在
		//返回值:capacity()追加失败,没有空间了;<capacity()为元素索引
		uint16_t push(const key_t *key, uint16_t key_cnt, bool *exist = NULL)
		{
			rx_assert(is_valid());
			rx_assert(!head().sorted);						//插入的时候需要hash排重,容器不能是排序的

			uint32_t caps = capacity();
			if (head().size >= caps || remain() < sizeof(key_t)*(key_cnt + 1))
			{//进行容量检查与尝试扩容处理
				if (!on_make_grow())
					return caps;
				caps = capacity();
				if (head().size >= caps || remain() < sizeof(key_t)*(key_cnt + 1))
					return caps;
			}

			uint32_t hash_code = hat_op::hash(key, key_cnt);//计算hash码
			for (uint32_t i = 0; i < caps; ++i)
			{//对哈希槽位进行循环顺序查找
				uint16_t idx = (hash_code + i) % caps;      //计算当前位置
				keyoff_t &ko = offset(idx);					//得到对应槽位的偏移信息

				if (ko.offset == 0)
				{//找到空位置了
					head_t &hat = head();					//得到状态头
					ko.offset = hat.buff_last;				//记录key偏移
					ko.key_cnt = key_cnt;					//记录key长度
					uint32_t keysize = sizeof(key_t)*key_cnt;//计算key的字节长度
					memcpy((uint8_t*)m_buff + hat.buff_last, key, keysize);//按字节拷贝key内容
					hat.buff_last += keysize;				//后移key空间指针
					memset((uint8_t*)m_buff + hat.buff_last, 0, sizeof(key_t));//在key内容的后面留下一个0元素
					hat.buff_last += sizeof(key_t);			//后移key空间指针

					ko.val_idx=hat.size++;					//元素总数增加,并记录此时key对应的val位置
					hat_op::on_key_make(idx, key, key_cnt, value(ko), head().val_cnt);
					return idx;
				}
				else
				{//目标槽位被占用了,需判断是否为重复key
					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (hat_op::equ(key, key_cnt, k, kc))
					{
						if (exist)
							*exist = true;
						return idx;                         //该节点已经被使用了,且value也是重复的,那么就直接给出吧
					}
				}
			}
			return capacity();
		}
		//-------------------------------------------------
		//按照指定的完整key查找对应的val值,根据操作策略,内部会进行哈希查找/二分查找/遍历查找
		//返回值:capacity()没找到;<capacity()为元素索引
		uint16_t find(const key_t *key, uint16_t key_cnt) const
		{
			if (sorted())
			{//已排序,使用二分法查找
				bs_cmp_t cmp(*this, key, key_cnt);
				uint32_t idx = bisect<keyoff_t, bs_cmp_t>(offset(), size(), cmp);
				if (idx == size())
					return capacity();
				return idx;
			}
			else
			{//未排序,还是按照哈希表查找
				uint32_t caps = capacity();
				uint32_t hash_code = hat_op::hash(key, key_cnt);	//计算hash码
				for (uint32_t i = 0; i < caps; ++i)
				{
					uint16_t idx = (hash_code + i) % caps;      //计算当前位置
					keyoff_t &ko = offset(idx);					//得到对应槽位的偏移信息

					if (ko.offset == 0)
						break;									//直接就碰到空档了,不用继续了

					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (hat_op::equ(key, key_cnt, k, kc))
						return idx;
				}
			}
			return capacity();
		}
		//-------------------------------------------------
		//尝试找到pos节点后的下一个被使用的节点(跳过中间未被使用的部分)
		//返回值:capacity()没找到;<capacity()为元素索引
		uint16_t next(uint16_t pos) const
		{
			rx_assert(is_valid());
			rx_assert(pos < capacity());

			uint16_t end = capacity();
			if (sorted())
			{
				pos += 1;
				if (pos < size())
					return pos;

			}
			else
			{
				for (uint32_t i = pos + 1;i < end;++i)
				{
					keyoff_t &ko = offset(i);
					if (ko.offset)
						return i;
				}
			}
			return end;
		}
		//-------------------------------------------------
		//尝试按升序进行排序,便于使用二分法查找与前缀查找.排序后就不可以再继续插入新值了.
		//返回值:true已经排序;false之前未排序,本次排序完成.
		bool sort()
		{
			if (sorted())
				return true;
			qs_cmp_t<keyoff_t> cmp(*this);
			quick_sort(offset(), capacity(), cmp);
			head().sorted = 1;
			return false;
		}
		bool sorted() const { return head().sorted != 0; }
		//-------------------------------------------------
		//按指定的key前缀长度进行查找(可指定是否查找左边界)
		//返回值:capacity()没找到;否则为找到的key序号(未必是匹配结果的左边界或右边界)
		uint16_t prefix(const key_t *prekey, uint16_t key_cnt,bool is_left=true) const
		{
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			//用二分法查找指定长度的key前缀,得到一个key偏移.
			bs_cmp_t cmp(*this, prekey, key_cnt, 1);

			uint32_t idx;
			if (is_left)
				idx = bisect_first<keyoff_t, bs_cmp_t>(offset(), size(), cmp);
			else
				idx = bisect<keyoff_t, bs_cmp_t>(offset(), size(), cmp);

			if (idx == size())
				return capacity();
			return idx;
		}
		//-------------------------------------------------
		//以当前索引位置和前缀高度,查探右边界
		//返回值:右边界key索引;无效时返回hat.capacity()
		uint16_t prefix_right(uint16_t curr, uint16_t key_cnt) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}

			uint16_t kl;
			const key_t *kp = key(curr, &kl);				//尝试得到当前索引对应的key指针
			if (kp == NULL || kl < key_cnt)
				return capacity();

			//用二分法,从指定的位置之后查找指定长度的key前缀,得到右侧key偏移.
			bs_cmp_t cmp(*this, kp, key_cnt, 1);
			kl = size() - curr;
			uint16_t idx = bisect_last<keyoff_t, bs_cmp_t>(offset() + curr, kl, cmp);
			rx_assert(idx != kl);
			return curr + idx;
		}
		//-------------------------------------------------
		//以当前索引位置和前缀高度,查探带有新元素的索引位置(模拟tire的递进)
		//返回值:key索引,代表在这个位置的key_cnt+1处,找到了item;无效时返回capacity()
		uint16_t prefix_step(uint16_t curr, uint16_t key_cnt, const key_t& item) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}

			uint16_t kl;
			const key_t *kp = key(curr, &kl);				//尝试得到当前索引对应的key指针
			if (kp == NULL || kl < key_cnt)
				return capacity();

			if (key_cnt + 1 <= kl && kp[key_cnt] == item)
				return curr;								//在当前索引的key上直接判断

			for (uint16_t i = curr + 1;i < size();++i)
			{
				uint16_t l;
				const key_t *k = key(i, &l);				//尝试得到当前索引对应的key指针
				rx_assert(k != NULL);
				rx_assert(l >= key_cnt);
				if (!hat_op::equ(kp, key_cnt, k, key_cnt))
					break;									//向后找,发现前缀的都不同了,没找到
				if (l <= key_cnt || k[key_cnt] != item)
					continue;
				else
					return i;								//找到了
			}
			return capacity();
		}
		//-------------------------------------------------
		//容器内容整体赋值
		hat_raw_t& operator=(const hat_raw_t& src)
		{
			assign(src);
			return *this;
		}
		bool assign(const hat_raw_t& src)
		{
			if (!is_valid()) 
				return false;
			if (capacity() < src.capacity()) 
				return false;
			if (key_space() < src.key_space()) 
				return false;
			if (head().val_cnt < src.head().val_cnt) 
				return false;

			clear();										//先清理本地容器

			uint16_t s = src.capacity();
			uint16_t sval_cnt = src.head().val_cnt;
			for (uint16_t i = 0;i < s;++i)					//对来源容器进行遍历
			{
				keyoff_t &sko = src.offset(i);
				if (sko.offset == 0)
					continue;

				uint16_t idx = push(src.key(sko), sko.key_cnt);
				rx_assert(idx != capacity());				//先复制key内容
				if (sval_cnt)
				{
					keyoff_t &ko = offset(idx);
					val_t *val = value(ko);
					val_t *sval = src.value(sko);
					for (uint16_t j = 0;j < sval_cnt;++j)	//再循环复制值内容
						val[j] = sval[j];
				}
			}
			return true;
		}
	};

	//-----------------------------------------------------
	//hat内部二分搜索需要的,>比较器,需要对keyoff_t和指定的key进行比较
	template<class key_t, class val_t, class hat_op>
	inline int hat_raw_t<key_t, val_t, hat_op>::bs_cmp_t::operator()(const keyoff_t& ko) const
	{
		rx_assert(ko.offset != 0);
		key_t *k = parent.key(ko);
		switch (is_pre)
		{
			case 0:											//全长度比较
				return hat_op::cmp(k, ko.key_cnt, key, key_cnt);
			case 1:
			{
				if (ko.key_cnt < key_cnt)					//前缀长度比较
					return -2;
				return hat_op::cmp(k, key_cnt, key, key_cnt);
			}
		}
		return false;
	}

	//-----------------------------------------------------
	//固定空间的hat表:元素总数;key条目类型,key平均长度;val条目类型,val长度;比较器运算类
	template<uint16_t caps, class key_t, uint32_t key_cnt, class val_t = void*, uint32_t val_cnt = 0, class hat_op = hat_op_t>
	class hat_ft :public hat_raw_t<key_t, val_t, hat_op>
	{
		typedef hat_raw_t<key_t, val_t, hat_op> super_t;
		uint8_t		buff[calc_hat_space(typename super_t::head_t, typename super_t::keyoff_t, caps, key_t, key_cnt, val_t, val_cnt)];
	public:
		hat_ft() { super_t::bind(buff, sizeof(buff), caps, val_cnt); }
	};

	//-----------------------------------------------------
	//动态空间的hat表:key条目类型,key平均长度;val条目类型,val长度;比较器运算类
	template<class key_t, class val_t = void*,  class hat_op = hat_op_t>
	class hat_t :public hat_raw_t<key_t, val_t, hat_op>
	{
		typedef hat_raw_t<key_t, val_t, hat_op> super_t;
		mem_allotter_i	&m_mem;								//内存分配器
		float			m_factor;							//扩容系数
		uint16_t		m_key_cnt;

		//-------------------------------------------------
		//子类实现,用于空间增长.返回值告知是否扩容成功
		virtual bool on_make_grow()
		{
			hat_t new_hat(m_mem);							//新容器,使用本容器的内存分配器
			uint16_t cap = super_t::capacity() + 8;			//控制新容量的最小值
			if (!new_hat.init(cap, m_key_cnt, super_t::head().val_cnt, m_factor))
				return false;								//给新容器分配新的空间
			
			if (!new_hat.assign(*this))						//将当前内容复制到新容器
				return false;

			uninit();										//销毁当前容器
			super_t::m_buff = new_hat.m_buff;				//当前容器的核心数据缓冲区指向新容器
			new_hat.m_buff = NULL;							//新容器放弃对核心缓冲区的管理,即将析构.
			return true;
		}
	public:
		//-------------------------------------------------
		hat_t() :m_mem(rx_mem()) {}
		hat_t(mem_allotter_i &mem) :m_mem(mem) {}
		~hat_t() { uninit(); }
		//-------------------------------------------------
		//进行容器初始化,告知初始容量/平均key长度/val长度/扩容系数
		bool init(uint16_t init_caps, uint16_t key_cnt, uint16_t val_cnt = 0, const float factor=1.2)
		{
			uninit();
			m_key_cnt = key_cnt;

			m_factor = factor;
			if (m_factor <= 1)
				m_factor = (float)1.15;

			uint32_t size = super_t::calc_space(init_caps, key_cnt, val_cnt);
			size = size_align8(size);

			void *buff = m_mem.alloc(size);
			if (!buff)
				return false;
			return super_t::bind(buff, size, init_caps, val_cnt);
		}
		//-------------------------------------------------
		//容器解除,释放全部资源
		void uninit()
		{
			if (super_t::m_buff==NULL)
				return;
			super_t::clear();
			m_mem.free(super_t::m_buff);
			super_t::m_buff = NULL;
		}
		//-------------------------------------------------
	};
}


#endif

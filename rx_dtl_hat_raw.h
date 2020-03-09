#ifndef _RX_DTL_HAT_RAW_H_
#define _RX_DTL_HAT_RAW_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_hash_data.h"
#include "rx_cc_base.h"
#include "rx_dtl_alg.h"

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
	class hat_fun_t
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
	//计算hat占用的空间:头部占用空间 + key偏移数组占用空间 + val占用总空间 + key占用总空间
	#define calc_hat_space(head_t,keyoff_t,capacity,key_t,key_count,val_t,val_cnt) \
		(sizeof(head_t)+ sizeof(keyoff_t)*capacity + sizeof(val_t)*val_cnt*capacity + sizeof(key_t)*(key_count+1)*capacity)

	//-----------------------------------------------------
	//轻量级紧凑查找表
	template<class key_t, class val_t, class kcmp = hat_fun_t>
	class hat_raw_t
	{
	protected:
		struct keyoff_t;
		//-------------------------------------------------
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
			//判断ko==*this
			bool equ(const keyoff_t& ko) const;
			//判断ko>*this
			bool gt(const keyoff_t& ko) const;
		};
		//-------------------------------------------------
		//定义key偏移量所需类型
		typedef struct keyoff_t
		{
			uint32_t	offset;								//key所在的偏移位置
			uint16_t	val_idx;							//key对应的val索引
			uint16_t	key_cnt;							//key的元素数量
			bool operator==(const bs_cmp_t& cmp) const { return cmp.equ(*this); }
			bool operator>(const bs_cmp_t& cmp) const { return cmp.gt(*this); }
		}keyoff_t;
		//-------------------------------------------------
		//定义容器头部信息结构
		typedef struct head_t
		{
			uint32_t    buff_size;                          //底层空间总容量
			uint32_t    buff_last;                          //底层空间最后可以拼装key的位置
			uint16_t	val_cnt;							//每个值占用的元素val_t的数量
			uint16_t	sorted;								//当前容器是否已经排序
			uint16_t	capacity;							//最大可放置的元素数量
			uint16_t	size;								//已经存在的元素数量
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
				return kcmp::cmp(k1, a.key_cnt, k2, b.key_cnt) < 0;
			}
		};
		//-------------------------------------------------

		//底层空间指针
		void*			m_buff;
		//-------------------------------------------------
		head_t& head() const { rx_assert(m_buff != NULL); return *(head_t*)m_buff; }
		//-------------------------------------------------
		//绑定所需空间
		bool bind(void *buff, uint32_t buff_size, uint16_t cap, uint32_t val_cnt)
		{
			if (m_buff)
				return false;
			memset(buff, 0, buff_size);
			m_buff = buff;
			head().buff_size = buff_size;
			head().capacity = cap;
			head().val_cnt = val_cnt;
			head().size = 0;
			head().buff_last = sizeof(head_t) + sizeof(keyoff_t)*cap + sizeof(val_t)*val_cnt*cap;
			return true;
		}
		//-------------------------------------------------
		//子类实现,用于空间增长.返回值告知是否扩容成功
		virtual bool make_grow() { return false; }
	public:
		//-------------------------------------------------
		hat_raw_t() :m_buff(NULL) {}
		virtual ~hat_raw_t() {}
		//-------------------------------------------------
		//判断查找表是否有效
		bool is_valid() const { return m_buff != NULL && head().capacity != 0; }
		//-------------------------------------------------
		//获取最大容量
		uint16_t capacity() const { rx_assert(is_valid()); return head().capacity; }
		//-------------------------------------------------
		//获取已有元素数量
		uint16_t size() const { rx_assert(is_valid()); return head().size; }
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
				if (!make_grow())
					return caps;
				caps = capacity();
				if (head().size >= caps || remain() < sizeof(key_t)*(key_cnt + 1))
					return caps;
			}

			uint32_t hash_code = kcmp::hash(key, key_cnt);	//计算hash码
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

					++hat.size;								//元素总数增加
					return idx;
				}
				else
				{//目标槽位被占用了,需判断是否为重复key
					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (kcmp::equ(key, key_cnt, k, kc))
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
				uint32_t hash_code = kcmp::hash(key, key_cnt);	//计算hash码
				for (uint32_t i = 0; i < caps; ++i)
				{
					uint16_t idx = (hash_code + i) % caps;      //计算当前位置
					keyoff_t &ko = offset(idx);					//得到对应槽位的偏移信息

					if (ko.offset == 0)
						break;									//直接就碰到空档了,不用继续了

					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (kcmp::equ(key, key_cnt, k, kc))
						return idx;
				}
			}
			return capacity();
		}
		//-------------------------------------------------
		//尝试找到pos节点后的下一个被使用的节点(跳过中间未被使用的部分)
		//返回值:capacity()没找到;<capacity()为元素索引
		uint32_t next(uint32_t pos) const
		{
			rx_assert(is_valid());
			rx_assert(pos < capacity());

			uint32_t end = capacity();
			for (uint32_t i = pos + 1;i < end;++i)
			{
				keyoff_t &ko = offset(i);
				if (ko.offset)
					return i;
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
		//按指定的key前缀长度进行查找
		//返回值:capacity()没找到;否则为找到的key序号(未必是匹配结果的左边界或右边界)
		uint16_t prefix(const key_t *prekey, uint16_t key_cnt) const
		{
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			//用二分法查找指定长度的key前缀,得到一个key偏移.
			bs_cmp_t cmp(*this, prekey, key_cnt, 1);
			uint32_t idx = bisect<keyoff_t, bs_cmp_t>(offset(), size(), cmp);
			if (idx == size())
				return capacity();
			return idx;
		}
		//-------------------------------------------------
		//以当前索引位置和前缀高度,查探左边界
		//返回值:左边界key索引;无效时返回hat.capacity()
		uint16_t prefix_left(uint16_t curr, uint16_t key_cnt) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			const key_t *kp = key(curr);					//尝试得到当前索引对应的key指针
			if (kp == NULL)
				return capacity();

			for (int32_t i = curr - 1;i >= 0;--i)			//从当前索引的左边开始向左遍历
			{
				uint16_t kl;
				const key_t *k = key(i, &kl);
				if (kl < key_cnt || !kcmp::equ(k, key_cnt, kp, key_cnt))
					return i + 1;							//左向元素的前缀与指定前缀不同了,返回最后匹配的位置
			}
			return curr;
		}
		//-------------------------------------------------
		//以当前索引位置和前缀高度,查探并更新右边界
		//返回值:右边界key索引;无效时返回hat.capacity()
		uint16_t prefix_right(uint16_t curr, uint16_t key_cnt) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			const key_t *kp = key(curr);					//尝试得到当前索引对应的key指针
			if (kp == NULL)
				return capacity();

			for (int32_t i = curr + 1;i < capacity();++i)	//从当前索引的右边开始向右遍历
			{
				uint16_t kl;
				const key_t *k = key(i, &kl);
				if (kl < key_cnt || !kcmp::equ(k, key_cnt, kp, key_cnt))
					return i - 1;							//右向元素的前缀与指定前缀不同了,返回最后匹配的位置
			}
			return curr;
		}
	};

	//-----------------------------------------------------
	//hat内部二分搜索需要的,==比较器,需要对keyoff_t和指定的key进行比较
	template<class key_t, class val_t, class kcmp>
	inline bool hat_raw_t<key_t, val_t, kcmp>::bs_cmp_t::equ(const keyoff_t& ko) const
	{
		rx_assert(ko.offset != 0);
		key_t *k = parent.key(ko);
		switch (is_pre)
		{
			case 0: return kcmp::equ(k, ko.key_cnt, key, key_cnt);	//key要求进行完全比较
			case 1:
			{
				if (ko.key_cnt < key_cnt)
					return false;
				return kcmp::equ(k, key_cnt, key, key_cnt);			//key要求进行前缀比较
			}
		}
		return false;
	}
	//-----------------------------------------------------
	//hat内部二分搜索需要的,>比较器,需要对keyoff_t和指定的key进行比较
	template<class key_t, class val_t, class kcmp>
	inline bool hat_raw_t<key_t, val_t, kcmp>::bs_cmp_t::gt(const keyoff_t& ko) const
	{
		rx_assert(ko.offset != 0);
		key_t *k = parent.key(ko);
		switch (is_pre)
		{
			case 0:return kcmp::cmp(k, ko.key_cnt, key, key_cnt) > 0;
			case 1:
			{
				if (ko.key_cnt < key_cnt)
					return false;
				return kcmp::cmp(k, key_cnt, key, key_cnt) > 0;
			}
		}
		return false;
	}

	//-----------------------------------------------------
	//固定空间的hat表:元素总数;key条目类型,key平均长度;val条目类型,val长度;比较器运算类
	template<uint16_t caps, class key_t, uint32_t key_cnt, class val_t = void*, uint32_t val_cnt = 0, class kcmp = hat_fun_t>
	class hat_ft :public hat_raw_t<key_t, val_t, kcmp>
	{
		typedef hat_raw_t<key_t, val_t, kcmp> super_t;
		uint8_t		buff[calc_hat_space(typename super_t::head_t, typename super_t::keyoff_t, caps, key_t, key_cnt, val_t, val_cnt)];
	public:
		hat_ft() { super_t::bind(buff, sizeof(buff), caps, val_cnt); }
	};

}


#endif

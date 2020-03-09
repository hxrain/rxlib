#ifndef _RX_DTL_HAT_RAW_H_
#define _RX_DTL_HAT_RAW_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_hash_data.h"
#include "rx_cc_base.h"
#include "rx_dtl_alg.h"

/*
	hash array table(hat)
	����һ�����ڿ��ټ�������������������,����array��Ϊ�ײ�,��ʵ��hash/bst�Ȳ��ҷ���
	key��val������Ϊ����(char*,wchar_t*,uint8_t*,uint32_t*,struct*��)
	Ϊ�˼����,val��size��Ҫ�̶�,����û��val�Ĳ���;���ṩɾ��Ԫ�صĹ���;
	�ײ����ݲ��ָ�ʽΪ:head|keyƫ������|ֵ����|key����
		head|keyoff1|keyoff2|...|val1|val2|...|key1/0|key2/0|.../0
	head�е���Ϣ��:���Ԫ������/����Ԫ������/ֵ�ߴ�/�ռ�����/���ÿռ����λ��
	�����ڲ�֧�����ֲ��ҷ���:Ĭ��Ϊhash��,δ����;�����,����,���ַ�;��������ǰ׺�����ȽϿ�.
*/
namespace rx
{
	//-----------------------------------------------------
	//hat������
	class hat_fun_t
	{
	public:
		//-------------------------------------------------
		//hash����
		template<class KT>
		static uint32_t hash(const KT *k, uint32_t cnt) { return rx_hash_murmur(k, sizeof(k)*cnt); }
		static uint32_t hash(const char *k, uint32_t cnt) { return rx_hash_zob(k); }
		static uint32_t hash(const wchar_t *k, uint32_t cnt) { return rx_hash_zob(k); }

		//-------------------------------------------------
		//������key�ıȽϺ���:�Ƿ���ͬ
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
		//��Ԫ�����ݱȽϲ�����������key,����ֵ:<0,k1<k2;=0,k1=k2;>0,k1>k2
		template<class KT>
		static int cmp(const KT* k1, uint16_t k1_cnt, const KT* k2, uint16_t k2_cnt)
		{
			uint16_t cnt = Min(k1_cnt, k2_cnt);				//�Ȱ�����key����̳��Ƚ������ݱȽ�
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
				return 0;									//�������key�ĳ�����ͬ������Ҳ��ͬ,���������������ͬ.
			return k1_cnt < k2_cnt ? -2 : 2;				//����key��ǰ׺��ȫ��ͬʱ,��̵�key��С��.
		}
	};

	//-----------------------------------------------------
	//����hatռ�õĿռ�:ͷ��ռ�ÿռ� + keyƫ������ռ�ÿռ� + valռ���ܿռ� + keyռ���ܿռ�
	#define calc_hat_space(head_t,keyoff_t,capacity,key_t,key_count,val_t,val_cnt) \
		(sizeof(head_t)+ sizeof(keyoff_t)*capacity + sizeof(val_t)*val_cnt*capacity + sizeof(key_t)*(key_count+1)*capacity)

	//-----------------------------------------------------
	//���������ղ��ұ�
	template<class key_t, class val_t, class kcmp = hat_fun_t>
	class hat_raw_t
	{
	protected:
		struct keyoff_t;
		//-------------------------------------------------
		//����������Ҫ��,==��>�Ƚ���,��Ҫ��keyoff_t��ָ����key���бȽ�
		class bs_cmp_t
		{
			const hat_raw_t &parent;						//hat����,���ڷ������ڲ�����������
			const key_t		*key;							//���Ƚϵ�key
			uint16_t		key_cnt;						//���Ƚ�key�ĳ���
			uint16_t		is_pre;							//�Ƿ�Ϊǰ׺ģʽ:0-keyȫ��ģʽ;1-keyǰ׺����;2-keyǰ׺��������
		public:
			const key_t		*item;							//ǰ׺��������ʱ��������Ŀ
			bs_cmp_t(const hat_raw_t &p, const key_t *k, uint16_t kc, uint16_t is_pre = 0) :parent(p), key(k), key_cnt(kc), is_pre(is_pre), item(NULL) {}
			//�ж�ko==*this
			bool equ(const keyoff_t& ko) const;
			//�ж�ko>*this
			bool gt(const keyoff_t& ko) const;
		};
		//-------------------------------------------------
		//����keyƫ������������
		typedef struct keyoff_t
		{
			uint32_t	offset;								//key���ڵ�ƫ��λ��
			uint16_t	val_idx;							//key��Ӧ��val����
			uint16_t	key_cnt;							//key��Ԫ������
			bool operator==(const bs_cmp_t& cmp) const { return cmp.equ(*this); }
			bool operator>(const bs_cmp_t& cmp) const { return cmp.gt(*this); }
		}keyoff_t;
		//-------------------------------------------------
		//��������ͷ����Ϣ�ṹ
		typedef struct head_t
		{
			uint32_t    buff_size;                          //�ײ�ռ�������
			uint32_t    buff_last;                          //�ײ�ռ�������ƴװkey��λ��
			uint16_t	val_cnt;							//ÿ��ֵռ�õ�Ԫ��val_t������
			uint16_t	sorted;								//��ǰ�����Ƿ��Ѿ�����
			uint16_t	capacity;							//���ɷ��õ�Ԫ������
			uint16_t	size;								//�Ѿ����ڵ�Ԫ������
		}head_t;

		//-------------------------------------------------
		//������Ҫ��,С�ڱȽ���,���a<b�򷵻���
		template<class DT>
		class qs_cmp_t
		{
			hat_raw_t &parent;
		public:
			qs_cmp_t(hat_raw_t &p) :parent(p) {}
			bool operator()(const DT &a, const DT &b) const
			{
				uint8_t ei = a.offset == 0 ? 0 : 2;			//aԪ��������ei��,Ҫô��0b00Ҫô��0b10
				ei |= b.offset == 0 ? 0 : 1;				//bԪ�ص�����ei��,�����0b00/0b10/0b01/0b11
				switch (ei)
				{
				case 0:return true;							//a��b���ǿղ�λ,��Ϊa<b
				case 1:return false;						//a�ǿղ�λ,b��ֵ,��Ϊa>=b
				case 2:return true;							//a��ֵ,b�ǿղ�λ,��Ϊa<b
				}
				//����,a��b����ֵ,��Ҫ�Ƚ�key��
				key_t *k1 = parent.key(a);
				key_t *k2 = parent.key(b);
				return kcmp::cmp(k1, a.key_cnt, k2, b.key_cnt) < 0;
			}
		};
		//-------------------------------------------------

		//�ײ�ռ�ָ��
		void*			m_buff;
		//-------------------------------------------------
		head_t& head() const { rx_assert(m_buff != NULL); return *(head_t*)m_buff; }
		//-------------------------------------------------
		//������ռ�
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
		//����ʵ��,���ڿռ�����.����ֵ��֪�Ƿ����ݳɹ�
		virtual bool make_grow() { return false; }
	public:
		//-------------------------------------------------
		hat_raw_t() :m_buff(NULL) {}
		virtual ~hat_raw_t() {}
		//-------------------------------------------------
		//�жϲ��ұ��Ƿ���Ч
		bool is_valid() const { return m_buff != NULL && head().capacity != 0; }
		//-------------------------------------------------
		//��ȡ�������
		uint16_t capacity() const { rx_assert(is_valid()); return head().capacity; }
		//-------------------------------------------------
		//��ȡ����Ԫ������
		uint16_t size() const { rx_assert(is_valid()); return head().size; }
		//-------------------------------------------------
		//ʣ����õ�key�洢�ռ�
		uint32_t remain() const { rx_assert(is_valid()); return head().buff_size - head().buff_last; }
		//-------------------------------------------------
		//��ȡkeyƫ������
		keyoff_t *offset() const
		{
			rx_assert(is_valid());
			return (keyoff_t*)((uint8_t*)m_buff + sizeof(head_t));
		}
		//����Ż�ȡkeyƫ����Ϣ
		keyoff_t &offset(uint16_t idx) const
		{
			rx_assert(idx < capacity());
			return offset()[idx];
		}
		//-------------------------------------------------
		//��keyƫ�ƻ�ȡkey����ָ��
		key_t* key(const keyoff_t &ko) const
		{
			rx_assert(is_valid());
			if (ko.offset == 0)
				return NULL;
			return (key_t*)((uint8_t*)m_buff + ko.offset);
		}
		//-------------------------------------------------
		//����Ż�ȡkey����ָ���볤��
		//����ֵ:NULL��λ��û��key;����Ϊkey_t���黺����
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
		//��keyƫ�Ʒ���valֵ������
		//����ֵ:NULLû��valֵ;����Ϊval_t���黺����
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
		//׷��key������;����exist��֪key�Ƿ��Ѿ�����
		//����ֵ:capacity()׷��ʧ��,û�пռ���;<capacity()ΪԪ������
		uint16_t push(const key_t *key, uint16_t key_cnt, bool *exist = NULL)
		{
			rx_assert(is_valid());
			rx_assert(!head().sorted);						//�����ʱ����Ҫhash����,���������������

			uint32_t caps = capacity();
			if (head().size >= caps || remain() < sizeof(key_t)*(key_cnt + 1))
			{//������������볢�����ݴ���
				if (!make_grow())
					return caps;
				caps = capacity();
				if (head().size >= caps || remain() < sizeof(key_t)*(key_cnt + 1))
					return caps;
			}

			uint32_t hash_code = kcmp::hash(key, key_cnt);	//����hash��
			for (uint32_t i = 0; i < caps; ++i)
			{//�Թ�ϣ��λ����ѭ��˳�����
				uint16_t idx = (hash_code + i) % caps;      //���㵱ǰλ��
				keyoff_t &ko = offset(idx);					//�õ���Ӧ��λ��ƫ����Ϣ

				if (ko.offset == 0)
				{//�ҵ���λ����
					head_t &hat = head();					//�õ�״̬ͷ
					ko.offset = hat.buff_last;				//��¼keyƫ��
					ko.key_cnt = key_cnt;					//��¼key����
					uint32_t keysize = sizeof(key_t)*key_cnt;//����key���ֽڳ���
					memcpy((uint8_t*)m_buff + hat.buff_last, key, keysize);//���ֽڿ���key����
					hat.buff_last += keysize;				//����key�ռ�ָ��
					memset((uint8_t*)m_buff + hat.buff_last, 0, sizeof(key_t));//��key���ݵĺ�������һ��0Ԫ��
					hat.buff_last += sizeof(key_t);			//����key�ռ�ָ��

					++hat.size;								//Ԫ����������
					return idx;
				}
				else
				{//Ŀ���λ��ռ����,���ж��Ƿ�Ϊ�ظ�key
					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (kcmp::equ(key, key_cnt, k, kc))
					{
						if (exist)
							*exist = true;
						return idx;                         //�ýڵ��Ѿ���ʹ����,��valueҲ���ظ���,��ô��ֱ�Ӹ�����
					}
				}
			}
			return capacity();
		}
		//-------------------------------------------------
		//����ָ��������key���Ҷ�Ӧ��valֵ,���ݲ�������,�ڲ�����й�ϣ����/���ֲ���/��������
		//����ֵ:capacity()û�ҵ�;<capacity()ΪԪ������
		uint16_t find(const key_t *key, uint16_t key_cnt) const
		{
			if (sorted())
			{//������,ʹ�ö��ַ�����
				bs_cmp_t cmp(*this, key, key_cnt);
				uint32_t idx = bisect<keyoff_t, bs_cmp_t>(offset(), size(), cmp);
				if (idx == size())
					return capacity();
				return idx;
			}
			else
			{//δ����,���ǰ��չ�ϣ�����
				uint32_t caps = capacity();
				uint32_t hash_code = kcmp::hash(key, key_cnt);	//����hash��
				for (uint32_t i = 0; i < caps; ++i)
				{
					uint16_t idx = (hash_code + i) % caps;      //���㵱ǰλ��
					keyoff_t &ko = offset(idx);					//�õ���Ӧ��λ��ƫ����Ϣ

					if (ko.offset == 0)
						break;									//ֱ�Ӿ������յ���,���ü�����

					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (kcmp::equ(key, key_cnt, k, kc))
						return idx;
				}
			}
			return capacity();
		}
		//-------------------------------------------------
		//�����ҵ�pos�ڵ�����һ����ʹ�õĽڵ�(�����м�δ��ʹ�õĲ���)
		//����ֵ:capacity()û�ҵ�;<capacity()ΪԪ������
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
		//���԰������������,����ʹ�ö��ַ�������ǰ׺����.�����Ͳ������ټ���������ֵ��.
		//����ֵ:true�Ѿ�����;false֮ǰδ����,�����������.
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
		//��ָ����keyǰ׺���Ƚ��в���
		//����ֵ:capacity()û�ҵ�;����Ϊ�ҵ���key���(δ����ƥ��������߽���ұ߽�)
		uint16_t prefix(const key_t *prekey, uint16_t key_cnt) const
		{
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			//�ö��ַ�����ָ�����ȵ�keyǰ׺,�õ�һ��keyƫ��.
			bs_cmp_t cmp(*this, prekey, key_cnt, 1);
			uint32_t idx = bisect<keyoff_t, bs_cmp_t>(offset(), size(), cmp);
			if (idx == size())
				return capacity();
			return idx;
		}
		//-------------------------------------------------
		//�Ե�ǰ����λ�ú�ǰ׺�߶�,��̽��߽�
		//����ֵ:��߽�key����;��Чʱ����hat.capacity()
		uint16_t prefix_left(uint16_t curr, uint16_t key_cnt) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			const key_t *kp = key(curr);					//���Եõ���ǰ������Ӧ��keyָ��
			if (kp == NULL)
				return capacity();

			for (int32_t i = curr - 1;i >= 0;--i)			//�ӵ�ǰ��������߿�ʼ�������
			{
				uint16_t kl;
				const key_t *k = key(i, &kl);
				if (kl < key_cnt || !kcmp::equ(k, key_cnt, kp, key_cnt))
					return i + 1;							//����Ԫ�ص�ǰ׺��ָ��ǰ׺��ͬ��,�������ƥ���λ��
			}
			return curr;
		}
		//-------------------------------------------------
		//�Ե�ǰ����λ�ú�ǰ׺�߶�,��̽�������ұ߽�
		//����ֵ:�ұ߽�key����;��Чʱ����hat.capacity()
		uint16_t prefix_right(uint16_t curr, uint16_t key_cnt) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			const key_t *kp = key(curr);					//���Եõ���ǰ������Ӧ��keyָ��
			if (kp == NULL)
				return capacity();

			for (int32_t i = curr + 1;i < capacity();++i)	//�ӵ�ǰ�������ұ߿�ʼ���ұ���
			{
				uint16_t kl;
				const key_t *k = key(i, &kl);
				if (kl < key_cnt || !kcmp::equ(k, key_cnt, kp, key_cnt))
					return i - 1;							//����Ԫ�ص�ǰ׺��ָ��ǰ׺��ͬ��,�������ƥ���λ��
			}
			return curr;
		}
	};

	//-----------------------------------------------------
	//hat�ڲ�����������Ҫ��,==�Ƚ���,��Ҫ��keyoff_t��ָ����key���бȽ�
	template<class key_t, class val_t, class kcmp>
	inline bool hat_raw_t<key_t, val_t, kcmp>::bs_cmp_t::equ(const keyoff_t& ko) const
	{
		rx_assert(ko.offset != 0);
		key_t *k = parent.key(ko);
		switch (is_pre)
		{
			case 0: return kcmp::equ(k, ko.key_cnt, key, key_cnt);	//keyҪ�������ȫ�Ƚ�
			case 1:
			{
				if (ko.key_cnt < key_cnt)
					return false;
				return kcmp::equ(k, key_cnt, key, key_cnt);			//keyҪ�����ǰ׺�Ƚ�
			}
		}
		return false;
	}
	//-----------------------------------------------------
	//hat�ڲ�����������Ҫ��,>�Ƚ���,��Ҫ��keyoff_t��ָ����key���бȽ�
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
	//�̶��ռ��hat��:Ԫ������;key��Ŀ����,keyƽ������;val��Ŀ����,val����;�Ƚ���������
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

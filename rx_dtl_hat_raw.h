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
	class hat_baseop_t
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
	//����˻�������,����չ���������¼���hat��������
	class hat_op_t:public hat_baseop_t
	{
	public:
		//-------------------------------------------------
		//����ʵ��,��������������֮ǰ����k/v����������
		template<class KT,class VT>
		static void on_key_clear(uint16_t idx, const KT* key, uint16_t key_cnt, VT *val, uint16_t val_cnt) {}
		//����ʵ��,��֪һ����key������,���ڽ���k/v�ĳ�ʼ������
		template<class KT, class VT>
		static void on_key_make(uint16_t idx, const KT* key, uint16_t key_cnt, VT *val, uint16_t val_cnt) {}
	};

	//-----------------------------------------------------
	//����hatռ�õĿռ�:ͷ��ռ�ÿռ� + keyƫ������ռ�ÿռ� + valռ���ܿռ� + keyռ���ܿռ�
	#define calc_hat_space(head_t,keyoff_t,capacity,key_t,key_count,val_t,val_cnt) \
		(sizeof(head_t)+ sizeof(keyoff_t)*capacity + sizeof(val_t)*val_cnt*capacity + sizeof(key_t)*(key_count+1)*capacity)

	//-----------------------------------------------------
	//���������ղ��ұ�
	template<class key_t, class val_t, class hat_op = hat_op_t>
	class hat_raw_t
	{
	public:
		//-------------------------------------------------
		struct keyoff_t;
	protected:
		//-------------------------------------------------
		//�ײ�ռ�ָ��,Ψһ������ݵĵط�
		void*				m_buff;

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
			//�ж�ko��*this�Ĺ�ϵ:<0,0,>0
			int operator()(const keyoff_t& ko) const;
		};
		//-------------------------------------------------
		//����keyƫ������������
		typedef struct keyoff_t
		{
			uint32_t		offset;							//key���ڵ�ƫ��λ��
			uint16_t		val_idx;						//key��Ӧ��val����
			uint16_t		key_cnt;						//key��Ԫ������
			bool operator==(const bs_cmp_t& cmp) const { return cmp(*this)==0; }
			bool operator>(const bs_cmp_t& cmp) const { return cmp(*this)>0; }
			bool operator<(const bs_cmp_t& cmp) const { return cmp(*this)<0; }
			bool operator<=(const bs_cmp_t& cmp) const { return cmp(*this)<=0; }
		}keyoff_t;
		//-------------------------------------------------
		//��������ͷ����Ϣ�ṹ
		typedef struct head_t
		{
			uint32_t		buff_size;                      //�ײ�ռ�������
			uint32_t		buff_last;                      //�ײ�ռ�������ƴװkey��λ��
			uint16_t		val_cnt;						//ÿ��ֵռ�õ�Ԫ��val_t������
			uint16_t		capacity;						//���ɷ��õ�Ԫ������
			uint16_t		sorted;							//��ǰ�����Ƿ��Ѿ�����
			uint16_t		size;							//�Ѿ����ڵ�Ԫ������
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
				return hat_op::cmp(k1, a.key_cnt, k2, b.key_cnt) < 0;
			}
		};
		//-------------------------------------------------
		head_t& head() const { rx_assert(m_buff != NULL); return *(head_t*)m_buff; }
		//-------------------------------------------------
		//������ռ�,�����г�ʼ��
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
		//״̬��λ
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
		//������head/offset/values�Ŀռ�ߴ�,�����ֽڳ���.
		uint32_t hov_space() const
		{
			if (!m_buff || !head().buff_size || !head().capacity)
				return 0;
			return sizeof(head_t) + sizeof(keyoff_t)*head().capacity + sizeof(val_t)*head().val_cnt*head().capacity;
		}
		//-------------------------------------------------
		//������key�Ŀռ�ߴ�,�����ֽڳ���.
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
		//����ʵ��,���ڿռ�����.����ֵ��֪�Ƿ����ݳɹ�
		virtual bool on_make_grow() { return false; }
		//-------------------------------------------------
		//����ʹ��,���ڼ�������ռ�
		static uint32_t calc_space(const uint16_t caps,const uint16_t key_cnt,const uint16_t val_cnt)
		{
			return calc_hat_space(head_t, keyoff_t, caps, key_t, key_cnt, val_t, val_cnt);
		}
	public:
		//-------------------------------------------------
		hat_raw_t() :m_buff(NULL) {}
		virtual ~hat_raw_t() { clear(); }
		//-------------------------------------------------
		//�жϲ��ұ��Ƿ���Ч
		bool is_valid() const { return m_buff != NULL && head().capacity != 0; }
		//-------------------------------------------------
		void clear()
		{
			if (m_buff == NULL)
				return;

			if (size())
			{//������ЧԪ��,���б�������
				uint16_t val_cnt = head().val_cnt;			//�õ�valֵ����
				if (sorted())
				{
					uint16_t s = size();					//�����������,������Чkey��Χ��������
					for (uint16_t i = 0;i < s; ++i)
					{
						keyoff_t& ko = offset(i);			//�õ�keyƫ��,�Ϳɵõ�����k/v��Ϣ
						hat_op::on_key_clear(i, key(ko), ko.key_cnt, value(ko), val_cnt);
					}
				}
				else
				{
					uint16_t s = capacity();				//δ���������,��Ҫ����ȫ��Χ����
					for (uint16_t i = 0;i < s; ++i)
					{
						keyoff_t& ko = offset(i);
						if (ko.key_cnt == 0)
							continue;						//��Чkey����������
						hat_op::on_key_clear(i, key(ko), ko.key_cnt, value(ko), val_cnt);
					}
				}
			}
			reset();										//״̬������ȫ����λ
		}
		//-------------------------------------------------
		//��ȡ�������
		uint16_t capacity() const { rx_assert(is_valid()); return head().capacity; }
		//-------------------------------------------------
		//��ȡ����Ԫ������
		uint16_t size() const { rx_assert(is_valid()); return head().size; }
		//-------------------------------------------------
		//ÿ��val��Ԫ������
		uint16_t value_cnt() const { rx_assert(is_valid()); return head().val_cnt; }
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
				if (!on_make_grow())
					return caps;
				caps = capacity();
				if (head().size >= caps || remain() < sizeof(key_t)*(key_cnt + 1))
					return caps;
			}

			uint32_t hash_code = hat_op::hash(key, key_cnt);//����hash��
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

					ko.val_idx=hat.size++;					//Ԫ����������,����¼��ʱkey��Ӧ��valλ��
					hat_op::on_key_make(idx, key, key_cnt, value(ko), head().val_cnt);
					return idx;
				}
				else
				{//Ŀ���λ��ռ����,���ж��Ƿ�Ϊ�ظ�key
					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (hat_op::equ(key, key_cnt, k, kc))
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
				uint32_t hash_code = hat_op::hash(key, key_cnt);	//����hash��
				for (uint32_t i = 0; i < caps; ++i)
				{
					uint16_t idx = (hash_code + i) % caps;      //���㵱ǰλ��
					keyoff_t &ko = offset(idx);					//�õ���Ӧ��λ��ƫ����Ϣ

					if (ko.offset == 0)
						break;									//ֱ�Ӿ������յ���,���ü�����

					uint16_t kc;
					key_t *k = this->key(idx, &kc);
					if (hat_op::equ(key, key_cnt, k, kc))
						return idx;
				}
			}
			return capacity();
		}
		//-------------------------------------------------
		//�����ҵ�pos�ڵ�����һ����ʹ�õĽڵ�(�����м�δ��ʹ�õĲ���)
		//����ֵ:capacity()û�ҵ�;<capacity()ΪԪ������
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
		//��ָ����keyǰ׺���Ƚ��в���(��ָ���Ƿ������߽�)
		//����ֵ:capacity()û�ҵ�;����Ϊ�ҵ���key���(δ����ƥ��������߽���ұ߽�)
		uint16_t prefix(const key_t *prekey, uint16_t key_cnt,bool is_left=true) const
		{
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}
			//�ö��ַ�����ָ�����ȵ�keyǰ׺,�õ�һ��keyƫ��.
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
		//�Ե�ǰ����λ�ú�ǰ׺�߶�,��̽�ұ߽�
		//����ֵ:�ұ߽�key����;��Чʱ����hat.capacity()
		uint16_t prefix_right(uint16_t curr, uint16_t key_cnt) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}

			uint16_t kl;
			const key_t *kp = key(curr, &kl);				//���Եõ���ǰ������Ӧ��keyָ��
			if (kp == NULL || kl < key_cnt)
				return capacity();

			//�ö��ַ�,��ָ����λ��֮�����ָ�����ȵ�keyǰ׺,�õ��Ҳ�keyƫ��.
			bs_cmp_t cmp(*this, kp, key_cnt, 1);
			kl = size() - curr;
			uint16_t idx = bisect_last<keyoff_t, bs_cmp_t>(offset() + curr, kl, cmp);
			rx_assert(idx != kl);
			return curr + idx;
		}
		//-------------------------------------------------
		//�Ե�ǰ����λ�ú�ǰ׺�߶�,��̽������Ԫ�ص�����λ��(ģ��tire�ĵݽ�)
		//����ֵ:key����,���������λ�õ�key_cnt+1��,�ҵ���item;��Чʱ����capacity()
		uint16_t prefix_step(uint16_t curr, uint16_t key_cnt, const key_t& item) const
		{
			rx_assert(is_valid());
			if (!sorted())
			{
				rx_alert("hat.prefix() need sorted.");
				return capacity();
			}

			uint16_t kl;
			const key_t *kp = key(curr, &kl);				//���Եõ���ǰ������Ӧ��keyָ��
			if (kp == NULL || kl < key_cnt)
				return capacity();

			if (key_cnt + 1 <= kl && kp[key_cnt] == item)
				return curr;								//�ڵ�ǰ������key��ֱ���ж�

			for (uint16_t i = curr + 1;i < size();++i)
			{
				uint16_t l;
				const key_t *k = key(i, &l);				//���Եõ���ǰ������Ӧ��keyָ��
				rx_assert(k != NULL);
				rx_assert(l >= key_cnt);
				if (!hat_op::equ(kp, key_cnt, k, key_cnt))
					break;									//�����,����ǰ׺�Ķ���ͬ��,û�ҵ�
				if (l <= key_cnt || k[key_cnt] != item)
					continue;
				else
					return i;								//�ҵ���
			}
			return capacity();
		}
		//-------------------------------------------------
		//�����������帳ֵ
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

			clear();										//������������

			uint16_t s = src.capacity();
			uint16_t sval_cnt = src.head().val_cnt;
			for (uint16_t i = 0;i < s;++i)					//����Դ�������б���
			{
				keyoff_t &sko = src.offset(i);
				if (sko.offset == 0)
					continue;

				uint16_t idx = push(src.key(sko), sko.key_cnt);
				rx_assert(idx != capacity());				//�ȸ���key����
				if (sval_cnt)
				{
					keyoff_t &ko = offset(idx);
					val_t *val = value(ko);
					val_t *sval = src.value(sko);
					for (uint16_t j = 0;j < sval_cnt;++j)	//��ѭ������ֵ����
						val[j] = sval[j];
				}
			}
			return true;
		}
	};

	//-----------------------------------------------------
	//hat�ڲ�����������Ҫ��,>�Ƚ���,��Ҫ��keyoff_t��ָ����key���бȽ�
	template<class key_t, class val_t, class hat_op>
	inline int hat_raw_t<key_t, val_t, hat_op>::bs_cmp_t::operator()(const keyoff_t& ko) const
	{
		rx_assert(ko.offset != 0);
		key_t *k = parent.key(ko);
		switch (is_pre)
		{
			case 0:											//ȫ���ȱȽ�
				return hat_op::cmp(k, ko.key_cnt, key, key_cnt);
			case 1:
			{
				if (ko.key_cnt < key_cnt)					//ǰ׺���ȱȽ�
					return -2;
				return hat_op::cmp(k, key_cnt, key, key_cnt);
			}
		}
		return false;
	}

	//-----------------------------------------------------
	//�̶��ռ��hat��:Ԫ������;key��Ŀ����,keyƽ������;val��Ŀ����,val����;�Ƚ���������
	template<uint16_t caps, class key_t, uint32_t key_cnt, class val_t = void*, uint32_t val_cnt = 0, class hat_op = hat_op_t>
	class hat_ft :public hat_raw_t<key_t, val_t, hat_op>
	{
		typedef hat_raw_t<key_t, val_t, hat_op> super_t;
		uint8_t		buff[calc_hat_space(typename super_t::head_t, typename super_t::keyoff_t, caps, key_t, key_cnt, val_t, val_cnt)];
	public:
		hat_ft() { super_t::bind(buff, sizeof(buff), caps, val_cnt); }
	};

	//-----------------------------------------------------
	//��̬�ռ��hat��:key��Ŀ����,keyƽ������;val��Ŀ����,val����;�Ƚ���������
	template<class key_t, class val_t = void*,  class hat_op = hat_op_t>
	class hat_t :public hat_raw_t<key_t, val_t, hat_op>
	{
		typedef hat_raw_t<key_t, val_t, hat_op> super_t;
		mem_allotter_i	&m_mem;								//�ڴ������
		float			m_factor;							//����ϵ��
		uint16_t		m_key_cnt;

		//-------------------------------------------------
		//����ʵ��,���ڿռ�����.����ֵ��֪�Ƿ����ݳɹ�
		virtual bool on_make_grow()
		{
			hat_t new_hat(m_mem);							//������,ʹ�ñ��������ڴ������
			uint16_t cap = super_t::capacity() + 8;			//��������������Сֵ
			if (!new_hat.init(cap, m_key_cnt, super_t::head().val_cnt, m_factor))
				return false;								//�������������µĿռ�
			
			if (!new_hat.assign(*this))						//����ǰ���ݸ��Ƶ�������
				return false;

			uninit();										//���ٵ�ǰ����
			super_t::m_buff = new_hat.m_buff;				//��ǰ�����ĺ������ݻ�����ָ��������
			new_hat.m_buff = NULL;							//�����������Ժ��Ļ������Ĺ���,��������.
			return true;
		}
	public:
		//-------------------------------------------------
		hat_t() :m_mem(rx_mem()) {}
		hat_t(mem_allotter_i &mem) :m_mem(mem) {}
		~hat_t() { uninit(); }
		//-------------------------------------------------
		//����������ʼ��,��֪��ʼ����/ƽ��key����/val����/����ϵ��
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
		//�������,�ͷ�ȫ����Դ
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

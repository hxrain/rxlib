#ifndef _RX_MEM_ALLOC_H_
#define _RX_MEM_ALLOC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"
#include "rx_cc_atomic.h"
#include <memory.h>

//�Ƿ����ڴ�������ı߽���
#define RX_MEM_ALLOC_USE_CHECKING 1

namespace rx
{
	//------------------------------------------------------
	//���ڼ�¼�ڴ�������Ĺ������
	typedef struct alloc_stat_t
	{
		intptr_t alloc_size;                                //�ڴ��ѷ���(��������벹�봦���,���շ���ĳߴ�)
		intptr_t real_size;                                 //ʵ�ʸ�����(ʵ����Ҫ�ĳߴ�)
		intptr_t using_size;                                //������ʹ��(������ǰ�����������ȥ�����ڱ�ʹ�õ��ڴ�,free�滮�����)
		alloc_stat_t() :alloc_size(0), real_size(0), using_size(0) {}
	} alloc_stat_t;

	//======================================================
	//�ڴ�������ӿ���
	class mem_allotter_i
	{
		//---------------------------------------------
		//���������ڴ��ṹ:|info_t|userdata|pad_data|
		#pragma pack(push,1)
		//��׼�ڴ�������Ĵ��
		typedef struct cookie_t
		{
			//�ڴ������
			typedef struct info_t
			{
				uint32_t item_count;                        //��¼���ڴ���Ƿ�Ϊ����,����Ԫ�ص�����
				uint32_t mem_size;                          //���ڴ��ĳߴ�
			} info_t;
		private:
			//---------------------------------------------
			#if RX_MEM_ALLOC_USE_CHECKING
			typedef uint32_t pad_data_t;
			static const pad_data_t pad_data = 0x12345678;  //�������,�����ڴ�Խ����ʵļ��
			enum { cookie_size = sizeof(info_t) + sizeof(pad_data_t) };//ȫ������ĳߴ�(�����Ϣ+���β)
			//�ӳ���Ϊmem_size���ڴ��R��β����ȡ�������
			#define RX_MEM_BLOCK_TAIL_PAD_GET(R,mem_size) *rx_ptr_tail_cast(R, mem_size, sizeof(pad_data), pad_data_t)
			//�Գ���Ϊmem_size���ڴ��R��β�������������
			#define RX_MEM_BLOCK_TAIL_PAD_SET(R,mem_size) RX_MEM_BLOCK_TAIL_PAD_GET(R, mem_size) = pad_data
			//У���鳤��Ϊmem_size���ڴ��R��β���������
			#define RX_MEM_BLOCK_CHECK_PAD(R,mem_size) rx_fail_msg(RX_MEM_BLOCK_TAIL_PAD_GET(R,mem_size)==pad_data,"!!allotter memory block overflow!!")
			#else
			enum { cookie_size = sizeof(info_t) };          //ȫ������ĳߴ�(�����Ϣ)
			#define RX_MEM_BLOCK_TAIL_PAD_GET(R,mem_size)
			#define RX_MEM_BLOCK_TAIL_PAD_SET(R,mem_size)
			#define RX_MEM_BLOCK_CHECK_PAD(R,mem_size)
			#endif
		public:
			//---------------------------------------------
			//��֪����������û�����ܿռ�ߴ�
			static uint32_t memsize(uint32_t datsize) { return datsize + cookie_size; }
			//������Ϣͷ����ʵ�ʿ��õ��û���ߴ�
			static uint32_t usrsize(info_t &info) { return info.mem_size - cookie_size; }
			//---------------------------------------------
			//��������Ϣ����R��ָ���cookie_t��:Count=0����alloc;1����new����;����Ϊnew����;mem_size�����ڴ���ʵ�ʴ�С(>=4)
			//����ֵ:�û����õ�ָ�벿��
			static void* set(void* R, uint32_t Count, uint32_t mem_size)
			{
				info_t &ck = *rx_ptr_head_cast(R, 0, info_t);//ָ��info����
				ck.item_count = Count;                      //����info�����Ϣ�ļ�¼
				ck.mem_size = mem_size;
				RX_MEM_BLOCK_TAIL_PAD_SET(R, mem_size);     //���ڴ���β����¼�����
				return rx_ptr_head_cast(R, sizeof(info_t), void);
			}
			//---------------------------------------------
			//��R�����е�cookie_t�еõ�������Ϣ
			static info_t& get(void* R)
			{
				info_t &ck = *rx_ptr_head_cast(R, 0, info_t);//ָ��info����
				RX_MEM_BLOCK_CHECK_PAD(R, ck.mem_size);
				return ck;
			}
			//---------------------------------------------
			//����R�õ������û����õ�ָ�벿��
			static void* usrptr(void *R)
			{
				info_t &ck = *rx_ptr_head_cast(R, 0, info_t);
				RX_MEM_BLOCK_CHECK_PAD(R, ck.mem_size);
				return rx_ptr_head_cast(R, sizeof(info_t), void);
			}
			//---------------------------------------------
			//�����û�ָ���ȡԭʼָ��
			static void* rawptr(void *P)
			{
				void *R = rx_ptr_head_cast(P, -(int)sizeof(info_t), void);
				info_t &ck = *rx_ptr_head_cast(R, 0, info_t);
				RX_MEM_BLOCK_CHECK_PAD(R, ck.mem_size);
				return R;
			}
		}cookie_t;
		#pragma pack(pop)

		//-------------------------------------------------
		//����������ķ��乹��������,�ɸ���.
		#define __DESC_NEW_BASE__                                   \
            uint32_t MemSize=0;                                     \
            uint32_t Size=sizeof(VT)*Count;                         \
            uint8_t *R=(uint8_t*)base_alloc(MemSize,cookie_t::memsize(Size));\
            if (R==NULL) return NULL;                               \
            rx_atomic_add(m_stat.real_size, (intptr_t)Size);        \
            rx_atomic_add(m_stat.alloc_size, (intptr_t)MemSize);    \
            rx_atomic_add(m_stat.using_size, (intptr_t)MemSize);    \
            VT* Ret=(VT*)cookie_t::set(R,Count,MemSize);            \

		mem_allotter_i& operator=(const mem_allotter_i&);
		alloc_stat_t    m_stat;
	public:
		const alloc_stat_t& stat() const { return m_stat; }
		//-------------------------------------------------
		//����ԭʼ����ӿ�,�õ��ڴ��
		void* alloc(uint32_t Size)
		{
			uint32_t MemSize = 0;
			uint8_t *R = (uint8_t*)base_alloc(MemSize, cookie_t::memsize(Size));
			if (R == NULL)
				return NULL;

			rx_atomic_add(m_stat.real_size, (intptr_t)Size);
			rx_atomic_add(m_stat.alloc_size, (intptr_t)MemSize);
			rx_atomic_add(m_stat.using_size, (intptr_t)MemSize);

			return cookie_t::set(R, 0, MemSize);
		}
		//-------------------------------------------------
		//�ͷ��ڴ�
		void free(void* P)
		{
			rx_assert_msg(P != NULL, "memory pointer is empty!");

			uint8_t* R = (uint8_t*)cookie_t::rawptr(P);
			cookie_t::info_t &ck = cookie_t::get(R);
			rx_assert_msg(!ck.item_count, "the alloc/free function was incorrectly matched!");

			uint32_t memsize = ck.mem_size;
			base_free(R, memsize);
			rx_atomic_sub(m_stat.using_size, (intptr_t)memsize);
		}
		//-------------------------------------------------
		//���Ի��������ڴ������չ�ڴ�(��ʱ��ʹ�����ݿ����ķ�ʽʵ��,�ȴ������Ż�)
		virtual void* realloc(void* P, uint32_t NewSize, uint32_t reserved = 64)
		{
			uint32_t Size = NewSize + reserved;
			if (P == NULL)                                    //û��ԭָ��,ֱ�ӷ���(������Ԥ��)
				return alloc(Size);

			if (NewSize == 0)                                 //��ԭָ��,��Ҫ���³ߴ�Ϊ0ʱ,ֱ���ͷ�
			{
				free(P);
				return NULL;
			}

			//���ԭָ����ڴ�ڵ�ߴ�,������ʱ��ֱ�ӷ���
			uint8_t* R = (uint8_t*)cookie_t::rawptr(P);
			cookie_t::info_t &ck = cookie_t::get(R);
			if (cookie_t::usrsize(ck) >= Size)
				return P;

			//���ڷ���,ԭָ��ȷʵ��������,��Ҫ�����¿ռ�
			uint32_t MemSize = 0;
			R = (uint8_t*)base_alloc(MemSize, cookie_t::memsize(Size));
			if (R == NULL)
				return NULL;
			void *Ret = cookie_t::set(R, 0, MemSize);           //�õ����տɷ��ص��û�ָ��

			rx_atomic_add(m_stat.real_size, (intptr_t)Size);
			rx_atomic_add(m_stat.alloc_size, (intptr_t)MemSize);
			rx_atomic_add(m_stat.using_size, (intptr_t)MemSize);

			//����ԭ���ݵ��¿ռ�
			memcpy(Ret, P, cookie_t::usrsize(ck));

			//���,�ͷ�ԭָ��
			free(P);
			return Ret;
		}
		//-------------------------------------------------
		//��ָ֪���ڴ����û����óߴ�
		uint32_t usrsize(void* P) { return cookie_t::usrsize(cookie_t::get(cookie_t::rawptr(P))); }
		//��ָ֪���ڴ���ʵ��ռ�óߴ�
		uint32_t memsize(void* P) { return cookie_t::get(cookie_t::rawptr(P)).mem_size; }
		//-------------------------------------------------
		//����һ��ָ�����͵Ķ��������,ʹ��Ĭ�Ϲ��캯�����г�ʼ��,û�ж������
		template<class VT>
		VT* make(uint32_t Count = 1)
		{
			__DESC_NEW_BASE__
				return Count == 1 ? ct::OC(Ret) : ct::AC(Ret, Count);
		}
		//-------------------------------------------------
		//����һ��ָ�����͵Ķ��������,��ʹ�ò������й����ʼ��
		template<class VT, class PT1>
		VT* make(PT1& P1, uint32_t Count = 1)
		{
			__DESC_NEW_BASE__
				return Count == 1 ? ct::OC(Ret, P1) : ct::AC(Ret, Count, P1);
		}
		//-------------------------------------------------
		//����һ��ָ�����͵Ķ��������,��ʹ�ò������й����ʼ��
		template<class VT, class PT1, class PT2>
		VT* make(PT1& P1, PT2 &P2, uint32_t Count = 1)
		{
			__DESC_NEW_BASE__
				return Count == 1 ? ct::OC(Ret, P1, P2) : ct::AC(Ret, Count, P1, P2);
		}
		//-------------------------------------------------
		//����һ��ָ�����͵Ķ��������,��ʹ�ò������й����ʼ��
		template<class VT, class PT1, class PT2, class PT3>
		VT* make(PT1& P1, PT2 &P2, PT3 &P3, uint32_t Count = 1)
		{
			__DESC_NEW_BASE__
				return Count == 1 ? ct::OC(Ret, P1, P2, P3) : ct::AC(Ret, Count, P1, P2, P3);
		}
		//-------------------------------------------------
		//��newϵ�к�������Ķ������������������ջ��ڴ�
		template<class VT> bool unmake(VT* P)
		{
			rx_assert_msg(P != NULL, "memory pointer is empty!");

			uint8_t* R = (uint8_t*)cookie_t::rawptr(P);
			cookie_t::info_t &ck = cookie_t::get(R);
			if (ck.item_count == 1)
				ct::OD(P);									//�����Ķ���
			else
				ct::AD(P, ck.item_count);					//����������
			uint32_t memsize = ck.mem_size;
			base_free(R, memsize);
			rx_atomic_sub(m_stat.using_size, (intptr_t)memsize);
			return true;
		}
		//-------------------------------------------------
		template<class VT> bool unmake(void* P) { try { throw 1; } catch (...) {} rx_show_msg("This function is designed for type matching! Should not be called!"); }
		//-------------------------------------------------
		virtual ~mem_allotter_i() { rx_assert(m_stat.using_size == 0); }
	protected:
		#undef __DESC_NEW_BASE__
		//-------------------------------------------------
		//�����������ԭʼ�ķ���ӿ�,���б�Ҫ.���������н���������
		virtual void* base_alloc(uint32_t &bksize, uint32_t size) = 0;
		virtual void base_free(void* ptr, uint32_t size) = 0;
	};

}


#endif

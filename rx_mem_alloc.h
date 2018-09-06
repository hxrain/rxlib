#ifndef _RX_MEM_ALLOC_H_
#define _RX_MEM_ALLOC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"


namespace rx
{
    //======================================================
    //�ڴ�������ӿ���
    class mem_allotter_i
    {
        //��׼�ڴ�������Ĵ��
        typedef struct alloc_cookie_t
        {
            //�ڴ������
            typedef struct cookie_t
            {
                uint32_t item_count;                        //��¼���ڴ���Ƿ�Ϊ����,����Ԫ�ص�����
                uint32_t node_mem_size;                     //���ڴ��ĳߴ�
            } cookie_t;
            //����ṹ�Ĵ�С
            enum {cookie_size=sizeof(cookie_t)};

            //---------------------------------------------
            //��������Ϣ����cookie_t:Count=0����alloc;1����new����;����Ϊnew����;node_mem_size�����ڴ���ʵ�ʴ�С(>=4)
            static void set(void* P,uint32_t Count,uint32_t node_mem_size)
            {
                ((cookie_t*)P)->item_count=Count;
                ((cookie_t*)P)->node_mem_size=node_mem_size;
            }
            //---------------------------------------------
            //��cookie_t�еõ�������Ϣ
            static cookie_t& get(void* P)
            {
                return *(cookie_t*)P;
            }
        } alloc_cookie_t;

        //-------------------------------------------------
        //����������ķ��乹��������,�ɸ���.
        #define __DESC_NEW_BASE__   \
            uint32_t MemSize=0;     \
            uint8_t *R=(uint8_t*)base_alloc(MemSize,sizeof(VT)*Count+alloc_cookie_t::cookie_size);\
            if (R==NULL) return NULL;                                       \
            alloc_cookie_t::set(R,Count,MemSize);                           \
            VT* Ret=(VT*)(R+alloc_cookie_t::cookie_size);                   \

        mem_allotter_i& operator=(const mem_allotter_i&);
    public:
        //-------------------------------------------------
        //����ԭʼ����ӿ�,�õ��ڴ��
        void* alloc(uint32_t Size)
        {
            uint32_t MemSize=0;
            uint8_t *R=(uint8_t*)base_alloc(MemSize,Size+alloc_cookie_t::cookie_size);
            if (R==NULL)
                return NULL;
            alloc_cookie_t::set(R,0,MemSize);
            return (R+alloc_cookie_t::cookie_size);
        }
        //-------------------------------------------------
        //�ͷ��ڴ�
        void free(void* P)
        {
            rx_assert_msg(P!=NULL,"���ͷ��ڴ����!ָ��Ϊ��");
            uint8_t* R=(uint8_t*)P-alloc_cookie_t::cookie_size;
            alloc_cookie_t::cookie_t &ck=alloc_cookie_t::get(R);
            rx_assert_msg(!ck.item_count,"�����ʹ����alloc/free����ƥ��.");
            base_free(R,ck.node_mem_size);
        }
        //-------------------------------------------------
        //���Ի��������ڴ������չ�ڴ�(��ʱ��ʹ�����ݿ����ķ�ʽʵ��,�ȴ������Ż�)
        virtual void* realloc(void* P,uint32_t NewSize,uint32_t reserved=64)
        {
            if (P==NULL)                                    //û��ԭָ��,ֱ�ӷ���(������Ԥ��)
                return alloc(NewSize+reserved);

            if (NewSize==0)                                 //��ԭָ��,��Ҫ���³ߴ�Ϊ0ʱ,ֱ���ͷ�
            {
                free(P);
                return NULL;
            }

            //���ԭָ����ڴ�ڵ�ߴ�,������ʱ��ֱ�ӷ���
            uint8_t* R=(uint8_t*)P-alloc_cookie_t::cookie_size;
            alloc_cookie_t::cookie_t &ck=alloc_cookie_t::get(R);
            if (ck.node_mem_size-alloc_cookie_t::cookie_size>=NewSize)
                return P;

            //���ڷ���,ԭָ��ȷʵ��������,��Ҫ�����¿ռ�
            uint32_t MemSize=0;
            R=(uint8_t*)base_alloc(MemSize, NewSize+reserved);
            if (R==NULL)
                return NULL;
            alloc_cookie_t::set(R,0,MemSize);

            //����ԭ����
            void *Ret=(R+alloc_cookie_t::cookie_size);
            memcpy(Ret,P,ck.node_mem_size-alloc_cookie_t::cookie_size);

            //���,�ͷ�ԭָ��
            free(P);
            return Ret;
        }

        //-------------------------------------------------
        //����һ��ָ�����͵Ķ��������,ʹ��Ĭ�Ϲ��캯�����г�ʼ��
        template<class VT>
        VT* new0(uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret) : ct::AC(Ret,Count);
        }
        //-------------------------------------------------
        //����һ��ָ�����͵Ķ��������,��ʹ�ò������й����ʼ��
        template<class VT,class PT1>
        VT* new1(PT1& P1,uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret,P1) : ct::AC(Ret,Count,P1);
        }
        //-------------------------------------------------
        //����һ��ָ�����͵Ķ��������,��ʹ�ò������й����ʼ��
        template<class VT,class PT1,class PT2>
        VT* new2(PT1& P1,PT2 &P2,uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret,P1,P2) : ct::AC(Ret,Count,P1,P2);
        }
        //-------------------------------------------------
        //����һ��ָ�����͵Ķ��������,��ʹ�ò������й����ʼ��
        template<class VT,class PT1,class PT2,class PT3>
        VT* new3(PT1& P1,PT2 &P2,PT3 &P3,uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret,P1,P2,P3) : ct::AC(Ret,Count,P1,P2,P3);
        }
        //-------------------------------------------------
        //��newϵ�к�������Ķ������������������ջ��ڴ�
        template<class VT> bool del(VT* P)
        {
            rx_assert_msg(P!=NULL,"���ͷ��ڴ����!ָ��Ϊ��");

            uint8_t* R=(uint8_t*)P-alloc_cookie_t::cookie_size;
            alloc_cookie_t::cookie_t &ck=alloc_cookie_t::get(R);
            if (ck.item_count==1)
                ct::OD(P);                              //�����Ķ���
            else
                ct::AD(P,ck.item_count);                //����������
            base_free(R,ck.node_mem_size);
            return true;
        }
        //-------------------------------------------------
        template<class VT> bool _del(void* P) {try {throw 1;} catch(...) {} rx_show_msg("�˺���רΪ�ͱ�ƥ��ʹ��!��Ӧ�ñ�����!����������");}
        //-------------------------------------------------
        virtual ~mem_allotter_i() {}
    protected:
#undef __DESC_NEW_BASE__
        //-------------------------------------------------
        //�����������ԭʼ�ķ���ӿ�
        virtual void* base_alloc(uint32_t &bksize, uint32_t size)=0;
        virtual void base_free(void* ptr,uint32_t size)=0;
    };

}


#endif

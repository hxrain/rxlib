#ifndef _RX_MEM_ALLOC_H_
#define _RX_MEM_ALLOC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"
#include <memory.h>

//�Ƿ����ڴ�������ı߽���
#define RX_MEM_ALLOC_USE_CHECKING 1

namespace rx
{
    //======================================================
    //�ڴ�������ӿ���
    class mem_allotter_i
    {
        //---------------------------------------------
        //���������ڴ��ṹ:|info_t|pad_data|userdata|pad_data|
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

            //---------------------------------------------
        #if RX_MEM_ALLOC_USE_CHECKING
            static const uint32_t pad_data=0x12345678;      //�������,�����ڴ�Խ����ʵļ��
            enum {ph_size=sizeof(pad_data)};
            enum {pt_size=sizeof(pad_data)};
        #else
            enum {ph_size=0};
            enum {pt_size=0};
        #endif
            enum {cookie_size=sizeof(info_t)+ph_size+pt_size};//ȫ������ĳߴ�(��Ϣ��/���ͷ/���β)

            //---------------------------------------------
            //��֪����������û�����ܿռ�ߴ�
            static uint32_t memsize(uint32_t datsize){return datsize+cookie_size;}
            //������Ϣͷ����ʵ�ʿ��õ��û���ߴ�
            static uint32_t usrsize(info_t &info){return info.mem_size-cookie_size;}
            //---------------------------------------------
            //��������Ϣ����R��ָ���cookie_t��:Count=0����alloc;1����new����;����Ϊnew����;mem_size�����ڴ���ʵ�ʴ�С(>=4)
            //����ֵ:�û����õ�ָ�벿��
            static void* set(void* R,uint32_t Count,uint32_t mem_size)
            {
                info_t &ck=*(info_t*)R;
                ck.item_count=Count;
                ck.mem_size=mem_size;
        #if RX_MEM_ALLOC_USE_CHECKING
                *(uint32_t*)(((uint8_t*)R)+sizeof(info_t))=pad_data;          //���ͷ��
                *(uint32_t*)(((uint8_t*)R)+(mem_size-pt_size))=pad_data;      //���β��
        #endif
                return ((uint8_t*)R)+sizeof(info_t)+ph_size;
            }
            //---------------------------------------------
            //���R�����ص�cookie��Ϣ
            static void clr(void* R)
            {
                info_t &ck=*(info_t*)R;
#if RX_MEM_ALLOC_USE_CHECKING
                uint32_t &ph=*(uint32_t*)(((uint8_t*)R)+sizeof(info_t));      //���ͷ��
                rx_assert(ph==pad_data);//�����ظ��ͷŻ�����Ѿ��ͷź���ڴ��ָ��
                uint32_t &pt=*(uint32_t*)(((uint8_t*)R)+(ck.mem_size-pt_size));//���β��
                rx_assert_msg(pt==pad_data,"memory overflow!!");    //����ڴ��Ƿ�Խ�縲��
                ph=0;
                pt=0;
#endif
                ck.item_count=0;
                ck.mem_size=0;
            }
            //---------------------------------------------
            //��R�����е�cookie_t�еõ�������Ϣ
            static info_t& get(void* R)
            {
                info_t &ck=*(info_t*)R;
        #if RX_MEM_ALLOC_USE_CHECKING
                uint32_t ph=*(uint32_t*)(((uint8_t*)R)+sizeof(info_t));         //���ͷ��
                rx_fail_msg(ph==pad_data,"illegal memory access!!");            //�����ظ��ͷŻ�����Ѿ��ͷź���ڴ��ָ��
                uint32_t pt=*(uint32_t*)(((uint8_t*)R)+(ck.mem_size-pt_size));  //���β��
                rx_fail_msg(pt==pad_data,"memory overflow!!");                  //����ڴ��Ƿ�Խ�縲��
        #endif
                return ck;
            }
            //---------------------------------------------
            //����R�õ������û����õ�ָ�벿��
            static void* usrptr(void *R)
            {
        #if RX_MEM_ALLOC_USE_CHECKING
                info_t &ck=*(info_t*)R;
                uint32_t ph=*(uint32_t*)(((uint8_t*)R)+sizeof(info_t));         //���ͷ��
                rx_fail_msg(ph==pad_data,"illegal memory access!!");            //�����ظ��ͷŻ�����Ѿ��ͷź���ڴ��ָ��
                uint32_t pt=*(uint32_t*)(((uint8_t*)R)+(ck.mem_size-pt_size));  //���β��
                rx_fail_msg(pt==pad_data,"memory overflow!!");                  //����ڴ��Ƿ�Խ�縲��
        #endif
                return ((uint8_t*)R)+sizeof(info_t)+ph_size;
            }
            //---------------------------------------------
            //�����û�ָ���ȡԭʼָ��
            static void* rawptr(void *P)
            {
                void *R=(uint8_t*)P-(sizeof(info_t)+ph_size);
        #if RX_MEM_ALLOC_USE_CHECKING
                info_t &ck=*(info_t*)R;
                uint32_t ph=*(uint32_t*)(((uint8_t*)R)+sizeof(info_t));         //���ͷ��
                rx_fail_msg(ph==pad_data,"illegal memory access!!");            //�����ظ��ͷŻ�����Ѿ��ͷź���ڴ��ָ��
                uint32_t pt=*(uint32_t*)(((uint8_t*)R)+(ck.mem_size-pt_size));  //���β��
                rx_fail_msg(pt==pad_data,"memory overflow!!");                  //����ڴ��Ƿ�Խ�縲��
        #endif
                return R;
            }
        }cookie_t;
        #pragma pack(pop)

        //-------------------------------------------------
        //����������ķ��乹��������,�ɸ���.
        #define __DESC_NEW_BASE__                                   \
            uint32_t MemSize=0;                                     \
            uint8_t *R=(uint8_t*)base_alloc(MemSize,cookie_t::memsize(sizeof(VT)*Count));\
            if (R==NULL) return NULL;                               \
            VT* Ret=(VT*)cookie_t::set(R,Count,MemSize);            \

        mem_allotter_i& operator=(const mem_allotter_i&);
    public:
        //-------------------------------------------------
        //����ԭʼ����ӿ�,�õ��ڴ��
        void* alloc(uint32_t Size)
        {
            uint32_t MemSize=0;
            uint8_t *R=(uint8_t*)base_alloc(MemSize,cookie_t::memsize(Size));
            if (R==NULL)
                return NULL;
            return cookie_t::set(R,0,MemSize);
        }
        //-------------------------------------------------
        //�ͷ��ڴ�
        void free(void* P)
        {
            rx_assert_msg(P!=NULL,"���ͷ��ڴ����!ָ��Ϊ��");
            uint8_t* R=(uint8_t*)cookie_t::rawptr(P);
            cookie_t::info_t &ck=cookie_t::get(R);
            rx_assert_msg(!ck.item_count,"�����ʹ����alloc/free����ƥ��.");
            uint32_t memsize=ck.mem_size;
            cookie_t::clr(R);
            base_free(R,memsize);
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
            uint8_t* R=(uint8_t*)cookie_t::rawptr(P);
            cookie_t::info_t &ck=cookie_t::get(R);
            if (cookie_t::usrsize(ck)>=NewSize+reserved)
                return P;

            //���ڷ���,ԭָ��ȷʵ��������,��Ҫ�����¿ռ�
            uint32_t MemSize=0;
            R=(uint8_t*)base_alloc(MemSize, cookie_t::memsize(NewSize+reserved));
            if (R==NULL)
                return NULL;
            void *Ret=cookie_t::set(R,0,MemSize);           //�õ����տɷ��ص��û�ָ��

            //����ԭ���ݵ��¿ռ�
            memcpy(Ret,P,cookie_t::usrsize(ck));

            //���,�ͷ�ԭָ��
            free(P);
            return Ret;
        }
        //-------------------------------------------------
        //��ָ֪���ڴ����û����óߴ�
        uint32_t usrsize(void* P){return cookie_t::usrsize(cookie_t::get(cookie_t::rawptr(P)));}
        //��ָ֪���ڴ���ʵ��ռ�óߴ�
        uint32_t memsize(void* P){return cookie_t::get(cookie_t::rawptr(P)).mem_size;}
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

            uint8_t* R=(uint8_t*)cookie_t::rawptr(P);
            cookie_t::info_t &ck=cookie_t::get(R);
            if (ck.item_count==1)
                ct::OD(P);                              //�����Ķ���
            else
                ct::AD(P,ck.item_count);                //����������
            uint32_t memsize=ck.mem_size;
            cookie_t::clr(R);
            base_free(R,memsize);
            return true;
        }
        //-------------------------------------------------
        template<class VT> bool del(void* P) {try {throw 1;} catch(...) {} rx_show_msg("�˺���רΪ�ͱ�ƥ��ʹ��!��Ӧ�ñ�����!����������");}
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

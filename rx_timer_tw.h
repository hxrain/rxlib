#ifndef _RX_TIMER_TW_H_
#define _RX_TIMER_TW_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list.h"
#include "rx_dtl_queue.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_ct_util.h"
#include "rx_ct_delegate.h"

namespace rx
{
    //-------------------------------------------------
    //��׼��ʱ���ص���������
    typedef void (*on_timer_t)(const size_t handle, uint32_t event_code, void *usrdat);

    //��Ա�����ص�����
    //void xx::on_timer(const size_t handle, uint32_t event_code)

    //��ʱ���¼��ص�ί������
    typedef delegate2_rt<const size_t, uint32_t, void> timer_delegate_t;

    namespace tw
    {
        //-------------------------------------------------
        //�����ڲ���ʱ����Ϣ�б�����
        struct timer_entry_t;
        typedef list_t<timer_entry_t*>  item_list_t;

        const uint32_t wheel_slots = 256;                   //ÿ�������ϵĲ�λ����
        const uint32_t max_rounds  = 65535;                 //���������������

        //-------------------------------------------------
        //�ڲ���ʱ����Ϣ��
        typedef struct timer_entry_t
        {
            static const uint8_t size_mask = 0x1F;          //����Ŀ�ĳߴ�����(2��5�η�,Ӧ<=sizeof(*this))

            item_list_t::iterator   slot_link;              //��¼���ڵ���ʱ��������е�λ��,���ڷ������
            timer_delegate_t        cb;                     //��ʱ��ί�лص�
            uint32_t                interval;               //��ʱ���ʱ��,ms
            uint32_t                event_code;             //�û��������¼���
            uint32_t                repeat;                 //�ظ�ִ�д���,-1Ϊ��Զִ��
            uint16_t                round;                  //��������ʱ����Ҫ��ʣ������,Ϊ0ʱ���ܴ���
            uint16_t                ttl;                    //����Ŀ���������

            //�ڲ���Ϣ���,׼���ٴθ���
            void reset()
            {
                slot_link=NULL;
                cb.reset();
                interval=0;
                event_code=0;
                repeat=0;
                round=0;
                ttl=(ttl+1)&size_mask;                      //�������������,����ȡģ
            }
            //��ȡ����Ŀ�Ķ�����
            size_t handle()
            {
                size_t ret=(size_t)this;
                rx_assert((ret&size_mask)==0);
                return ret|ttl;                             //����ָ���5bit���ttl,�ȴ�����ʱ����֤
            }
            //У�����������Ч��
            bool check(size_t handle)
            {
                return (handle&size_mask)==ttl&&((handle&(~size_mask))==(size_t)this);
            }
            //��̬����,���ݶ�ʱ����������ڲ���Ϣ����ָ��
            static timer_entry_t* ptr(size_t handle)
            {
                timer_entry_t *ret=(timer_entry_t*)(handle&(~size_mask));
                if (ret->check(handle))
                    return ret;
                return NULL;
            }
            timer_entry_t():ttl(0){reset();}
        }timer_entry_t;

        //-------------------------------------------------
        //���������ʱ���ֹ��ܷ�װ
        class timing_wheel_t
        {
            item_list_t     slots[wheel_slots];             //ʱ�����еĲ�λ����.ÿ����λ��Ӧ��һ����ʱ����Ϣָ���б�
            uint32_t        slot_ptr;                       //ʱ���ֵĲ�ָ��,ָ��ǰ���������ʱ���
        public:
        };
        //-------------------------------------------------
    }






}

#endif

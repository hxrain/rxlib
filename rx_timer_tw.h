#ifndef _RX_TIMER_TW_H_
#define _RX_TIMER_TW_H_
#include "list"
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

        //����ָ��ת���¼�ί��
        typedef delegate1_rt<timer_entry_t&, void> step_delegate_t;

        //����ת��һ���¼�ί��
        typedef delegate1_rt<uint32_t, void> round_delegate_t;

        const uint32_t max_wheels  = 4;                     //������������
        const uint32_t wheel_slots = 256;                   //ÿ�������ϵĲ�λ����
        const uint32_t slot_mask   = 0xFF;                  //��λ��������,����ȡģ����
        const uint32_t max_rounds  = 65535;                 //���������������

        //-------------------------------------------------
        //�ڲ���ʱ����Ϣ��
        typedef struct timer_entry_t
        {
            static const uint8_t size_mask = 0x1F;          //����Ŀ�ĳߴ�����(2��5�η�,Ӧ<=sizeof(*this))
            //---------------------------------------------
            //�ⲿ�û��ṩ�Ĳ���
            timer_delegate_t      u_cb_func;                //��ʱ��ί�лص�
            uint32_t              u_interval;               //��ʱ���ʱ��,ms
            uint32_t              u_event_code;             //�û��������¼���
            uint16_t              u_repeat;                 //�ظ�ִ�д���,-1Ϊ��Զִ��

                                                            //---------------------------------------------
                                                            //�ڲ�������Ҫ�Ĳ���
            uint16_t              c_round;                  //��������ʱ����Ҫ��ʣ������,Ϊ0ʱ���ܴ���
            uint8_t               c_dummy;
            uint8_t               c_ttl;                    //����Ŀ���������
            uint8_t               w_wheel_idx;              //����Ŀ���������ӱ��
            uint8_t               w_slot_idx;               //����Ŀ�����Ĳ�λ����
            item_list_t::iterator w_slot_link;              //��¼���ڵ���ʱ��������е�λ��,���slot_idx���ڷ������

            //---------------------------------------------
            //�ڲ���Ϣ���,׼���ٴθ���
            void reset()
            {
                u_cb_func.reset();
                u_interval=0;
                u_event_code=0;
                u_repeat=0;

                c_round=0;
                c_ttl = (c_ttl + 1)&size_mask;              //�������������,����ȡģ

                w_wheel_idx = -1;
                w_slot_idx = 0;
                w_slot_link = NULL;
            }
            //---------------------------------------------
            //��ȡ����Ŀ�Ķ�����
            size_t handle()
            {
                size_t ret=(size_t)this;
                rx_assert((ret&size_mask)==0);
                return ret | c_ttl;                         //����ָ���5bit���ttl,�ȴ�����ʱ����֤
            }
            //---------------------------------------------
            //У�����������Ч��
            bool check(size_t handle)
            {
                return (handle&size_mask)== c_ttl;
            }
            //---------------------------------------------
            //��̬����,���ݶ�ʱ����������ڲ���Ϣ����ָ��
            static timer_entry_t* to_ptr(size_t handle)
            {
                timer_entry_t *ret=(timer_entry_t*)(handle&(~size_mask));
                if (ret->check(handle))
                    return ret;
                return NULL;
            }
            //---------------------------------------------
            timer_entry_t():c_ttl(0){reset();}
        }timer_entry_t;

        //-------------------------------------------------
        //������ʱ���ֹ��ܷ�װ
        class wheel_t
        {
            item_list_t   m_slots[wheel_slots];             //ʱ�����еĲ�λ����.ÿ����λ��Ӧ��һ����ʱ����Ϣָ���б�
            uint32_t      m_slot_ptr;                       //ʱ���ֵĲ�ָ��,ָ��ǰ���������ʱ���
            uint32_t      m_wheel_idx;                      //�������
        public:
            wheel_t():m_wheel_idx(-1){}
            step_delegate_t     cb_step;                    //ָ��ת���¼�,��Ҫ����һ����Ŀ
            round_delegate_t    cb_round;                   //����ת��һ���¼�,���������ϲ�����
            //---------------------------------------------
            //��ʼ��
            bool init(uint32_t wheel_idx, mem_allotter_i& mem)
            {
                if (m_wheel_idx != (uint32_t)-1)
                    return false;
                m_slot_ptr = 0;
                m_wheel_idx = wheel_idx;
                ct::AC(m_slots, wheel_slots, mem);          //!!ǿ��!!"���¹����ʼ��"��λ����,���ڴ������
                return true;
            }
            //---------------------------------------------
            //���
            void uninit()
            {
                for (uint32_t i = 0; i < wheel_slots; ++i)
                    m_slots[i].clear();
                m_wheel_idx = -1;
                m_slot_ptr = 0;
                cb_round.reset();
                cb_step.reset();
            }
            //---------------------------------------------
            //��ȡ��ǰ���ӵĲ�λָ��λ��
            uint32_t slot_ptr() { return m_slot_ptr; }
            //---------------------------------------------
            //��������item��װ�ڸ����Ĳ�λ��
            bool insert(timer_entry_t& item, uint8_t slot_idx)
            {
                rx_assert(slot_idx < wheel_slots);
                if (slot_idx >= wheel_slots)
                    return false;

                //��ָ���Ĳ�λ�ϲ�����Ŀ
                item_list_t &slot = m_slots[slot_idx];
                item.w_slot_link = slot.push_back(&item);
                if (item.w_slot_link == slot.end())
                {
                    item.w_slot_link = NULL;
                    return false;
                }

                //��¼��Ŀ���ֲ���Ϣ
                item.w_wheel_idx = m_wheel_idx;
                item.w_slot_idx = slot_idx;
                return true;
            }
            //---------------------------------------------
            //��������item�Ӳ�λ��ժ��
            bool remove(timer_entry_t& item)
            {
                if (item.w_slot_idx >= wheel_slots || item.w_wheel_idx!=m_wheel_idx ||
                    item.w_slot_link == NULL)
                    return false;

                //����Ŀ�Ӿɲ����Ƴ�
                item_list_t &slot = m_slots[item.w_slot_idx];
                item_list_t::iterator I = item.w_slot_link;
                if (!slot.earse(I))
                    return false;

                //Ĩ����Ŀ���ֲ���Ϣ
                item.w_wheel_idx = -1;
                item.w_slot_idx = -1;
                item.w_slot_link = NULL;

                return true;
            }
            //---------------------------------------------
            //��ָ������Ŀ�ƶ����µĲ�λ
            bool move(timer_entry_t& item,uint8_t slot_idx)
            {
                rx_assert(slot_idx < wheel_slots);
                if (slot_idx >= wheel_slots || item.w_slot_idx >= wheel_slots ||
                    item.w_wheel_idx != m_wheel_idx || item.w_slot_link == NULL)
                    return false;

                //����Ŀ�Ӿɲ��Ƴ�
                {
                item_list_t &slot = m_slots[item.w_slot_idx];
                item_list_t::iterator I = item.w_slot_link;
                if (!slot.earse(I))
                    return false;
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
            uint32_t step()
            {
                //��λָ��ǰ��һ��
                m_slot_ptr = (m_slot_ptr + 1) & slot_mask;
                if (m_slot_ptr == 0)
                    cb_round(m_wheel_idx);                  //��һ�ֽ�����,��һ�ֿ�ʼ�����¼�,�����������ϲ����ӵ�ת��

                item_list_t &slot = m_slots[m_slot_ptr];
                uint32_t rc=0;
                for (item_list_t::iterator I = slot.begin(); I != slot.end();)
                {
                    timer_entry_t &item = *(*I);
                    ++I;                                    //������Ԥ�Ⱥ���,������cb_step��item��ɾ��ʱ��������.

                    if (item.c_round)
                    {//����ʣ������,�����ȴ���һ�ΰ�
                        --item.c_round;
                        continue;
                    }

                    ++rc;
                    cb_step(item);                          //�����ⲿ�¼�����
                }
                return rc;
            }
        };
        //-------------------------------------------------
    }

    //-----------------------------------------------------
    //ʱ���ֶ�ʱ��
    //-----------------------------------------------------
    class timing_wheel_t
    {
        tw::wheel_t     m_wheels[tw::max_wheels];           //����༶(max_wheels)��������,0Ϊ��ͼ�;ÿ�����Ӽ�ʱ���������256(wheel_slots)��
        uint64_t        m_last_step_time;                   //��һ�εĶ�������ʱ��,us.
        uint32_t        m_slot_unit_us;                     //0������ʱ��۵�ʱ�䵥λ,��us��ʾ.
        //-------------------------------------------------
        //����һ����ʱ����Ŀ
        void on_step(tw::timer_entry_t& item)
        {

        }
        //-------------------------------------------------
        //����һ������ת��(��������ʱ�˷������ܻᱻ�ݹ����max_wheels��)
        void on_round(uint32_t wi)
        {
            if (wi<tw::max_wheels-1)
                m_wheels[wi+1].step();                      //���������ϲ�ʱ����
        }
    public:
        //-------------------------------------------------
        //Ĭ�Ϲ��첻��ʼ��(����ȷ������ʼʱ��Ž��й����ʼ��).
        timing_wheel_t(uint64_t curr_time_us=0,uint32_t slot_unit_us=1000,mem_allotter_i& ma=rx_global_mem_allotter())
        {
            if (curr_time_us)
                wheels_init(curr_time_us,slot_unit_us,ma);
        }
        ~timing_wheel_t(){wheels_uninit();}
        //-------------------------------------------------
        //ʱ���ֳ�ʼ��:������ʼʱ���;�ڲ�ʱ��۵�ʱ�䵥λ(Ĭ��1ms);ʱ����ʹ�õ��ڴ������.
        bool wheels_init(uint64_t curr_time_us,uint32_t slot_unit_us=1000,mem_allotter_i& mem=rx_global_mem_allotter())
        {
            for(uint32_t i=0;i<tw::max_wheels;++i)
            {
                tw::wheel_t  &w=m_wheels[i];
                if (!w.init(i,mem))
                    return false;
                w.cb_round.bind(*this,&timing_wheel_t::on_round);
                w.cb_step.bind(*this,&timing_wheel_t::on_step);

            }

            m_last_step_time = curr_time_us;
            m_slot_unit_us = slot_unit_us;

            return true;
        }
        //-------------------------------------------------
        //���
        void wheels_uninit()
        {
            for(uint32_t i=0;i<tw::max_wheels;++i)
            {
                tw::wheel_t  &w=m_wheels[i];
                w.uninit();
            }
        }
        //-------------------------------------------------
        //������ǰʱ����:������ǰʱ��.
        //����ֵ:�����������ߵĲ���
        uint32_t wheels_step(uint64_t curr_time_us)
        {
            //��������ʱ��
            uint64_t delta=curr_time_us-m_last_step_time;
            //����Ӧ���߶��Ĳ���
            uint32_t step_count=uint32_t(delta/m_slot_unit_us);

            //������ͼ�ʱ��������(��round�ص��л�ͬ�������ϼ�ʱ����)
            for(uint32_t i=0;i<step_count;++i)
                m_wheels[0].step();

            //����������ߵ�ʱ���
            m_last_step_time+=step_count*m_slot_unit_us;
            return step_count;
        }
        //-------------------------------------------------
        //����һ���µĶ�ʱ��
        size_t timer_insert()
        {

        }
        //-------------------------------------------------
        //ɾ��һ����ʱ��
        bool timer_remove(size_t timer)
        {

        }
        //-------------------------------------------------
        //����һ����ʱ��(�Ե�ǰʱ���ʹ������,������һ�εĴ���ʱ��)
        bool timer_update(size_t timer)
        {

        }
    };




}

#endif

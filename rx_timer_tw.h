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
    //��׼��ʱ���ص���������:��ʱ�����;�ⲿ�󶨵��¼���;��ʱ��ʣ���ظ�����,0ʱ����ɾ��;�ⲿ�󶨵��û�ָ��.
    typedef void (*on_timer_t)(size_t handle, uint32_t event_code, uint16_t repeat, void *usrdat);

    //��Ա�����ص�����:��ʱ�����;�ⲿ�󶨵��¼���;��ʱ��ʣ���ظ�����,0ʱ����ɾ��;
    //void xx::on_timer(size_t handle, uint32_t event_code, uint16_t repeat)

    //��ʱ���¼��ص�ί������
    typedef delegate3_rt<size_t, uint32_t, uint16_t, void> timer_delegate_t;

    namespace tw
    {
        //-------------------------------------------------
        //�����ڲ���ʱ����Ϣ�б�����
        struct timer_item_t;
        typedef list_t<timer_item_t*>  item_list_t;

        //����ָ�������¼�ί��
        typedef delegate1_rt<timer_item_t*, void> item_delegate_t;

        //����ת��һ���¼�ί��
        typedef delegate1_rt<uint32_t, uint32_t> round_delegate_t;

        const uint32_t wheel_slots = 256;                   //ÿ�������ϵĲ�λ����
        const uint32_t slot_mask   = wheel_slots-1;         //��λ��������,����ȡģ����
        const uint32_t max_rounds  = 65535;                 //��ʱ����Ŀ�����������Ȧ��

        //-------------------------------------------------
        //�ڲ���ʱ����Ϣ��
        typedef struct timer_item_t
        {
            static const uint8_t size_mask = 0x1F;          //����Ŀ�ĳߴ�����(2��5�η�,Ӧ<=sizeof(*this))

            //---------------------------------------------
            //�ⲿ�û��ṩ�Ĳ���
            size_t                u_inv_tick;               //��ʱ����δ���
            uint32_t              u_event_code;             //�û��������¼���
            timer_delegate_t      u_cb_func;                //��ʱ��ί�лص�
            uint16_t              u_repeat;                 //�ظ�ִ�д���,-1Ϊ��Զִ��

            //---------------------------------------------
            //�ڲ�������Ҫ�Ĳ���
            uint8_t               dummy;
            uint8_t               c_ttl;                    //����Ŀ���������

            //---------------------------------------------
            //��Ŀ��������ʱ���е�λ����Ϣ
            uint8_t               w_wheel_idx;              //����Ŀ���������ӱ��
            uint8_t               w_slot_idx;               //����Ŀ�����Ĳ�λ����
            uint16_t              w_round;                  //��������ʱ����Ҫ��ʣ������,Ϊ0ʱ���ܴ���
            item_list_t::iterator w_slot_link;              //��¼���ڵ���ʱ��������е�λ��,���slot_idx���ڷ������

            //---------------------------------------------
            //�ڲ���Ϣ���,׼���ٴθ���
            void reset()
            {
                u_cb_func.reset();
                u_inv_tick=0;
                u_event_code=0;
                u_repeat=0;

                w_round=0;
                c_ttl = (c_ttl + 1)&size_mask;              //�������������,����ȡģ

                w_wheel_idx = -1;
                w_slot_idx = 0;
                w_slot_link = NULL;
            }
            //---------------------------------------------
            //��ȡ����Ŀ�Ķ�����(��5bit������һ���̶��ϱ�����ʵ��Ѿ�����ʱɾ������Ŀ)
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
            static timer_item_t* to_ptr(size_t handle)
            {
                timer_item_t *ret=(timer_item_t*)(handle&(~size_mask));
                if (ret->check(handle))
                    return ret;
                return NULL;
            }
            //---------------------------------------------
            timer_item_t():c_ttl(0){reset();}
        }timer_item_t;

        //-------------------------------------------------
        //��չ�򵥻��������
        class entry_cache_t :public object_cache_t<timer_item_t>
        {
        public:
            //-------------------------------------------------
            void recycle(timer_item_t *item)
            {
                rx_assert(item != NULL);
                item->reset();
                put(item);
            }
        };

        //-------------------------------------------------
        //������ʱ���ֹ��ܷ�װ
        class wheel_t
        {
            item_list_t   m_slots[wheel_slots];             //ʱ�����еĲ�λ����.ÿ����λ��Ӧ��һ����ʱ����Ϣָ���б�
            uint32_t      m_slot_ptr;                       //ʱ���ֵĲ�ָ��,ָ��ǰ���������ʱ���
            uint32_t      m_wheel_idx;                      //�������
        public:
            wheel_t():m_wheel_idx(-1){}
            item_delegate_t     cb_item;                    //ָ��ת���¼�,��Ҫ����һ����Ŀ
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
            void uninit(entry_cache_t& ec)
            {
                for (uint32_t i = 0; i < wheel_slots; ++i)
                {
                    item_list_t &slot = m_slots[i];
                    for (item_list_t::iterator I = slot.begin(); I != slot.end();)
                    {
                        timer_item_t *item = *I;
                        slot.earse(I);
                        ec.recycle(item);
                    }
                }

                m_wheel_idx = -1;
                m_slot_ptr = 0;
                cb_round.reset();
                cb_item.reset();
            }
            //---------------------------------------------
            //��ȡ��ǰ���ӵĲ�λָ��λ��
            uint32_t slot_ptr() { return m_slot_ptr; }
            //---------------------------------------------
            //��������item��װ�ڸ����Ĳ�λ��
            //����ֵ:�Ƿ����²�λ����ɹ�
            bool put(timer_item_t& item)
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
            bool pick(timer_item_t& item)
            {
                rx_assert_ret(item.w_wheel_idx==m_wheel_idx);

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
            bool move(timer_item_t& item,uint8_t slot_idx)
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
            uint32_t step()
            {
                uint32_t rc=0;
                //��λָ��ǰ��һ��
                m_slot_ptr = (m_slot_ptr + 1) & slot_mask;
                if (m_slot_ptr == 0)
                    rc+=cb_round(m_wheel_idx);              //��һ�ֽ�����,��һ�ֿ�ʼ�����¼�,�����������ϲ����ӵ�ת��

                item_list_t &slot = m_slots[m_slot_ptr];
                for (item_list_t::iterator I = slot.begin(); I != slot.end();)
                {
                    timer_item_t &item = *(*I);
                    ++I;                                    //������Ԥ�Ⱥ���,������cb_step��item��ɾ��ʱ��������.

                    if (item.w_round)
                    {//����ʣ������,�����ȴ���һ�ΰ�
                        --item.w_round;
                        continue;
                    }

                    ++rc;
                    cb_item(&item);                         //�����ⲿ�¼�����
                }
                return rc;
            }
        };
        //-------------------------------------------------
    }

    //-----------------------------------------------------
    //ʱ���ֶ�ʱ��
    //-----------------------------------------------------
    template<uint32_t max_wheels = 4>
    class timing_wheel_t
    {
        //static const uint32_t max_wheels = 4;
        static const uint32_t min_level_wheel = 0;          //��ײ���������
        static const uint32_t max_level_wheel =max_wheels-1;//�����������
        rx_static_assert(max_wheels<=4);

        tw::entry_cache_t   m_items_cache;                  //��ʱ����Ŀ����ָ�뻺��
        tw::wheel_t         m_wheels[max_wheels];           //����༶(max_wheels)��������,min_level_wheel(0)Ϊ��ͼ�;ÿ�����Ӽ�ʱ���������256��(wheel_slots)
        uint64_t            m_last_step_time;               //��һ�εĶ�������ʱ��,ms.
        uint32_t            m_tick_unit_ms;                 //min_level_wheel(0)������ʱ��۵ĵδ�ʱ�䵥λ,��ms��ʾ.
        //-------------------------------------------------
        //�������Ӷ�Ӧ�����ֵδ�������(��ֵ��һ,����ģ�뷽��ȡ��)
        static uint32_t wheel_ticks_mask(uint32_t i)
        {
            rx_assert(i<max_wheels);
            static const uint32_t tbl[max_wheels] = { 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF };
            return tbl[i];
        }
        //�������Ӷ�Ӧ�����ֵδ�����λ(������λ��ȡ��)
        static uint32_t wheel_ticks_shift(uint32_t i)
        {
            if (i>=max_wheels) return 0;
            static const uint32_t tbl[max_wheels] = { 8, 16, 24, 32 };
            return tbl[i];
        }

        //-------------------------------------------------
        //����һ������ת��(��������ʱ�˷������ܻᱻ�ݹ����max_wheels��)
        uint32_t on_round(uint32_t wi)
        {
            uint32_t rc=0;
            if (wi<max_level_wheel)
                rc=m_wheels[wi+1].step();                      //���������ϲ�ʱ����
            return rc;
        }
        //-------------------------------------------------
        //����һ����ʱ����Ŀ
        void on_item(tw::timer_item_t* item)
        {
            rx_assert(item != NULL);
            rx_assert(item->w_round == 0);
            size_t remain_tick = item->u_inv_tick & wheel_ticks_mask(item->w_wheel_idx);

            if (item->w_wheel_idx == min_level_wheel ||
                remain_tick == 0)
            {//��Ͳ������ϵĶ�ʱ����������;�����ϲ����ӵ��ٽ�㶨ʱ��ֱ�Ӵ���.
                if (item->u_repeat != (uint16_t)-1)
                    --item->u_repeat;                       //�����ظ�����

                //�����û��󶨵Ļص�����
                item->u_cb_func(item->handle(), item->u_event_code, item->u_repeat);

                if (item->u_repeat == 0)
                    m_timer_remove(item);                   //����ظ�����Ϊ0��ɾ����ǰ��ʱ����Ŀ
                else
                    m_timer_update(item,item->u_inv_tick);  //��Ҫ���µ�����ǰ��ʱ����Ŀ����λ����
            }
            else
            {//�ϲ������ϵĶ�ʱ����Ҫ���н�������
                m_timer_update(item,remain_tick);
            }
        }

        //-------------------------------------------------
        //���ݸ����Ķ�ʱ���,��̬�������¶�ʱ��Ӧ�ò����λ��:���ڵδ���,��������,��λ����
        //����ֵ:��Ŀ��round��
        uint16_t m_calc_pos(size_t inv_tick, uint8_t &wheel_idx, uint8_t &slot_idx)
        {
            uint32_t round = uint32_t(inv_tick >> wheel_ticks_shift(max_level_wheel));
            if (round)
            {//��������߲������Ͻ��еδ��Ȧ����
                uint8_t offset = uint8_t((inv_tick & wheel_ticks_mask(max_level_wheel)) >> wheel_ticks_shift(max_level_wheel-1));
                wheel_idx = max_level_wheel;
                slot_idx = m_wheels[wheel_idx].slot_ptr() + offset;
                return uint16_t(round > tw::max_rounds ? tw::max_rounds : round);
            }
            else
            {//��߲������Ȧ,�����������������ȷ��λ��
                for (int i = max_level_wheel; i >= 1; --i)
                {
                    if (inv_tick >> wheel_ticks_shift(i - 1))
                    {//���ڵδ����ڵ�ǰ�����һ�������������,����Ҫ�ҽӵ���ǰ��
                        uint8_t offset = uint8_t((inv_tick & wheel_ticks_mask(i)) >> wheel_ticks_mask(i-1));
                        wheel_idx = i;
                        slot_idx = m_wheels[wheel_idx].slot_ptr() + offset;
                        return 0;
                    }
                }
                //����ײ������Ϸ���
                wheel_idx = 0;
                slot_idx = uint8_t(m_wheels[wheel_idx].slot_ptr() + inv_tick);
                return 0;
            }
        }

        //-------------------------------------------------
        //������ʱ����Ŀ
        tw::timer_item_t* m_timer_create(size_t inv_tick, uint32_t event_code, uint16_t repeat)
        {
            tw::timer_item_t *item = m_items_cache.get();
            if (!item) return NULL;

            //������λ��
            item->w_round = m_calc_pos(inv_tick, item->w_wheel_idx, item->w_slot_idx);
            if (!m_wheels[item->w_wheel_idx].put(*item))
            {//��λ�ò���ʧ��,�黹��Դ
                m_items_cache.recycle(item);
                return NULL;
            }
            return item;
        }

        //-------------------------------------------------
        //ɾ��һ����ʱ��
        bool m_timer_remove(tw::timer_item_t *item)
        {
            rx_assert(item->w_slot_idx < tw::wheel_slots);
            rx_assert(item->w_slot_link != NULL);
            rx_assert(item->w_wheel_idx < max_wheels);

            if (!m_wheels[item->w_wheel_idx].pick(*item))
                return false;
            m_items_cache.recycle(item);
            return true;
        }

        //-------------------------------------------------
        //��ָ���Ķ�ʱ�������µ�λ��
        bool m_timer_update(tw::timer_item_t *item, size_t inv_ticks)
        {
            rx_assert(item->w_slot_idx < tw::wheel_slots);
            rx_assert(item->w_slot_link != NULL);
            rx_assert(item->w_wheel_idx < max_wheels);

            uint8_t wheel_idx;
            uint8_t slot_idx;
            //������λ��
            uint16_t round = m_calc_pos(inv_ticks, wheel_idx, slot_idx);

            if (wheel_idx == item->w_wheel_idx)
            {//�����λ�ú;�λ�ö���һ��������,��ֱ�ӽ����ƶ�����
                if (slot_idx == item->w_slot_idx)
                    return true;                            //����,�����ƶ�

                item->w_round = round;
                if (!m_wheels[item->w_wheel_idx].move(*item, slot_idx))
                {//�ƶ����ɹ�,���յ�ǰ��ʱ��,����ʹ��
                    m_items_cache.recycle(item);
                    return false;
                }
                return true;
            }

            //�Ӿ�������ժ��
            m_wheels[item->w_wheel_idx].pick(*item);

            item->w_slot_idx = slot_idx;
            item->w_wheel_idx = wheel_idx;
            item->w_round = round;

            if (!m_wheels[item->w_wheel_idx].put(*item))
            {//��λ�ò��벻�ɹ�,���յ�ǰ��ʱ��,����ʹ��
                m_items_cache.recycle(item);
                return false;
            }
            return true;
        }
    public:
        //-------------------------------------------------
        //Ĭ�Ϲ��첻��ʼ��(����ȷ������ʼʱ��Ž��й����ʼ��).
        timing_wheel_t(uint64_t curr_time_ms=0,uint32_t tick_unit_ms=1,mem_allotter_i& ma=rx_global_mem_allotter())
        {
            if (curr_time_ms)
                wheels_init(curr_time_ms,tick_unit_ms,ma);
        }
        ~timing_wheel_t(){wheels_uninit();}

        //-------------------------------------------------
        //ʱ���ֳ�ʼ��:������ʼʱ���;�ڲ�ʱ��۵�ʱ�䵥λ(Ĭ��1ms);ʱ����ʹ�õ��ڴ������.
        bool wheels_init(uint64_t curr_time_ms,uint32_t tick_unit_ms=1,mem_allotter_i& mem=rx_global_mem_allotter())
        {
            if (!m_items_cache.bind(mem))
                return false;

            for(uint32_t i=0;i<max_wheels;++i)
            {
                tw::wheel_t  &w=m_wheels[i];
                if (!w.init(i,mem))                                 //��һ��ʼ����������
                    return false;

                w.cb_round.bind(mr_this(timing_wheel_t, on_round));//����ǰʱ���ֹҽ�����ת���¼�
                w.cb_item.bind(mr_this(timing_wheel_t, on_item));  //����ǰʱ���ֹҽ���Ŀ�����¼�
            }

            m_last_step_time = curr_time_ms;                        //��¼��������ʱ�����ײ�ʱ��۵�λ
            m_tick_unit_ms = tick_unit_ms;

            return true;
        }

        //-------------------------------------------------
        //���
        void wheels_uninit()
        {
            for(uint32_t i=0;i<max_wheels;++i)
            {
                tw::wheel_t &w=m_wheels[i];
                w.uninit(m_items_cache);
            }
            m_items_cache.clear();
        }

        //-------------------------------------------------
        //������ǰʱ����:������ǰʱ��.
        //����ֵ:���������Ķ�ʱ����Ŀ����;-1ʱ����ʱ��ͻ����,�ȴ��´ι���.
        uint32_t wheels_step(uint64_t curr_time_ms)
        {
            if (curr_time_ms < m_last_step_time)
            {
                m_last_step_time = curr_time_ms;
                return -1;
            }

            //��������ʱ��
            uint64_t delta=curr_time_ms-m_last_step_time;
            //����Ӧ���߶��Ĳ���
            uint32_t step_count=uint32_t(delta/m_tick_unit_ms);

            uint32_t rc=0;
            //������ͼ�ʱ��������(��round��Ȧ�ص��л�ͬ�������ϼ�ʱ����)
            for(uint32_t i=0;i<step_count;++i)
                rc+=m_wheels[min_level_wheel].step();

            //��¼������ߵ�ʱ���
            m_last_step_time+=step_count*m_tick_unit_ms;
            return rc;
        }

        //-------------------------------------------------
        //����һ���µĶ�ʱ��:�ص�����ָ��;�ص�����;����δ���(*tick_unit_ms��Ϊ���ʱ��ms);�¼���;�ظ�����
        //����ֵ:0ʧ��;����Ϊ��ʱ�����.
        size_t timer_insert(on_timer_t func,void* usrdat, size_t inv_tick, uint32_t event_code=0, uint16_t repeat=-1)
        {
            if (repeat == 0) return 0;

            tw::timer_item_t *item = m_timer_create(inv_tick, event_code, repeat);
            if (!item) return 0;

            item->u_cb_func.bind(func, usrdat);
            return item->handle();
        }
        template<class H>
        size_t timer_insert(H& owner, void(H::*member_func)(size_t handle, uint32_t event_code, uint16_t repeat), size_t inv_tick, uint32_t event_code = 0, uint16_t repeat = -1)
        {
            if (repeat == 0) return 0;

            tw::timer_item_t *item = m_timer_create(inv_tick, event_code, repeat);
            if (!item) return 0;

            item->u_cb_func.bind(owner, member_func);
            return item->handle();
        }

        //-------------------------------------------------
        //ɾ��һ����ʱ��
        bool timer_remove(size_t timer)
        {
            tw::timer_item_t *item = tw::timer_item_t::to_ptr(timer);
            rx_assert_ret(!item);
            return m_timer_remove(item);
        }

        //-------------------------------------------------
        //����һ����ʱ��(�Ե�ǰʱ���ʹ������,������һ�εĴ���ʱ��)
        bool timer_update(size_t timer)
        {
            tw::timer_item_t *item = tw::timer_item_t::to_ptr(timer);
            rx_assert_ret(!item);
            return m_timer_update(item,item->u_inv_tick);
        }
    };




}

#endif

#ifndef _RX_DTL_RINGBUFF_H_
#define _RX_DTL_RINGBUFF_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_os_lock.h"
#include "rx_os_spinlock.h"


namespace rx
{
    //-----------------------------------------------------
    //https://github.com/willemt/bipbuffer/blob/master/bipbuffer.c
/*
    bipbufferͨ�����������鶯̬����ΪA/B��������,�����˻��λ�������ֱ�����ݶ�ȡ������,�����ؽ���β�λ��Ƶ��µĻ�����ȡ���ݿ�������.
    ��Ӧ�õĽǶȿ�,���ݵĴ�����Ȼ��Ҫmemcpy,��ȡ����������Ҫ,����������ռ�������ʱ�������ringbuffer�Ե�,��������Ԥ�����ݳ���.
    Ҫ��:ÿ�η�����ȡ�����ݵĶ���,����Ҫ������Ʋ��������.

    |{    B region      }     {     A region    }    |
    |                   |     |                 |    |
 m_data               b_end a_start           a_end  m_capacity
*/
    //-----------------------------------------------------
    //����bipbufferԭ���װһ��ѭ�����������ܶ���
    class bipbuff_base
    {
    protected:
        uint8_t     b_inuse;                                //���B�����Ƿ�ʹ����
        uint32_t    b_end;                                  //��¼B����Ľ�����λ��
        uint32_t    a_start;                                //��¼A����Ŀ�ʼ��λ��
        uint32_t    a_end;                                  //��¼A����Ľ�����λ��
        uint32_t    m_capacity;                             //��¼��������������
        uint8_t    *m_data;                                 //������ָ��
        //-------------------------------------------------
        //�ڲ���ʼ��,��¼��Ҫ����Ϣ
        void m_init(const uint32_t capacity,uint8_t *buff)
        {
            m_capacity = capacity;
            m_data = buff;
            clear();
        }
    private:
        //�ж��Ƿ�Ӧ���л���B��
        void try_switch_to_b()
        {
            if (m_capacity - a_end < a_start - b_end)
                b_inuse = 1;  //���A���ĺ�ʣ��ռ�С��A��ǰ��B�����ÿռ�,���л���B��
        }

    public:
        //-------------------------------------------------
        //��ȡ����ʣ��ռ�ߴ�(��������������ʣ��ռ�)
        uint32_t remain()
        {
            if (b_inuse)
                return a_start - b_end;                     //B����ʹ�õ�ʱ��,���õ�ʣ��ռ����B����ʣ��ռ�
            else
                return m_capacity - a_end;                  //B��δ��ʹ�õ�ʱ��,���õ�ʣ��ռ����A����ʣ��ռ�
        }
        //-------------------------------------------------
        //��ȡ����������������
        uint32_t capacity() { return m_capacity; }
        //-------------------------------------------------
        //��ȡ��ֱ�Ӷ�ȡ�����ݳ���(�������ܳ���)
        uint32_t size(bool is_total=false) 
        { 
            if (is_total)
                return (a_end - a_start) + b_end;           //A����B���������ܳ�
            else
                return (a_end - a_start);                   //��ǰ�ɶ���������A����
        }       
        //-------------------------------------------------
        //��յ�ǰ������
        void clear() { a_start = a_end = b_end = 0; b_inuse = 0; }
        //-------------------------------------------------
        //�жϻ������Ƿ�Ϊ��
        bool empty() { return a_start == a_end; }
        //-------------------------------------------------
        //���ݷ��뻺����(������B�����л�����)
        //����ֵ:�Ƿ����ɹ�(��ǰ��ʣ��ռ��Ƿ��㹻)
        bool push(const unsigned char *data, const uint32_t size)
        {
            if (remain() < size)
                return false;                               //������п���ʣ��ռ���ж�

            if (b_inuse)
            {//B����ʹ����,��ֱ�ӽ����ݷ���B����β��,B������
                memcpy(m_data + b_end, data, size);
                b_end += size;
            }
            else
            {//B��û�б�ʹ��,�����ݷ���A����β��,A������
                memcpy(m_data + a_end, data, size);
                a_end += size;
                try_switch_to_b();                          //�ж��Ƿ�Ӧ���л���B��
            }
            return true;
        }
        //-------------------------------------------------
        //�ӻ�������ȡ������(������鿴������ʣ������)
        uint8_t *pop(uint32_t size = 0, bool is_peek = false)
        {
            if (empty())
                return NULL;                                //û������ֱ�ӷ��ؿ�

            if (!size)
                size = this->size();                        //û��ָ������ȡ�ߴ��ʱ��,���Ƿ��ʵ�ǰ��ȫ����������

            if (m_capacity < a_start + size)                //�����������Ǵ�A������
                return NULL;                                //A��û�п��õ���ô������ʱ,Ҳ���ؿ�.

            uint8_t *pos = m_data + a_start;                //��¼���ݷ��ʵ�,�����A����ʼ

            if (is_peek)
                return pos;                                 //�鿴ģʽֱ�ӷ���

            a_start += size;                                //��ȡģʽ,���ƶ�A���Ŀ�ʼ��

            if (a_start == a_end)
            {//���A�������ݱ�ȡ����
                if (b_inuse)
                {//���B����ʹ����,����B��תA���Ķ���
                    a_start = 0;
                    a_end = b_end;
                    b_end = b_inuse = 0;                    //�ر�B��
                }
                else //���B��û�б�ʹ��,��A���ƶ����������Ŀ�ʼ.
                    a_start = a_end = 0;
            }
            else //���A������ʣ������,���Խ���B�����л�
                try_switch_to_b();

            return pos;
        }
    };

    //-----------------------------------------------------
    //��̬�ռ�Ļ��λ�����;����CP.
    template<uint32_t CP = 256>
    class ringbuff_fixed :public bipbuff_base
    {
        uint8_t  m_buff[CP];                                //���������Ļ������ռ�
    public:
        ringbuff_fixed() { bipbuff_base::m_init(CP,m_buff); }//����ʱ���г�ʼ��
    };
}



#endif
#ifndef _RX_NET_SOCK_EXT_H_
#define _RX_NET_SOCK_EXT_H_

#include "rx_net_sock_std.h"
#include "rx_ct_delegate.h"
#include <memory.h>

namespace rx
{
    namespace sock
    {
        //-------------------------------------------------
        //�����շ������¼���ί������:sock,sock�û�����,�������ݻ�����,�������ݳ���,�ܳ���,����ֵ��֪�Ƿ����
        typedef delegate5_t<socket_t,void*,uint8_t*,uint32_t,uint32_t,bool> event_rw_t;

        //�������������ݽӿ�
        class recv_data_i
        {
        public:
            //������ջ�����:�ڴ�����󻺳�������;ʵ�ʿ��û���������;����ֵ:NULLʧ��;�����ɹ�
            virtual uint8_t* get(uint32_t maxsize,uint32_t &realsize)=0;
            //��֪������ʵ��ʹ�ó���;����ֵ:�Ƿ��������
            virtual bool put(uint32_t datasize)=0;
            //�Ѿ����յ����ݳߴ�
            virtual uint32_t size()=0;
        };
        //-------------------------------------------------
        //�����������ݵĻ�����
        class recv_buff_i:public recv_data_i
        {
        protected:
            uint8_t     *m_buff;                            //���ջ�����
            uint32_t    m_maxsize;                          //�����ճ���
            bool        m_must;                             //�Ƿ���������󳤶�
            uint32_t    m_size;                             //�Ѿ����յĳ���
        public:
            //�󶨽��ջ�����:���ջ�����,�����ճ���,�Ƿ�������.
            recv_buff_i(uint8_t* buff,uint32_t maxsize,bool mustsize=false):m_buff(buff),m_maxsize(maxsize),m_must(mustsize),m_size(0){}
            //������ջ�����:�ڴ�����󻺳�������;ʵ�ʿ��û���������;����ֵ:NULLʧ��;�����ɹ�
            uint8_t* get(uint32_t maxsize,uint32_t &realsize)
            {
                realsize=min(m_maxsize-m_size,maxsize);
                return m_buff+m_size;
            }
            //�������,��֪ʵ�ʽ������ݳ���;����ֵ:�Ƿ��������
            bool put(uint32_t datasize)
            {
                m_size+=datasize;
                rx_assert(m_size<=m_maxsize);
                if (m_must&&m_size==m_maxsize)
                    return false;
                return true;
            }
            //�Ѿ����յ����ݳߴ�
            uint32_t size(){return m_size;}
            //��������λ,׼�����½���ʹ��
            void reset(){m_size=0;}
        };

        //-------------------------------------------------
        //���Եȴ����յ�tag�ͽ���
        class recv_tag_i:public recv_buff_i
        {
        public:
            uint8_t     *tag;
            uint32_t    tagsize;
            uint32_t    tagpos;
            recv_tag_i(uint8_t* buff,uint32_t maxsize,bool mustsize=false):recv_buff_i(buff,maxsize,mustsize),tag(NULL),tagsize(0),tagpos(-1){}
            //�������,��֪ʵ�ʽ������ݳ���;����ֵ:�Ƿ��������
            bool put(uint32_t datasize)
            {
                bool ic=recv_buff_i::put(datasize);
                if (recv_buff_i::m_size>tagsize)
                {//�������㹻����,�ж��Ƿ���tag
                    rx_assert_if(tagsize,tag!=NULL);
                    const uint8_t* pos=st::memmemx(recv_buff_i::m_buff,recv_buff_i::m_size,tag,tagsize);
                    if (pos)
                    {//���ջ�������ȷʵ����tag��,���Խ���������.
                        tagpos=uint32_t(pos-recv_buff_i::m_buff); //tagpos��¼tag������buff�е�λ��
                        return false;
                    }
                }
                if (!ic)
                    return false;                           //����������,�����ټ���������
                return true;
            }
        };

        //-------------------------------------------------
        //�ɿ���ѭ�������͵������ȴ�ʱ��ĳ�����ȡ����:��ȡ������;���εȴ���ʱus;���չ۲��¼����¼�����;���ѭ������(0����);��ȡ���.
        //����ֵ:��֪ѭ������ԭ��,<0������;0���ӶϿ�;1�ȴ���ʱ;2�¼�Ҫ��ֹͣ;3������Ҫ��ֹͣ;4ѭ����������.
        inline int32_t read_loop(socket_t sock,recv_data_i& ri,uint32_t timeout_us=1000*500,
                                 event_rw_t* evt=NULL,void* evt_param=NULL,uint32_t loops=0,uint32_t flag=0)
        {
            uint32_t block_size=opt_buffsize_rd(sock);      //��ѯsocket�ĵײ㻺�����ߴ�
            if (block_size==(uint32_t)-1) return -1;

            uint32_t lps=0;
            int r;
            while(loops==0||++lps<=loops)                   //����ѭ����ȡ
            {
                uint32_t buff_size=0;
                uint8_t *buff=ri.get(block_size,buff_size); //�ӻ�������ȡ���ջ�����
                if (buff==NULL) return 3;

                if (timeout_us!=(uint32_t)-1)
                {//���Խ��н���ǰ�ĳ�ʱ�ȴ�,��������ģʽ��������״̬
                    r=wait(sock,timeout_us);
                    if (r<0)
                        return -2;
                    else if (r==0)
                        return 1;
                }

                r=recv(sock,buff,buff_size,flag);       //ִ�������Ľ��ն���
                if (r<0)
                    return -3;
                else if (r==0)
                    return 0;                               //���ն�����֪,�����Ѿ��Ͽ�

                bool is_break=!ri.put(r);                   //��֪���������ν��ճ���,��֪�Ƿ�����ж�ѭ��
                if (evt&&evt->is_valid())                   //�ٴγ���֪ͨ�¼��ص�:���ν��յ����ݺ����ݳ���,�ѽ��ճ���
                    is_break=!(*evt)(sock,evt_param,buff,r,ri.size())||is_break;  //�Ƿ���Ҫ�ж�ѭ�����ж���Ҫ����
                if (is_break)
                    return 2;                               //�¼��򻺳���Ҫ��ֹͣ����
            }
            return 4;                                       //ѭ��������������
        }
        //�ɿ���ѭ�������͵������ȴ�ʱ��ĳ�����ȡ����:ʵ�ʽ��ճ���;�������ͻ��峤��,�Ƿ�������������;���εȴ���ʱus;���չ۲��¼����¼�����;���ѭ������(0����);��ȡ���.
        //����ֵ:��֪ѭ������ԭ��,<0������;0���ӶϿ�;1�ȴ���ʱ;2�¼�Ҫ��ֹͣ;3������Ҫ��ֹͣ;4ѭ����������.
        inline int32_t read_loop(socket_t sock,uint32_t &recved,uint8_t* buff,uint32_t maxsize,bool mustsize=false,
                                 uint32_t timeout_us=1000*500,event_rw_t* evt=NULL,void* evt_param=NULL,uint32_t loops=0,uint32_t flag=0)
        {
            recv_buff_i ri(buff,maxsize,mustsize);
            int r=read_loop(sock,ri,timeout_us,evt,evt_param,loops,flag);
            recved=ri.size();
            return r;
        }
        //-------------------------------------------------
        //ͳһ��д�ӿ�:���ݻ�����;���ݳ���;д�ȴ���ʱ
        //����ֵ:<0����;0��ʱ;>0�ɹ�
        inline int32_t write_loop(socket_t sock,const void* datas,uint32_t data_size,
                                  uint32_t timeout_us=1000*500,event_rw_t* evt=NULL,void* evt_param=NULL,uint32_t flag=0)
        {
            uint32_t block_size=opt_buffsize_wr(sock);      //��ѯsocket�ĵײ㻺�����ߴ�
            if (block_size==(uint32_t)-1) return -1;

            uint32_t sended=0;                              //�Ѿ�д��������ݳ���
            while (sended<data_size)                        //ѭ��д��ȫ��������
            {
                uint32_t dl=min(block_size,data_size-sended);//������Ҫд������ݳ���
                if (timeout_us)
                {//���Խ��з���ǰ�ĳ�ʱ�ȴ�,����ײ㻺����������
                    int r=wait(sock,timeout_us,false);
                    if (r<0) return -2;
                    else if (r==0) return 0;
                }
                uint8_t *buff=(uint8_t*)datas+sended;       //�����ͻ�����ָ��
                int32_t rl=send(sock,buff,dl,flag);         //�����ķ��Ͷ���
                if (rl<0) return -3;
                if (evt&&evt->is_valid())
                    (*evt)(sock,evt_param,buff,rl,sended);  //֪ͨ�¼��ص�:���η��͵����ݺ����ݳ���,�ѷ������ݵ��ܳ���

                sended+=rl;                                 //����������ʵд��������ݳ���
            }
            return sended;
        }
    }
}

#endif

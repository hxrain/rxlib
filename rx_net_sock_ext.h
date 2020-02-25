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
        //定义收发数据事件的委托类型:sock,sock用户数据,本次数据缓冲区,本次数据长度,总长度,返回值告知是否继续
        typedef delegate5_t<socket_t,void*,uint8_t*,uint32_t,uint32_t,bool> event_rw_t;

        //持续化接收数据接口
        class recv_data_i
        {
        public:
            //申请接收缓冲区:期待的最大缓冲区长度;实际可用缓冲区长度;返回值:NULL失败;其他成功
            virtual uint8_t* get(uint32_t maxsize,uint32_t &realsize)=0;
            //告知缓冲区实际使用长度;返回值:是否继续接收
            virtual bool put(uint32_t datasize)=0;
            //已经接收的数据尺寸
            virtual uint32_t size()=0;
        };
        //-------------------------------------------------
        //持续接收数据的缓冲器
        class recv_buff_i:public recv_data_i
        {
        protected:
            uint8_t     *m_buff;                            //接收缓冲区
            uint32_t    m_maxsize;                          //最大接收长度
            bool        m_must;                             //是否必须接满最大长度
            uint32_t    m_size;                             //已经接收的长度
        public:
            //绑定接收缓冲区:接收缓冲区,最大接收长度,是否必须接满.
            recv_buff_i(uint8_t* buff,uint32_t maxsize,bool mustsize=false):m_buff(buff),m_maxsize(maxsize),m_must(mustsize),m_size(0){}
            //申请接收缓冲区:期待的最大缓冲区长度;实际可用缓冲区长度;返回值:NULL失败;其他成功
            uint8_t* get(uint32_t maxsize,uint32_t &realsize)
            {
                realsize=min(m_maxsize-m_size,maxsize);
                return m_buff+m_size;
            }
            //接收完成,告知实际接收数据长度;返回值:是否继续接收
            bool put(uint32_t datasize)
            {
                m_size+=datasize;
                rx_assert(m_size<=m_maxsize);
                if (m_must&&m_size==m_maxsize)
                    return false;
                return true;
            }
            //已经接收的数据尺寸
            uint32_t size(){return m_size;}
            //缓冲器复位,准备重新接收使用
            void reset(){m_size=0;}
        };

        //-------------------------------------------------
        //尝试等待接收到tag就结束
        class recv_tag_i:public recv_buff_i
        {
        public:
            uint8_t     *tag;
            uint32_t    tagsize;
            uint32_t    tagpos;
            recv_tag_i(uint8_t* buff,uint32_t maxsize,bool mustsize=false):recv_buff_i(buff,maxsize,mustsize),tag(NULL),tagsize(0),tagpos(-1){}
            //接收完成,告知实际接收数据长度;返回值:是否继续接收
            bool put(uint32_t datasize)
            {
                bool ic=recv_buff_i::put(datasize);
                if (recv_buff_i::m_size>tagsize)
                {//缓冲区足够长了,判断是否含有tag
                    rx_assert_if(tagsize,tag!=NULL);
                    const uint8_t* pos=st::memmemx(recv_buff_i::m_buff,recv_buff_i::m_size,tag,tagsize);
                    if (pos)
                    {//接收缓冲区中确实含有tag了,可以结束接收了.
                        tagpos=uint32_t(pos-recv_buff_i::m_buff); //tagpos记录tag出现在buff中的位置
                        return false;
                    }
                }
                if (!ic)
                    return false;                           //缓冲区满了,不能再继续接收了
                return true;
            }
        };

        //-------------------------------------------------
        //可控制循环数量和单次最大等待时间的持续读取功能:读取缓冲器;单次等待超时us;接收观察事件和事件参数;最大循环次数(0不限);读取标记.
        //返回值:告知循环结束原因,<0错误了;0连接断开;1等待超时;2事件要求停止;3缓冲器要求停止;4循环到达上限.
        inline int32_t read_loop(socket_t sock,recv_data_i& ri,uint32_t timeout_us=1000*500,
                                 event_rw_t* evt=NULL,void* evt_param=NULL,uint32_t loops=0,uint32_t flag=0)
        {
            uint32_t block_size=opt_buffsize_rd(sock);      //查询socket的底层缓冲区尺寸
            if (block_size==(uint32_t)-1) return -1;

            uint32_t lps=0;
            int r;
            while(loops==0||++lps<=loops)                   //进行循环读取
            {
                uint32_t buff_size=0;
                uint8_t *buff=ri.get(block_size,buff_size); //从缓冲器获取接收缓冲区
                if (buff==NULL) return 3;

                if (timeout_us!=(uint32_t)-1)
                {//尝试进行接收前的超时等待,避免阻塞模式进入死等状态
                    r=wait(sock,timeout_us);
                    if (r<0)
                        return -2;
                    else if (r==0)
                        return 1;
                }

                r=recv(sock,buff,buff_size,flag);       //执行真正的接收动作
                if (r<0)
                    return -3;
                else if (r==0)
                    return 0;                               //接收动作告知,连接已经断开

                bool is_break=!ri.put(r);                   //告知缓冲器本次接收长度,获知是否可以中断循环
                if (evt&&evt->is_valid())                   //再次尝试通知事件回调:本次接收的数据和数据长度,已接收长度
                    is_break=!(*evt)(sock,evt_param,buff,r,ri.size())||is_break;  //是否需要中断循环的判断需要叠加
                if (is_break)
                    return 2;                               //事件或缓冲器要求停止接收
            }
            return 4;                                       //循环次数到达上限
        }
        //可控制循环数量和单次最大等待时间的持续读取功能:实际接收长度;缓冲区和缓冲长度,是否必须接满缓冲区;单次等待超时us;接收观察事件和事件参数;最大循环次数(0不限);读取标记.
        //返回值:告知循环结束原因,<0错误了;0连接断开;1等待超时;2事件要求停止;3缓冲器要求停止;4循环到达上限.
        inline int32_t read_loop(socket_t sock,uint32_t &recved,uint8_t* buff,uint32_t maxsize,bool mustsize=false,
                                 uint32_t timeout_us=1000*500,event_rw_t* evt=NULL,void* evt_param=NULL,uint32_t loops=0,uint32_t flag=0)
        {
            recv_buff_i ri(buff,maxsize,mustsize);
            int r=read_loop(sock,ri,timeout_us,evt,evt_param,loops,flag);
            recved=ri.size();
            return r;
        }
        //-------------------------------------------------
        //统一的写接口:数据缓冲区;数据长度;写等待超时
        //返回值:<0错误;0超时;>0成功
        inline int32_t write_loop(socket_t sock,const void* datas,uint32_t data_size,
                                  uint32_t timeout_us=1000*500,event_rw_t* evt=NULL,void* evt_param=NULL,uint32_t flag=0)
        {
            uint32_t block_size=opt_buffsize_wr(sock);      //查询socket的底层缓冲区尺寸
            if (block_size==(uint32_t)-1) return -1;

            uint32_t sended=0;                              //已经写入过的数据长度
            while (sended<data_size)                        //循环写入全部的数据
            {
                uint32_t dl=min(block_size,data_size-sended);//本次需要写入的数据长度
                if (timeout_us)
                {//尝试进行发送前的超时等待,避免底层缓冲区不够用
                    int r=wait(sock,timeout_us,false);
                    if (r<0) return -2;
                    else if (r==0) return 0;
                }
                uint8_t *buff=(uint8_t*)datas+sended;       //待发送缓冲区指针
                int32_t rl=send(sock,buff,dl,flag);         //真正的发送动作
                if (rl<0) return -3;
                if (evt&&evt->is_valid())
                    (*evt)(sock,evt_param,buff,rl,sended);  //通知事件回调:本次发送的数据和数据长度,已发送数据的总长度

                sended+=rl;                                 //调整本次真实写入过的数据长度
            }
            return sended;
        }
    }
}

#endif

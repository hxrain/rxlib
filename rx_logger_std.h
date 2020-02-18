#ifndef _RX_LOGGER_STD_H_
#define _RX_LOGGER_STD_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"
#include "rx_str_util_bin.h"
#include "rx_str_util_fmt.h"
#include "rx_str_tiny.h"
#include "rx_os_misc.h"

namespace rx
{
    //-----------------------------------------------------
    //日志消息类型
    typedef enum
    {
        LT_LEVEL_ERROR          =5,                         //错误信息
        LT_LEVEL_WARN           =4,                         //警告信息
        LT_LEVEL_INFO           =3,                         //常规信息
        LT_LEVEL_CYCLE          =2,                         //周期消息
        LT_LEVEL_DEBUG          =1,                         //调试消息
        LT_LEVEL_NONE           =0,                         //禁止输出
    }logger_level_t;

    inline const char* logger_level_name(logger_level_t type)
    {
        switch(type)
        {
            case LT_LEVEL_ERROR:return "ERR";
            case LT_LEVEL_WARN :return "WRN";
            case LT_LEVEL_INFO :return "INF";
            case LT_LEVEL_CYCLE:return "CYC";
            case LT_LEVEL_DEBUG:return "DBG";
            default:return "";
        }
    }

    //-----------------------------------------------------
    //日志输出器接口,可以用于控制台输出/文件输出/调试输出/网络传输等等目的
    //根据具体的实现,可以跨线程被多个日志记录器并发使用.
    class logger_writer_i
    {
    public:
        virtual void on_begin(uint64_t tex,logger_level_t type,uint32_t tag,const char* msg,uint32_t msgsize)=0;
        virtual void on_write(uint64_t tex,const void* data,uint32_t size)=0;
        virtual void on_end(uint64_t tex)=0;
        virtual ~logger_writer_i(){}
    };
    //每个日志记录器允许绑定输出器的最大数量
    const uint32_t max_logger_writer_count=5;

    //-----------------------------------------------------
    //日志记录器,可以绑定多个日志输出器,不能跨线程使用
    class logger_t
    {
        logger_writer_i     *m_writers[max_logger_writer_count];
        uint32_t            m_writer_count;                 //绑定过的输出器数量
        logger_level_t      m_can_level;                    //允许输出的日志级别,>=m_can_level才可以输出,默认为输出全部
        const char*         m_mod_name;                     //日志记录器所属的功能模块
        uint32_t            m_pid;
        //-------------------------------------------------
        //内部使用的日志输出器
        class writer_t
        {
            friend logger_t;
            logger_t        *parent;                        //父对象指针
            uint32_t        m_last_seq;                     //最后的日志序号
            uint64_t        m_last_tex;                     //最后的日志事务号,this<<32|seq,告知日志记录器每次的唯一事务

            //---------------------------------------------
            //fmt格式化需要的底层输出器
            class fmt_follower_logger:public fmt_imp::fmt_follower_null<char>
            {
                typedef fmt_imp::fmt_follower_null<char> super_t;
                writer_t        *owner;
                char            m_buff[512];                //内部持有的临时缓冲区
            public:
                //-----------------------------------------
                fmt_follower_logger(writer_t* o)
                {
                    owner=o;
                    super_t::bind(m_buff,sizeof(m_buff));
                }
                //-----------------------------------------
                void operator ()(char character)
                {//格式化后的内容先暂存一下,避免逐字符输出给底层
                    super_t::buffer[super_t::idx++] = character;
                    ++super_t::count;
                    if (super_t::idx >= super_t::maxlen)
                    {
                        //循环输出给所有绑定的输出器
                        for(uint32_t i=0;i<owner->parent->m_writer_count;++i)
                            owner->parent->m_writers[i]->on_write(owner->m_last_tex,super_t::buffer,(uint32_t)super_t::maxlen);
                        super_t::idx=0;
                    }
                }
                //-----------------------------------------
                void end()
                {//还有剩余的格式化结果则全部输出给底层
                    if (super_t::idx)
                    {
                        //循环输出给所有绑定的输出器
                        for(uint32_t i=0;i<owner->parent->m_writer_count;++i)
                            owner->parent->m_writers[i]->on_write(owner->m_last_tex,super_t::buffer,(uint32_t)super_t::idx);
                        super_t::idx=0;
                    }
                }
            };
        public:
            //---------------------------------------------
            writer_t():parent(NULL),m_last_seq(0),m_last_tex(0){}
            //---------------------------------------------
            //输出日志的前端固定部分
            writer_t& begin(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1)
            {
                rx_assert(parent!=NULL);
                end();
                if (type<parent->m_can_level)
                    return *this;
                rx_assert(m_last_tex==0);
                //构造本次事务序号
                m_last_tex=(size_t)this;
                m_last_tex<<=32;
                m_last_tex|=++m_last_seq;

                tiny_string_t<char,512> scat;

                //拼装当前时间/日志级别/pid/tid
                char buff[512];
                rx_iso_datetime(time(NULL),buff);
                scat("[%s|",buff) ("%s|",logger_level_name(type)) ("PID:%6u|",parent->m_pid) ("TID:%6u]",get_tid());

                //尝试输出有效的mod名称和tag标记
                if (!is_empty(parent->m_mod_name))
                    scat("[%s]",tag);
                if (tag!=(uint32_t)-1)
                    scat("[TAG:%6u]",tag);

                //循环输出给所有绑定的输出器
                for(uint32_t i=0;i<parent->m_writer_count;++i)
                    parent->m_writers[i]->on_begin(m_last_tex,type,tag,buff,scat.size());

                return *this;
            }
            //---------------------------------------------
            //格式化输出日志的具体内容部分
            writer_t& vfmt(const char* fmt,va_list ap)
            {
                rx_assert(m_last_tex!=0);
                fmt_follower_logger fbuf(this);
                fmt_imp::fmt_core(fbuf, fmt, ap);
                return *this;
            }
            writer_t& operator()(const char* fmt,...)
            {
                va_list ap;
                va_start(ap,fmt);
                vfmt(fmt,ap);
                va_end(ap);
                return *this;
            }
            //---------------------------------------------
            //以十六进制字符串方式输出数据部分:数据缓冲区指针,数据长度,前缀空格数量(最大64),每行字节数量(最大256)
            writer_t& hex(const void* data,uint32_t size,uint32_t pre_tab=4,uint32_t line_bytes=32)
            {
                rx_assert(m_last_tex!=0);

                char line_buff[256*4];                      //当前行输出缓冲区
                line_bytes=min(line_bytes,(uint32_t)256);   //每行最大输出字节数量
                pre_tab=min(pre_tab,(uint32_t)64);          //每行最大前缀空格数量
                const uint8_t *bytes=(const uint8_t*)data;  //待处理的缓冲区指针
                uint32_t remain=size;                       //剩余数据长度
                while(remain)
                {
                    uint32_t blocks=min(line_bytes,remain); //本行待处理的字节长度
                    uint32_t sl=0;
                    for(;sl<pre_tab;++sl)
                        line_buff[sl]=' ';                  //当前行填充前缀

                    for(uint32_t t=0;t<blocks;++t)
                    {//循环输出当前行数据为hex
                        st::hex2(bytes[t],line_buff+sl);
                        ++bytes;
                        sl+=2;
                        line_buff[sl++]=' ';
                    }
                    remain-=blocks;
                    if (remain)                             //不是最后的行,那么行输出缓冲区就补上回车换行和结束符
                    {
                        line_buff[sl++]='\n';
                        line_buff[sl]='0';
                    }
                    //循环输出给所有绑定的输出器
                    for(uint32_t i=0;i<parent->m_writer_count;++i)
                        parent->m_writers[i]->on_write(m_last_tex,line_buff,sl);
                }

                return *this;
            }
            //---------------------------------------------
            //以原始二进制方式输出数据部分
            writer_t& bin(const void* data,uint32_t size)
            {
                rx_assert(m_last_tex!=0);
                //循环输出给所有绑定的输出器
                for(uint32_t i=0;i<parent->m_writer_count;++i)
                    parent->m_writers[i]->on_write(m_last_tex,data,size);
                return *this;
            }
            //---------------------------------------------
            //告知本次日志输出结束了
            void end()
            {
                if (m_last_tex==0)
                    return;
                //循环通知给所有绑定的输出器
                for(uint32_t i=0;i<parent->m_writer_count;++i)
                    parent->m_writers[i]->on_end(m_last_tex);
                m_last_tex=0;
            }
        };

    public:
        logger_t(const char* mod=""):m_writer_count(0),m_can_level(LT_LEVEL_DEBUG),m_mod_name(mod)
        {
            rx_assert(m_mod_name!=NULL);
            writer.parent=this;
            m_pid=(uint32_t)get_pid();
        }
        //-------------------------------------------------
        //底层输出接口,更灵活
        writer_t writer;
        writer_t& operator()(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1){return writer.begin(type,tag);}
        //-------------------------------------------------
        //绑定输出器接口
        bool bind(logger_writer_i& w)
        {
            if (m_writer_count>=max_logger_writer_count)
                return false;
            m_writers[m_writer_count++]=&w;
        }
        //-------------------------------------------------
        //输出错误信息
        void error(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_ERROR,tag,fmt,ap);
            va_end(ap);
        }
        void error(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_ERROR,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        //输出警告信息
        void warn(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_WARN,tag,fmt,ap);
            va_end(ap);
        }
        void warn(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_WARN,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        //输出普通信息
        void info(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_INFO,tag,fmt,ap);
            va_end(ap);
        }
        void info(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_INFO,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        //输出周期性信息
        void cycle(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_CYCLE,tag,fmt,ap);
            va_end(ap);
        }
        void cycle(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_CYCLE,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        //输出调试信息
        void debug(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_DEBUG,tag,fmt,ap);
            va_end(ap);
        }
        void debug(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            print(LT_LEVEL_DEBUG,-1,fmt,ap);
            va_end(ap);
        }
        //---------------------------------------------
        //各类输出信息的底层格式化方法
        void print(logger_level_t type,uint32_t tag,const char* fmt,va_list ap)
        {
            if (type<m_can_level)
                return;
            writer.begin(type,tag).vfmt(fmt,ap).end();
        }
        //-------------------------------------------------
        //调整允许的输出级别,0完全禁止输出
        void level(logger_level_t lvl){m_can_level=lvl;}
    };
}


#endif

#ifndef _RX_LOGGER_STD_H_
#define _RX_LOGGER_STD_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"

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
        virtual void on_write(uint64_t tex,const void* msg,uint32_t msgsize)=0;
        virtual void on_end(uint64_t tex,const void* msg,uint32_t msgsize)=0;
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
        //-------------------------------------------------
        //内部使用的日志输出器
        class writer_t
        {
            friend logger_t;
            logger_t        *parent;
            uint32_t        m_last_seq;
            uint64_t        m_last_tex;
            //---------------------------------------------
            void print(logger_level_t type,uint32_t tag,const char* fmt,va_list ap)
            {
                rx_assert(parent!=NULL);
                if (type<parent->m_can_level)
                    return;
            }
        public:
            //---------------------------------------------
            writer_t():parent(NULL),m_last_seq(0){}
            //---------------------------------------------
            writer_t& begin(logger_level_t type,uint32_t tag)
            {
                rx_assert(parent!=NULL);
                if (type<parent->m_can_level)
                    return *this;

                return *this;
            }
            //---------------------------------------------
            writer_t& fmt()
            {
                return *this;
            }
            //---------------------------------------------
            writer_t& hex(const void* data,uint32_t size)
            {
                return *this;
            }
            //---------------------------------------------
            writer_t& bin(const void* data,uint32_t size)
            {
                return *this;
            }
            //---------------------------------------------
            void end()
            {
                
            }
        };

    public:
        logger_t(const char* mod=""):m_writer_count(0),m_can_level(LT_LEVEL_DEBUG),m_mod_name(mod)
        {
            rx_assert(m_mod_name!=NULL);
            writer.parent=this;
        }
        //-------------------------------------------------
        //底层输出接口,更灵活
        writer_t writer;
        //-------------------------------------------------
        //绑定输出器接口
        bool bind(logger_writer_i& w)
        {
            if (m_writer_count>=max_logger_writer_count) 
                return false;
            m_writers[m_writer_count++]=&w;
        }
        //-------------------------------------------------
        void error(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_ERROR,tag,fmt,ap);
            va_end(ap);
        }
        void error(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_ERROR,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        void warn(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_WARN,tag,fmt,ap);
            va_end(ap);
        }
        void warn(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_WARN,-1,fmt,ap);
            va_end(ap);
        }        
        //-------------------------------------------------
        void info(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_INFO,tag,fmt,ap);
            va_end(ap);
        }
        void info(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_INFO,-1,fmt,ap);
            va_end(ap);
        }  
        //-------------------------------------------------
        void cycle(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_CYCLE,tag,fmt,ap);
            va_end(ap);
        }
        void cycle(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_CYCLE,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        void debug(uint32_t tag,const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_DEBUG,tag,fmt,ap);
            va_end(ap);
        }
        void debug(const char* fmt,...)
        {
            va_list ap;
            va_start(ap,fmt);
            writer.print(LT_LEVEL_DEBUG,-1,fmt,ap);
            va_end(ap);
        }
        //-------------------------------------------------
        //调整允许的输出级别,0完全禁止输出
        void level(logger_level_t lvl){m_can_level=lvl;}
    };

}


#endif

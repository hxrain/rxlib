#ifndef _RX_LOGGER_STD_H_
#define _RX_LOGGER_STD_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"
#include "rx_str_util_bin.h"
#include "rx_str_util_fmt.h"
#include "rx_str_tiny.h"
#include "rx_os_misc.h"
#include "rx_os_lock.h"
/*
    //日志记录器接口
    class logger_i;

    //日志记录器,可以绑定多个日志输出器,被多线程中的日志记录器接口引用
    template<class LT>
    class logger_t;

    //日志输出器:控制台输出
    class logger_wrcon_t;

    //日志输出器:文件输出
    class logger_wrfile_t;
*/

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

    //语法糖,给日志记录器接口绑定模块特征
    #define logger_modname(logger) logger.modname(rx_src_filename,__LINE__)

    //-----------------------------------------------------
    //日志记录器接口
    class logger_i
    {
    protected:
        //日志记录器接口的核心功能可由子类来实现;默认时日志记录器接口的功能由绑定的其他接口处理.
        virtual bool on_can_write(logger_level_t type)
        {
            if (!m_logger) return false;
            return m_logger->on_can_write(type);
        }
        virtual void on_begin(logger_level_t type,uint32_t tag,uint64_t tex,const char* modname)
        {
            if (m_logger) 
                m_logger->on_begin(type,tag,tex,modname);
        }
        virtual void on_vfmt(const char* fmt,va_list ap,uint64_t tex)
        {
            if (m_logger) 
                m_logger->on_vfmt(fmt,ap,tex);
        }
        virtual void on_hex(const void* data,uint32_t size,uint32_t pre_tab,uint32_t line_bytes,uint64_t tex)
        {
            if (m_logger) 
                m_logger->on_hex(data,size,pre_tab,line_bytes,tex);
        }
        virtual void on_bin(const void* data,uint32_t size,uint64_t tex)
        {
            if (m_logger) 
                m_logger->on_bin(data,size,tex);
        }
        virtual void on_end(uint64_t tex)
        {
            if (m_logger) 
                m_logger->on_end(tex);
        }
        logger_i    *m_logger;                              //日志记录接口的底层转发接口指针
        //-----------------------------------------------------
        //内部使用的日志输出器
        class writer_t
        {
            friend class logger_i;
            logger_i        *parent;                        //父对象指针
            uint32_t        m_last_seq;                     //最后的日志序号
            uint64_t        m_last_tex;                     //最后的日志事务号,this<<32|seq,告知日志记录器每次的唯一事务
            char            m_mod_name[32];                 //日志记录器所属的功能模块
        public:
            //-------------------------------------------------
            writer_t():parent(NULL),m_last_seq(0),m_last_tex(0){m_mod_name[0]=0;}
            //-------------------------------------------------
            //输出日志的前端固定部分
            writer_t& begin(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1)
            {
                rx_assert(parent!=NULL);
                rx_assert_msg(m_last_tex==0,"must call end()");//要求之前的end()必须被调用

                if (!parent->on_can_write(type))
                    return *this;

                //构造本次事务序号
                m_last_tex=(size_t)this;
                m_last_tex<<=32;
                m_last_tex|=++m_last_seq;

                parent->on_begin(type,tag,m_last_tex,m_mod_name);

                return *this;
            }
            //-------------------------------------------------
            //格式化输出日志的具体内容部分
            writer_t& vfmt(const char* fmt,va_list ap)
            {
                parent->on_vfmt(fmt, ap, m_last_tex);
                return *this;
            }
            writer_t& fmt(const char* str,...)
            {
                va_list ap;
                va_start(ap,str);
                vfmt(str,ap);
                va_end(ap);
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
            //-------------------------------------------------
            //以十六进制字符串方式输出数据部分:数据缓冲区指针,数据长度,前缀空格数量(最大64),每行字节数量(最大256)
            writer_t& hex(const void* data,uint32_t size,uint32_t pre_tab=4,uint32_t line_bytes=32)
            {
                parent->on_hex(data,size,pre_tab,line_bytes,m_last_tex);
                return *this;
            }
            //-------------------------------------------------
            //以原始二进制方式输出数据部分
            writer_t& bin(const void* data,uint32_t size)
            {
                parent->on_bin(data,size,m_last_tex);
                return *this;
            }
            //-------------------------------------------------
            //告知本次日志输出结束了
            void end()
            {
                parent->on_end(m_last_tex);
                m_last_tex=0;
            }
        };

    public:
        logger_i():m_logger(NULL){writer.parent=this;}
        //-----------------------------------------------------
        //绑定日志功能转发接口
        void bind(logger_i &log){m_logger=&log;}
        //-----------------------------------------------------
        //绑定模块名称(名称是个性化的,随着日志接口的部署而不同)
        void modname(const char* name,uint32_t lno=0)
        {
            if (lno)
                st::snprintf(writer.m_mod_name,sizeof(writer.m_mod_name),"%s:%u",name,lno);
            else
                st::strcpy(writer.m_mod_name,sizeof(writer.m_mod_name),name);
        }
        //-----------------------------------------------------
        //底层输出接口,更灵活
        writer_t writer;
        //语法糖,当作writer.begin()使用
        writer_t& operator()(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1){return writer.begin(type,tag);}
        //-----------------------------------------------------
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
        //-----------------------------------------------------
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
        //-----------------------------------------------------
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
        //-----------------------------------------------------
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
        //-----------------------------------------------------
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
        //-------------------------------------------------
        //各类输出信息的底层格式化方法
        void print(logger_level_t type,uint32_t tag,const char* fmt,va_list ap)
        {//这也是日志编写器的正确用法示例,必须先begin(),最后再end()
            writer.begin(type,tag).vfmt(fmt,ap).end();
        }
    };

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
    const uint32_t max_logger_writer_count=4;

    //-----------------------------------------------------
    //日志记录器,可以绑定多个日志输出器,可跨线程被多个日志记录器接口引用
    template<class LT=null_lock_t>
    class logger_t:public logger_i
    {
        logger_writer_i     *m_writers[max_logger_writer_count];
        uint32_t            m_writer_count;                 //绑定过的输出器数量
        logger_level_t      m_can_level;                    //允许输出的日志级别,>=m_can_level才可以输出,默认为输出全部
        uint32_t            m_pid;
        LT                  m_locker;
        //-------------------------------------------------
        //fmt格式化需要的底层输出器
        class fmt_follower_logger:public fmt_imp::fmt_follower_null<char>
        {
            typedef fmt_imp::fmt_follower_null<char> super_t;
            logger_t        *parent;
            char            m_buff[512];                    //内部持有的临时缓冲区
            uint64_t        m_last_tex;
        public:
            //-----------------------------------------
            fmt_follower_logger(logger_t* o,uint64_t tex)
            {
                parent=o;
                m_last_tex=tex;
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
                    for(uint32_t i=0;i<parent->m_writer_count;++i)
                        parent->m_writers[i]->on_write(m_last_tex,super_t::buffer,(uint32_t)super_t::maxlen);
                    super_t::idx=0;
                }
            }
            //-----------------------------------------
            void end()
            {//还有剩余的格式化结果则全部输出给底层
                if (super_t::idx)
                {
                    //循环输出给所有绑定的输出器
                    for(uint32_t i=0;i<parent->m_writer_count;++i)
                        parent->m_writers[i]->on_write(m_last_tex,super_t::buffer,(uint32_t)super_t::idx);
                    super_t::idx=0;
                }
            }
        };
        //-----------------------------------------------------
        //判断是否可输出当前级别的日志内容
        virtual bool on_can_write(logger_level_t type)
        {
            return (type>=m_can_level)&&m_writer_count;
        }
        //-----------------------------------------------------
        //开始一次日志事务的处理
        virtual void on_begin(logger_level_t type,uint32_t tag,uint64_t tex,const char* modname)
        {
            rx_assert(tex!=0);

            m_locker.lock();

            tiny_string_t<char,512> scat;

            //拼装当前时间/日志级别/pid/tid
            char dt[24];
            rx_datetime2iso(dt,"%u-%02u-%02uT%02u:%02u:%02u.%03u",true);
            scat("[%s|",dt) ("%s|",logger_level_name(type)) ("PID:%4u|",m_pid) ("TID:%4u]",get_tid());

            //尝试输出有效的mod名称和tag标记
            if (!is_empty(modname))
                scat("[%s]",modname);
            if (tag!=(uint32_t)-1)
                scat("[TAG:%6u]",tag);
            scat("[TEX:%016zx]",tex);
            scat<<' ';

            //循环输出给所有绑定的输出器
            for(uint32_t i=0;i<m_writer_count;++i)
                m_writers[i]->on_begin(tex,type,tag,scat.c_str(),scat.size());
        }
        //-----------------------------------------------------
        //在当前日志事务中输出格式化拼装内容
        virtual void on_vfmt(const char* fmt,va_list ap,uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;
            fmt_follower_logger fbuf(this,tex);
            fmt_imp::fmt_core(fbuf, fmt, ap);
        }
        //-----------------------------------------------------
        //在当前日志事务中输出HEX数据内容
        virtual void on_hex(const void* data,uint32_t size,uint32_t pre_tab,uint32_t line_bytes,uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;

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
                    st::hex2(*bytes,line_buff+sl);
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
                for(uint32_t i=0;i<m_writer_count;++i)
                    m_writers[i]->on_write(tex,line_buff,sl);
            }
        }
        //-----------------------------------------------------
        //在当前日志事务中输出原始二进制数据内容
        virtual void on_bin(const void* data,uint32_t size,uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;

            //循环输出给所有绑定的输出器
            for(uint32_t i=0;i<m_writer_count;++i)
                m_writers[i]->on_write(tex,data,size);
        }
        //-----------------------------------------------------
        //结束一次日志事务
        virtual void on_end(uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;

            //循环通知给所有绑定的输出器
            for(uint32_t i=0;i<m_writer_count;++i)
                m_writers[i]->on_end(tex);

            m_locker.unlock();
        }
        //覆盖隐藏父接口中的转发绑定功能接口
        void bind(logger_i log){}
    public:
        logger_t():m_writer_count(0),m_can_level(LT_LEVEL_DEBUG)
        {
            m_pid=(uint32_t)get_pid();
        }
        //-----------------------------------------------------
        //绑定输出器接口
        bool bind(logger_writer_i& w)
        {
            if (m_writer_count>=max_logger_writer_count)
                return false;
            m_writers[m_writer_count++]=&w;
            return true;
        }
        //-----------------------------------------------------
        //调整允许的输出级别,0完全禁止输出
        void level(logger_level_t lvl){m_can_level=lvl;}
        //-----------------------------------------------------
        uint32_t writers(){return m_writer_count;}

    };

    //-----------------------------------------------------
    //日志输出器:控制台输出
    template<class LT=null_lock_t>
    class logger_wrcon_t:public logger_writer_i
    {
    protected:
        LT          m_locker;
        FILE        *m_file;
        //-----------------------------------------------------
        void on_begin(uint64_t tex,logger_level_t type,uint32_t tag,const char* msg,uint32_t msgsize)
        {
            if (!is_valid()) return;
            m_locker.lock();
            fwrite(msg,1,msgsize,m_file);
        }
        //-----------------------------------------------------
        void on_write(uint64_t tex,const void* data,uint32_t size)
        {
            if (!is_valid()) return;
            fwrite(data,1,size,m_file);
        }
        //-----------------------------------------------------
        void on_end(uint64_t tex)
        {
            if (!is_valid()) return;
            putc('\n',m_file);
            fflush(m_file);
            m_locker.unlock();
        }
        //-----------------------------------------------------
        virtual bool is_valid(){return true;}
    public:
        logger_wrcon_t():m_file(stdout){}
    };

    //-----------------------------------------------------
    //日志输出器:文件输出
    template<class LT=null_lock_t>
    class logger_wrfile_t:public logger_wrcon_t<LT>
    {
        typedef logger_wrcon_t<LT> super_t;
    public:
        //-----------------------------------------------------
        logger_wrfile_t(){super_t::m_file=NULL;}
        ~logger_wrfile_t(){close();}
        bool is_valid(){return super_t::m_file!=NULL;}
        //-----------------------------------------------------
        bool open(const char* filepath,bool append=false)
        {
            super_t::m_file=fopen(filepath,append?"ab+":"wb+");
            if (super_t::m_file==NULL)
                return false;
            return true;
        }
        //-----------------------------------------------------
        void close()
        {
            if (super_t::m_file)
            {
                fclose(super_t::m_file);
                super_t::m_file=NULL;
            }
        }
        //-----------------------------------------------------
    };

    //---------------------------------------------------------
    //可全局使用,输出到文件和控制台的日志记录器(单实例)
    logger_i& make_logger_confile(const char* fname="./log.txt",bool append=false)
    {
        static logger_t<locker_t> logger;
        static logger_wrfile_t<> logger_wrfile;
        static logger_wrcon_t<> logger_wrcon;
        if (logger.writers()==0)
        {
            rx_check(logger_wrfile.open(fname,append));
            logger.bind(logger_wrfile);
            logger.bind(logger_wrcon);
        }
        return logger;
    }

    //可全局使用,输出到文件的日志记录器(通过区分不同的seq可以使用多个实例)
    template<int seq>
    logger_i& make_logger_file(const char* fname="./log.txt",bool append=false)
    {
        static logger_t<locker_t> logger;
        static logger_wrfile_t<> logger_wrfile;
        if (logger.writers()==0)
        {
            rx_check(logger_wrfile.open(fname,append));
            logger.bind(logger_wrfile);
        }
        return logger;
    }

    //可全局使用,输出到控制台的日志记录器(单实例)
    logger_i& make_logger_con()
    {
        static logger_t<locker_t> logger;
        static logger_wrcon_t<> logger_wrcon;
        if (logger.writers()==0)
            logger.bind(logger_wrcon);
        return logger;
    }

}


#endif

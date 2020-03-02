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
    //��־��¼���ӿ�
    class logger_i;

    //��־��¼��,���԰󶨶����־�����,�����߳��е���־��¼���ӿ�����
    template<class LT>
    class logger_t;

    //��־�����:����̨���
    class logger_wrcon_t;

    //��־�����:�ļ����
    class logger_wrfile_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //��־��Ϣ����
    typedef enum
    {
        LT_LEVEL_ERROR          =5,                         //������Ϣ
        LT_LEVEL_WARN           =4,                         //������Ϣ
        LT_LEVEL_INFO           =3,                         //������Ϣ
        LT_LEVEL_CYCLE          =2,                         //������Ϣ
        LT_LEVEL_DEBUG          =1,                         //������Ϣ
        LT_LEVEL_NONE           =0,                         //��ֹ���
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

    //�﷨��,����־��¼���ӿڰ�ģ������
    #define logger_modname(logger) logger.modname(rx_src_filename,__LINE__)

    //-----------------------------------------------------
    //��־��¼���ӿ�
    class logger_i
    {
    protected:
        //��־��¼���ӿڵĺ��Ĺ��ܿ���������ʵ��;Ĭ��ʱ��־��¼���ӿڵĹ����ɰ󶨵������ӿڴ���.
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
        logger_i    *m_logger;                              //��־��¼�ӿڵĵײ�ת���ӿ�ָ��
        //-----------------------------------------------------
        //�ڲ�ʹ�õ���־�����
        class writer_t
        {
            friend class logger_i;
            logger_i        *parent;                        //������ָ��
            uint32_t        m_last_seq;                     //������־���
            uint64_t        m_last_tex;                     //������־�����,this<<32|seq,��֪��־��¼��ÿ�ε�Ψһ����
            char            m_mod_name[32];                 //��־��¼�������Ĺ���ģ��
        public:
            //-------------------------------------------------
            writer_t():parent(NULL),m_last_seq(0),m_last_tex(0){m_mod_name[0]=0;}
            //-------------------------------------------------
            //�����־��ǰ�˹̶�����
            writer_t& begin(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1)
            {
                rx_assert(parent!=NULL);
                rx_assert_msg(m_last_tex==0,"must call end()");//Ҫ��֮ǰ��end()���뱻����

                if (!parent->on_can_write(type))
                    return *this;

                //���챾���������
                m_last_tex=(size_t)this;
                m_last_tex<<=32;
                m_last_tex|=++m_last_seq;

                parent->on_begin(type,tag,m_last_tex,m_mod_name);

                return *this;
            }
            //-------------------------------------------------
            //��ʽ�������־�ľ������ݲ���
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
            //��ʮ�������ַ�����ʽ������ݲ���:���ݻ�����ָ��,���ݳ���,ǰ׺�ո�����(���64),ÿ���ֽ�����(���256)
            writer_t& hex(const void* data,uint32_t size,uint32_t pre_tab=4,uint32_t line_bytes=32)
            {
                parent->on_hex(data,size,pre_tab,line_bytes,m_last_tex);
                return *this;
            }
            //-------------------------------------------------
            //��ԭʼ�����Ʒ�ʽ������ݲ���
            writer_t& bin(const void* data,uint32_t size)
            {
                parent->on_bin(data,size,m_last_tex);
                return *this;
            }
            //-------------------------------------------------
            //��֪������־���������
            void end()
            {
                parent->on_end(m_last_tex);
                m_last_tex=0;
            }
        };

    public:
        logger_i():m_logger(NULL){writer.parent=this;}
        //-----------------------------------------------------
        //����־����ת���ӿ�
        void bind(logger_i &log){m_logger=&log;}
        //-----------------------------------------------------
        //��ģ������(�����Ǹ��Ի���,������־�ӿڵĲ������ͬ)
        void modname(const char* name,uint32_t lno=0)
        {
            if (lno)
                st::snprintf(writer.m_mod_name,sizeof(writer.m_mod_name),"%s:%u",name,lno);
            else
                st::strcpy(writer.m_mod_name,sizeof(writer.m_mod_name),name);
        }
        //-----------------------------------------------------
        //�ײ�����ӿ�,�����
        writer_t writer;
        //�﷨��,����writer.begin()ʹ��
        writer_t& operator()(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1){return writer.begin(type,tag);}
        //-----------------------------------------------------
        //���������Ϣ
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
        //���������Ϣ
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
        //�����ͨ��Ϣ
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
        //�����������Ϣ
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
        //���������Ϣ
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
        //���������Ϣ�ĵײ��ʽ������
        void print(logger_level_t type,uint32_t tag,const char* fmt,va_list ap)
        {//��Ҳ����־��д������ȷ�÷�ʾ��,������begin(),�����end()
            writer.begin(type,tag).vfmt(fmt,ap).end();
        }
    };

    //-----------------------------------------------------
    //��־������ӿ�,�������ڿ���̨���/�ļ����/�������/���紫��ȵ�Ŀ��
    //���ݾ����ʵ��,���Կ��̱߳������־��¼������ʹ��.
    class logger_writer_i
    {
    public:
        virtual void on_begin(uint64_t tex,logger_level_t type,uint32_t tag,const char* msg,uint32_t msgsize)=0;
        virtual void on_write(uint64_t tex,const void* data,uint32_t size)=0;
        virtual void on_end(uint64_t tex)=0;
        virtual ~logger_writer_i(){}
    };
    //ÿ����־��¼���������������������
    const uint32_t max_logger_writer_count=4;

    //-----------------------------------------------------
    //��־��¼��,���԰󶨶����־�����,�ɿ��̱߳������־��¼���ӿ�����
    template<class LT=null_lock_t>
    class logger_t:public logger_i
    {
        logger_writer_i     *m_writers[max_logger_writer_count];
        uint32_t            m_writer_count;                 //�󶨹������������
        logger_level_t      m_can_level;                    //�����������־����,>=m_can_level�ſ������,Ĭ��Ϊ���ȫ��
        uint32_t            m_pid;
        LT                  m_locker;
        //-------------------------------------------------
        //fmt��ʽ����Ҫ�ĵײ������
        class fmt_follower_logger:public fmt_imp::fmt_follower_null<char>
        {
            typedef fmt_imp::fmt_follower_null<char> super_t;
            logger_t        *parent;
            char            m_buff[512];                    //�ڲ����е���ʱ������
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
            {//��ʽ������������ݴ�һ��,�������ַ�������ײ�
                super_t::buffer[super_t::idx++] = character;
                ++super_t::count;
                if (super_t::idx >= super_t::maxlen)
                {
                    //ѭ����������а󶨵������
                    for(uint32_t i=0;i<parent->m_writer_count;++i)
                        parent->m_writers[i]->on_write(m_last_tex,super_t::buffer,(uint32_t)super_t::maxlen);
                    super_t::idx=0;
                }
            }
            //-----------------------------------------
            void end()
            {//����ʣ��ĸ�ʽ�������ȫ��������ײ�
                if (super_t::idx)
                {
                    //ѭ����������а󶨵������
                    for(uint32_t i=0;i<parent->m_writer_count;++i)
                        parent->m_writers[i]->on_write(m_last_tex,super_t::buffer,(uint32_t)super_t::idx);
                    super_t::idx=0;
                }
            }
        };
        //-----------------------------------------------------
        //�ж��Ƿ�������ǰ�������־����
        virtual bool on_can_write(logger_level_t type)
        {
            return (type>=m_can_level)&&m_writer_count;
        }
        //-----------------------------------------------------
        //��ʼһ����־����Ĵ���
        virtual void on_begin(logger_level_t type,uint32_t tag,uint64_t tex,const char* modname)
        {
            rx_assert(tex!=0);

            m_locker.lock();

            tiny_string_t<char,512> scat;

            //ƴװ��ǰʱ��/��־����/pid/tid
            char dt[24];
            rx_datetime2iso(dt,"%u-%02u-%02uT%02u:%02u:%02u.%03u",true);
            scat("[%s|",dt) ("%s|",logger_level_name(type)) ("PID:%4u|",m_pid) ("TID:%4u]",get_tid());

            //���������Ч��mod���ƺ�tag���
            if (!is_empty(modname))
                scat("[%s]",modname);
            if (tag!=(uint32_t)-1)
                scat("[TAG:%6u]",tag);
            scat("[TEX:%016zx]",tex);
            scat<<' ';

            //ѭ����������а󶨵������
            for(uint32_t i=0;i<m_writer_count;++i)
                m_writers[i]->on_begin(tex,type,tag,scat.c_str(),scat.size());
        }
        //-----------------------------------------------------
        //�ڵ�ǰ��־�����������ʽ��ƴװ����
        virtual void on_vfmt(const char* fmt,va_list ap,uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;
            fmt_follower_logger fbuf(this,tex);
            fmt_imp::fmt_core(fbuf, fmt, ap);
        }
        //-----------------------------------------------------
        //�ڵ�ǰ��־���������HEX��������
        virtual void on_hex(const void* data,uint32_t size,uint32_t pre_tab,uint32_t line_bytes,uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;

            char line_buff[256*4];                      //��ǰ�����������
            line_bytes=min(line_bytes,(uint32_t)256);   //ÿ���������ֽ�����
            pre_tab=min(pre_tab,(uint32_t)64);          //ÿ�����ǰ׺�ո�����
            const uint8_t *bytes=(const uint8_t*)data;  //������Ļ�����ָ��
            uint32_t remain=size;                       //ʣ�����ݳ���
            while(remain)
            {
                uint32_t blocks=min(line_bytes,remain); //���д�������ֽڳ���
                uint32_t sl=0;
                for(;sl<pre_tab;++sl)
                    line_buff[sl]=' ';                  //��ǰ�����ǰ׺

                for(uint32_t t=0;t<blocks;++t)
                {//ѭ�������ǰ������Ϊhex
                    st::hex2(*bytes,line_buff+sl);
                    ++bytes;
                    sl+=2;
                    line_buff[sl++]=' ';
                }
                remain-=blocks;
                if (remain)                             //����������,��ô������������Ͳ��ϻس����кͽ�����
                {
                    line_buff[sl++]='\n';
                    line_buff[sl]='0';
                }
                //ѭ����������а󶨵������
                for(uint32_t i=0;i<m_writer_count;++i)
                    m_writers[i]->on_write(tex,line_buff,sl);
            }
        }
        //-----------------------------------------------------
        //�ڵ�ǰ��־���������ԭʼ��������������
        virtual void on_bin(const void* data,uint32_t size,uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;

            //ѭ����������а󶨵������
            for(uint32_t i=0;i<m_writer_count;++i)
                m_writers[i]->on_write(tex,data,size);
        }
        //-----------------------------------------------------
        //����һ����־����
        virtual void on_end(uint64_t tex)
        {
            if (tex==0||m_writer_count==0)
                return;

            //ѭ��֪ͨ�����а󶨵������
            for(uint32_t i=0;i<m_writer_count;++i)
                m_writers[i]->on_end(tex);

            m_locker.unlock();
        }
        //�������ظ��ӿ��е�ת���󶨹��ܽӿ�
        void bind(logger_i log){}
    public:
        logger_t():m_writer_count(0),m_can_level(LT_LEVEL_DEBUG)
        {
            m_pid=(uint32_t)get_pid();
        }
        //-----------------------------------------------------
        //��������ӿ�
        bool bind(logger_writer_i& w)
        {
            if (m_writer_count>=max_logger_writer_count)
                return false;
            m_writers[m_writer_count++]=&w;
            return true;
        }
        //-----------------------------------------------------
        //����������������,0��ȫ��ֹ���
        void level(logger_level_t lvl){m_can_level=lvl;}
        //-----------------------------------------------------
        uint32_t writers(){return m_writer_count;}

    };

    //-----------------------------------------------------
    //��־�����:����̨���
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
    //��־�����:�ļ����
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
    //��ȫ��ʹ��,������ļ��Ϳ���̨����־��¼��(��ʵ��)
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

    //��ȫ��ʹ��,������ļ�����־��¼��(ͨ�����ֲ�ͬ��seq����ʹ�ö��ʵ��)
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

    //��ȫ��ʹ��,���������̨����־��¼��(��ʵ��)
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

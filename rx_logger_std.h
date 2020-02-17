#ifndef _RX_LOGGER_STD_H_
#define _RX_LOGGER_STD_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"

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

    //-----------------------------------------------------
    //��־������ӿ�,�������ڿ���̨���/�ļ����/�������/���紫��ȵ�Ŀ��
    //���ݾ����ʵ��,���Կ��̱߳������־��¼������ʹ��.
    class logger_writer_i
    {
    public:
        virtual void on_begin(uint64_t tex,logger_level_t type,uint32_t tag,const char* msg,uint32_t msgsize)=0;
        virtual void on_write(uint64_t tex,const void* msg,uint32_t msgsize)=0;
        virtual void on_end(uint64_t tex,const void* msg,uint32_t msgsize)=0;
        virtual ~logger_writer_i(){}
    };
    //ÿ����־��¼���������������������
    const uint32_t max_logger_writer_count=5;

    //-----------------------------------------------------
    //��־��¼��,���԰󶨶����־�����,���ܿ��߳�ʹ��
    class logger_t
    {
        logger_writer_i     *m_writers[max_logger_writer_count];
        uint32_t            m_writer_count;                 //�󶨹������������
        logger_level_t      m_can_level;                    //�����������־����,>=m_can_level�ſ������,Ĭ��Ϊ���ȫ��
        const char*         m_mod_name;                     //��־��¼�������Ĺ���ģ��
        //-------------------------------------------------
        //�ڲ�ʹ�õ���־�����
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
        //�ײ�����ӿ�,�����
        writer_t writer;
        //-------------------------------------------------
        //��������ӿ�
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
        //����������������,0��ȫ��ֹ���
        void level(logger_level_t lvl){m_can_level=lvl;}
    };

}


#endif

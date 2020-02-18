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
        virtual void on_write(uint64_t tex,const void* data,uint32_t size)=0;
        virtual void on_end(uint64_t tex)=0;
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
        uint32_t            m_pid;
        //-------------------------------------------------
        //�ڲ�ʹ�õ���־�����
        class writer_t
        {
            friend logger_t;
            logger_t        *parent;                        //������ָ��
            uint32_t        m_last_seq;                     //������־���
            uint64_t        m_last_tex;                     //������־�����,this<<32|seq,��֪��־��¼��ÿ�ε�Ψһ����

            //---------------------------------------------
            //fmt��ʽ����Ҫ�ĵײ������
            class fmt_follower_logger:public fmt_imp::fmt_follower_null<char>
            {
                typedef fmt_imp::fmt_follower_null<char> super_t;
                writer_t        *owner;
                char            m_buff[512];                //�ڲ����е���ʱ������
            public:
                //-----------------------------------------
                fmt_follower_logger(writer_t* o)
                {
                    owner=o;
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
                        for(uint32_t i=0;i<owner->parent->m_writer_count;++i)
                            owner->parent->m_writers[i]->on_write(owner->m_last_tex,super_t::buffer,(uint32_t)super_t::maxlen);
                        super_t::idx=0;
                    }
                }
                //-----------------------------------------
                void end()
                {//����ʣ��ĸ�ʽ�������ȫ��������ײ�
                    if (super_t::idx)
                    {
                        //ѭ����������а󶨵������
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
            //�����־��ǰ�˹̶�����
            writer_t& begin(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1)
            {
                rx_assert(parent!=NULL);
                end();
                if (type<parent->m_can_level)
                    return *this;
                rx_assert(m_last_tex==0);
                //���챾���������
                m_last_tex=(size_t)this;
                m_last_tex<<=32;
                m_last_tex|=++m_last_seq;

                tiny_string_t<char,512> scat;

                //ƴװ��ǰʱ��/��־����/pid/tid
                char buff[512];
                rx_iso_datetime(time(NULL),buff);
                scat("[%s|",buff) ("%s|",logger_level_name(type)) ("PID:%6u|",parent->m_pid) ("TID:%6u]",get_tid());

                //���������Ч��mod���ƺ�tag���
                if (!is_empty(parent->m_mod_name))
                    scat("[%s]",tag);
                if (tag!=(uint32_t)-1)
                    scat("[TAG:%6u]",tag);

                //ѭ����������а󶨵������
                for(uint32_t i=0;i<parent->m_writer_count;++i)
                    parent->m_writers[i]->on_begin(m_last_tex,type,tag,buff,scat.size());

                return *this;
            }
            //---------------------------------------------
            //��ʽ�������־�ľ������ݲ���
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
            //��ʮ�������ַ�����ʽ������ݲ���:���ݻ�����ָ��,���ݳ���,ǰ׺�ո�����(���64),ÿ���ֽ�����(���256)
            writer_t& hex(const void* data,uint32_t size,uint32_t pre_tab=4,uint32_t line_bytes=32)
            {
                rx_assert(m_last_tex!=0);

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
                        st::hex2(bytes[t],line_buff+sl);
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
                    for(uint32_t i=0;i<parent->m_writer_count;++i)
                        parent->m_writers[i]->on_write(m_last_tex,line_buff,sl);
                }

                return *this;
            }
            //---------------------------------------------
            //��ԭʼ�����Ʒ�ʽ������ݲ���
            writer_t& bin(const void* data,uint32_t size)
            {
                rx_assert(m_last_tex!=0);
                //ѭ����������а󶨵������
                for(uint32_t i=0;i<parent->m_writer_count;++i)
                    parent->m_writers[i]->on_write(m_last_tex,data,size);
                return *this;
            }
            //---------------------------------------------
            //��֪������־���������
            void end()
            {
                if (m_last_tex==0)
                    return;
                //ѭ��֪ͨ�����а󶨵������
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
        //�ײ�����ӿ�,�����
        writer_t writer;
        writer_t& operator()(logger_level_t type=LT_LEVEL_INFO,uint32_t tag=-1){return writer.begin(type,tag);}
        //-------------------------------------------------
        //��������ӿ�
        bool bind(logger_writer_i& w)
        {
            if (m_writer_count>=max_logger_writer_count)
                return false;
            m_writers[m_writer_count++]=&w;
        }
        //-------------------------------------------------
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
        //-------------------------------------------------
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
        //-------------------------------------------------
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
        //-------------------------------------------------
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
        //-------------------------------------------------
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
        //---------------------------------------------
        //���������Ϣ�ĵײ��ʽ������
        void print(logger_level_t type,uint32_t tag,const char* fmt,va_list ap)
        {
            if (type<m_can_level)
                return;
            writer.begin(type,tag).vfmt(fmt,ap).end();
        }
        //-------------------------------------------------
        //����������������,0��ȫ��ֹ���
        void level(logger_level_t lvl){m_can_level=lvl;}
    };
}


#endif

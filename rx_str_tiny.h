#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"

namespace rx
{
#pragma pack(push,1)
    //-----------------------------------------------------
    //��װһ�����׵��ַ�������,����dtl�����ڲ�����ʱkey�ַ����洢
    template<class CT=char,uint16_t max_str_size=0>
    struct tiny_string_t
    {
    protected:
        //-------------------------------------------------
        //���������õĴ�ģʽ
        template<class ct,uint16_t buff_size>
        class head_t
        {
        public:
            head_t() :length(0) { buff[0] = 0; }
            //���û�����,�����.�����ڽӿڼ���.
            void bind(ct* buff, uint16_t size) { rx_alert("don't call this method."); }
            //��ȡ���û�����������
            const uint16_t capacity()const{return buff_size;}

            uint16_t    length;                             //��¼buff��ʵ�ʳ���.
            ct          buff[buff_size];                    //�ַ���������ָ��.
        };

        //-------------------------------------------------
        //�ػ�,ʹ�����ð󶨵Ļ�����
        template<class ct>
        class head_t<ct,0>
        {
        public:
            head_t():max_size(0),length(0),buff(NULL){}
            //�󶨻�����ָ��������
            void bind(ct* buf, uint16_t size) { buff = buf; max_size = size; length = 0; }
            //��֪�󶨵Ļ�����������
            const uint16_t capacity()const{return max_size;}

            uint16_t    max_size;                           //�����֪buff�Ŀ�������
            uint16_t    length;                             //��¼buff��ʵ�ʳ���.
            ct         *buff;                               //�ַ���������ָ��.
        };

        //-------------------------------------------------
        head_t<CT,max_str_size>  m_head;                    //����ʹ�õ�С������������
    private:
        //-------------------------------------------------
        //�����ø�ֵ�����,Ĭ�ϵ�ʱ��ֵ�ͽ���m_head��Ӳ����:ָ���ָ��;���������.
        //tiny_string_t& operator=(const tiny_string_t&);
        tiny_string_t(const tiny_string_t&);
        //-------------------------------------------------
    public:
        tiny_string_t() {}
        //�󶨻�����,����֪�������ڲ���������ƫ��
        tiny_string_t(uint32_t cap, CT* buff, uint32_t init_offset = 0) { m_head.bind(buff, cap); m_head.length = init_offset; }
        //�������û�����ģʽ,����ֱ�ӽ��д���ֵ
        tiny_string_t(const CT* str,uint32_t len=0)
        {
            rx_static_assert(max_str_size!=0);              //Ҫ����������û�����ģʽ,�ſ���ֱ�Ӹ�ֵ
            assign(str,len);
        }
        //-------------------------------------------------
        //ʹ�ø������ַ������и�ֵ(�ܷŶ��ٷŶ���)
        //����ֵ:���������Ĵ��ߴ�.
        uint32_t assign(const CT* str=NULL, uint32_t len = 0)
        {
            if (is_empty(str)|| m_head.capacity()==1)
            {
                m_head.length = 0;
                m_head.buff[0] = 0;
                return 0;
            }
            else
            {
                //�����ַ����Ŀ���
                if (!len)
                    len = (uint32_t)st::strlen(str);
                m_head.length = Min(len, uint32_t(m_head.capacity() - 1));
                st::strncpy(m_head.buff, str, m_head.length);
                m_head.buff[m_head.length] = 0;
                return m_head.length;
            }
        }
        //-------------------------------------------------
        //��ʽ�������ڲ���
        bool fmt(const CT *str, va_list arg)
        {
            m_head.length = 0;
            m_head.buff[0] = 0;

            int rc=st::vsnprintf(m_head.buff, m_head.capacity(), str, arg);
            if (rc < 0 || rc >= m_head.capacity())
                return false;
            m_head.length = rc;
            return true;
        }
        bool fmt(const CT *str,...)
        {
            va_list	ap;
            va_start(ap, str);
            return fmt(str, ap);
        }
        //-------------------------------------------------
        //�󶨻������������ַ����ĸ�ֵ
        uint32_t bind(CT* buff,uint32_t cap,const CT* str,uint32_t len = 0)
        {
            rx_static_assert(max_str_size == 0);            //Ҫ�����Ϊ��������ģʽ,�ſ������°�
            m_head.bind(buff,cap);
            return assign(str,len);
        }
        //-------------------------------------------------
        //��֪����������
        uint32_t capacity()const { return m_head.capacity(); }
        //��֪�����������ݳ���,�����������,��˵��������ƴװ�������
        uint32_t size()const { return m_head.length; }
        //��֪����������
        const CT* c_str() const { return m_head.buff; }
        operator const CT* ()const {return m_head.buff;}
        //-------------------------------------------------
        //�Ƚ����������
        bool operator <  (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) < 0;}
        bool operator <= (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) <= 0;}
        bool operator == (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) == 0;}
        bool operator >  (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) > 0;}
        bool operator >= (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) >= 0;}
        bool operator != (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) != 0;}
        //-------------------------------------------------
        bool operator <  (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) < 0;}
        bool operator <= (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) <= 0;}
        bool operator == (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) == 0;}
        bool operator >  (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) > 0;}
        bool operator >= (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) >= 0;}
        bool operator != (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) != 0;}
        //-------------------------------------------------
        //�ڸ�����buff�ڴ���Ϲ�����״����󲢽��г�ʼ��
        //����ֵ:NULLʧ��;����Ϊ��ͷ����ָ��
        static tiny_string_t* make(void* buff, uint32_t buffsize, const CT* str, uint32_t len = 0)
        {
            if (buffsize <= sizeof(tiny_string_t))       //�����С�ߴ�
                return NULL;

            //�ڸ����Ļ������Ϲ�������ַ�������
            uint32_t cap = buffsize - sizeof(tiny_string_t);
            CT *strbuf = (CT*)((uint8_t*)buff + sizeof(tiny_string_t));
            tiny_string_t *s = ct::OC<tiny_string_t >((tiny_string_t*)buff, cap, strbuf);
            s->assign(str, len);
            return s;
        }
        //-------------------------------------------------
        //ƴװ�ַ�
        tiny_string_t& operator<<(const CT c)
        {
            if (m_head.length < m_head.capacity() - 1)
            {
                m_head.buff[m_head.length++] = c;
                m_head.buff[m_head.length] = 0;
            }
            else
                m_head.length = m_head.capacity();          //��������,��Ǵ���

            return *this;
        }
        //-------------------------------------------------
        //ƴװ�ַ���
        tiny_string_t& operator<<(const CT *str)
        {
            //������ʣ��Ŀռ��з���ָ���ַ���
            uint32_t rc = st::strcpy(m_head.buff + m_head.length, m_head.capacity() - m_head.length, str);
            if (rc)
                m_head.length += rc;
            else
                m_head.length = m_head.capacity();          //��������,��Ǵ���
            return *this;
        }
        //-------------------------------------------------
        //ƴװ�����ַ���
        tiny_string_t& operator()(const CT *str, uint32_t len)
        {
            //������ʣ��Ŀռ��з���ָ���ַ���
            uint32_t rc = st::strcpy(m_head.buff + m_head.length, m_head.capacity() - m_head.length, str, len);
            if (rc)
                m_head.length += rc;
            else
                m_head.length = m_head.capacity();          //��������,��Ǵ���
            return *this;
        }
        //-------------------------------------------------
        //�ַ���������ֵ
        tiny_string_t& operator=(const CT *str) { assign(str); return *this; }
    };
#pragma pack(pop)

}


#endif

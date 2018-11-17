#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util.h"

namespace rx
{
#pragma pack(push,1)
    //-----------------------------------------------------
    //��װһ�����׵��ַ�������,����dtl�����ڲ�����ʱkey�ַ����洢
    template<class CT=char,uint16_t max_str_size=0>
    struct tiny_string_t
    {
        //-------------------------------------------------
        template<class ct,uint16_t buff_size>
        class head_t                                        //���������õ�С��
        {
        public:
            head_t():length(0){}
            //���û�����,�����.�����ڽӿڼ���.
            void bind(ct* buff, uint16_t size) { rx_alert("don't call this method."); }
            //��ȡ���û�����������
            const uint16_t capacity()const{return buff_size;}

            uint16_t    length;                             //��¼buff��ʵ�ʳ���.
            ct          buff[buff_size];                    //�ַ���������ָ��.
        };

        //-------------------------------------------------
        template<class ct>
        class head_t<ct,0>                                  //�ػ�,ʹ�����ð󶨵Ļ�����
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
        tiny_string_t(uint32_t cap, CT* buff){m_head.bind(buff,cap);}
        tiny_string_t(){}
        tiny_string_t(const CT* str){set(str);}
        //-------------------------------------------------
        //ʹ�ø������ַ������и�ֵ
        //����ֵ:���������Ĵ��ߴ�.
        uint32_t set(const CT* str, uint32_t len = 0)
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
        //�󶨻������������ַ����ĸ�ֵ
        uint32_t bind(CT* buff,uint32_t cap,const CT* str,uint32_t len = 0){m_head.bind(buff,cap);return set(str,len);}
        //-------------------------------------------------
        //��֪����������
        uint32_t capacity()const { return m_head.capacity(); }
        //��֪�����������ݳ���
        uint32_t size()const { return m_head.length; }
        //��֪����������
        const CT* c_str() const { return m_head.buff; }
        operator const CT* ()const {return m_head.buff;}
        //-------------------------------------------------
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
    };
#pragma pack(pop)

    //-------------------------------------------------------
    //char���͵�С����
    typedef tiny_string_t<char> tiny_string_head_ct;
    //wchar_t���͵�С����
    typedef tiny_string_t<wchar_t> tiny_string_head_wt;

    ////-----------------------------------------------------
    //�ڸ�����buff�ڴ���Ϲ�����״����󲢽��г�ʼ��
    //����ֵ:NULLʧ��;����Ϊ��ͷ����ָ��
    template<class CT>
    tiny_string_t<CT>* make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string_t<CT> string_t;
        if (buffsize <= sizeof(string_t))       //�����С�ߴ�
            return NULL;

        //�ڸ����Ļ������Ϲ�������ַ�������
        uint32_t cap = buffsize - sizeof(string_t);
        CT *strbuf=(CT*)((uint8_t*)buff+sizeof(string_t));
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,strbuf);
        s->set(str,len);
        return s;
    }
}


#endif

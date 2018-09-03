#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util.h"

namespace rx
{
    //-----------------------------------------------------
    //��װһ�����׵��ַ�������,����dtl�����ڲ�����ʱkey�ַ����洢
    //!!!����ע��!!!:�ڱ�����ʵ���ĺ�,�����㹻���ȵ�m_capacity�ռ�.
    template<class CT=char>
    struct tiny_string_head_t
    {
    private:
        //-------------------------------------------------
        tiny_string_head_t& operator=(const tiny_string_head_t&);
        tiny_string_head_t(const tiny_string_head_t&);
        //-------------------------------------------------
        uint32_t    m_capacity;                             //�����֪m_string�Ŀ�������
        uint32_t    m_length;                               //��¼m_string��ʵ�ʳ���.
        CT         *m_string;                               //�ַ���������ָ��.
    public:
        tiny_string_head_t(uint32_t cap, CT* buff) :m_capacity(cap),m_length(0),m_string(buff){}
        //-------------------------------------------------
        //ʹ�ø������ַ������и�ֵ
        //����ֵ:���������Ĵ��ߴ�.
        uint32_t set(const CT* str, uint32_t len = 0)
        {
            if (is_empty(str)|| m_capacity==1)
            {
                m_length = 0;
                m_string[0] = 0;
                return 0;
            }
            else
            {
                //�����ַ����Ŀ���
                if (!len)
                    len = (uint32_t)st::strlen(str);
                m_length = Min(len, uint32_t(m_capacity - 1));
                st::strncpy(m_string, str, m_length);
                m_string[m_length] = 0;
                return m_length;
            }
        }
        //-------------------------------------------------
        uint32_t capacity()const { return m_capacity; }
        uint32_t length()const { return m_length; }
        const CT* c_str() const { return m_string; }
        operator const CT* ()const {return m_string;}
        //-------------------------------------------------
        bool operator <  (const tiny_string_head_t& str) const {return strcmp(m_string, str.m_string) < 0;}
        bool operator <= (const tiny_string_head_t& str) const {return strcmp(m_string, str.m_string) <= 0;}
        bool operator == (const tiny_string_head_t& str) const {return strcmp(m_string, str.m_string) == 0;}
        bool operator >  (const tiny_string_head_t& str) const {return strcmp(m_string, str.m_string) > 0;}
        bool operator >= (const tiny_string_head_t& str) const {return strcmp(m_string, str.m_string) >= 0;}
        bool operator != (const tiny_string_head_t& str) const {return strcmp(m_string, str.m_string) != 0;}
        //-------------------------------------------------
        bool operator <  (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) < 0;}
        bool operator <= (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) <= 0;}
        bool operator == (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) == 0;}
        bool operator >  (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) > 0;}
        bool operator >= (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) >= 0;}
        bool operator != (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) != 0;}
        //-------------------------------------------------
    };

    typedef tiny_string_head_t<char> tiny_string_head_ct;
    typedef tiny_string_head_t<wchar_t> tiny_string_head_wt;

    ////-----------------------------------------------------
    //�ڸ�����buff�ڴ���Ϲ�����״����󲢽��г�ʼ��
    //����ֵ:NULLʧ��;����Ϊ��ͷ����ָ��
    template<class CT>
    tiny_string_head_t<CT>* make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string_head_t<CT> string_t;
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

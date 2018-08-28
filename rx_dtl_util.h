#ifndef _RX_DTL_UTIL_H_
#define _RX_DTL_UTIL_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
#if RX_CC == RX_CC_VC
    #pragma warning(disable:4200)
#endif
    //-----------------------------------------------------
    //��װһ�����׵��ַ�������,����dtl�����ڲ�����ʱkey�ַ����洢
    template<class CT=char>
    struct tiny_string
    {
    private:
        tiny_string& operator=(const tiny_string&);
        tiny_string(const tiny_string&);
        uint16_t    m_capacity;
        uint16_t    m_length;
        CT          m_string[];
    public:
        //-------------------------------------------------
        tiny_string(uint16_t cap, const CT* str, uint32_t len = 0) :m_capacity(cap) { set(str,len); }
        //-------------------------------------------------
        uint16_t set(const CT* str, uint32_t len = 0)
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
                if (!len) len = strlen(str);
                m_length = Min(len, uint32_t(m_capacity - 1));
                strncpy(m_string, str, m_length);
                m_string[m_length] = 0;
                return m_length;
            }
        }
        //-------------------------------------------------
        uint16_t length()const { return m_length; }
        const CT* c_str() const { return m_string(); }
        //-------------------------------------------------
        bool operator < (tiny_string& str)
        {
            return strcmp(m_string, str.m_string) < 0;
        }
        bool operator <= (tiny_string& str)
        {
            return strcmp(m_string, str.m_string) <= 0;
        }
        bool operator == (tiny_string& str)
        {
            return strcmp(m_string, str.m_string) == 0;
        }
        bool operator > (tiny_string& str)
        {
            return strcmp(m_string, str.m_string) > 0;
        }
        bool operator >= (tiny_string& str)
        {
            return strcmp(m_string, str.m_string) >= 0;
        }
        bool operator != (tiny_string& str)
        {
            return strcmp(m_string, str.m_string) != 0;
        }
    };
#if RX_CC == RX_CC_VC
    #pragma warning(default:4200)
#endif

    typedef tiny_string<char> tiny_stringc;
    typedef tiny_string<wchar_t> tiny_stringw;

    ////-----------------------------------------------------
    //�ڸ�����buff�ڴ���Ϲ�����״����󲢽��г�ʼ��
    //����ֵ:0�ڴ���С,����ʧ��;>0Ϊʵ�ʳ�ʼ��������str����
    template<class CT>
    uint32_t make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string<CT> string_t;
        if (buffsize <= sizeof(string_t))       //�����С�ߴ�
            return 0;

        //�ڸ����Ļ������Ϲ�������ַ�������
        uint32_t cap = buffsize - sizeof(string_t);
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,str,len);
        return s->length();
    }
}


#endif
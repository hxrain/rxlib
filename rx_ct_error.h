#ifndef _RX_CT_ERROR_H_
#define _RX_CT_ERROR_H_

#include "rx_cc_macro.h"
#include <string>
//---------------------------------------------------------
//��ȡϵͳ�������
#if RX_OS_WIN
namespace rx
{
    //��ȡϵͳ�������
    inline uint32_t os_error_code() { return GetLastError(); }
    //��ϵͳ��������ʽ��Ϊ�ɶ���Ϣ��
    inline std::string os_error_reason(uint32_t os_errno=0)
    {
        if (!os_errno)
            os_errno=GetLastError();

        if (!os_errno)
            return "";

        char buff[256];
        buff[0]=0;

        DWORD rc=FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,NULL,os_errno,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),buff,(DWORD)sizeof(buff),NULL);
        return rc?buff:"";
    }
    inline bool os_error_reason(std::string &msg,uint32_t os_errno=0)
    {
        if (!os_errno)
            os_errno=GetLastError();
        
        msg.clear();
        if (!os_errno)
            return true;

        char buff[256];
        buff[0]=0;

        DWORD rc=FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,NULL,os_errno,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),buff,(DWORD)sizeof(buff),NULL);
        if (!rc) return false;
        msg=buff;
        return true;
    }
}
#else
#include <errno.h>
namespace rx
{
    inline uint32_t os_error_code() { return errno; }
    inline std::string os_error_reason(uint32_t os_errno=0)
    {
        if (!os_errno)
            os_errno=errno;
        char* str=strerror(os_errno);
        return str?str:"";
    }
    inline bool os_error_reason(std::string &msg,uint32_t os_errno=0)
    {
        if (!os_errno)
            os_errno=GetLastError();

        msg.clear();
        if (!os_errno)
            return true;

        char* str=strerror(os_errno);
        if (str==NULL)
            return false;

        msg=str;
        return true;
    }
}
#endif

namespace rx
{
    //-----------------------------------------------------
    //����ͳһ�Ĵ�������ؽṹ��
    typedef struct error_code
    {
        union
        {
            struct
            {
                uint16_t    e_reason;                      //���ӵĴ���ԭ�����
                uint16_t    e_code;                        //������
            };
            uint32_t        value;                         //����ʹ�õ�ͳһ�Ĵ���������ֵ
        };
        //�����������������,�����ж��Ƿ��д�.
        operator bool() { return e_code != 0; }              //�ṹ���ʵ������ֱ�ӽ����߼��Ƚ�
        
        bool operator == (const uint16_t ec) { return e_code == ec; }

        //���캯��
        error_code():value(0){}
        error_code(const error_code& ec):value(ec.value){}
        error_code(const uint16_t code,const uint16_t reason=0):e_reason(reason),e_code(code){}
        //���ô���ԭ���벢��������
        error_code reason(const uint16_t reason){e_reason=reason;return value;}
    }error_code;


    //-----------------------------------------------------
    //����rx��ͳһʹ�õĴ�����������
    #define desc_errcode(classify,no) ((classify)<<8|no)

    //������:û�д���,�������.
    const uint16_t ec_ok                    = 0;

    //-----------------------------------------------------
    const uint8_t err_cls_mem               = 1;            //�ڴ�����������
    const uint8_t err_cls_param             = 2;            //��δ������
    const uint8_t err_cls_os                = 3;            //����ϵͳ�������
    const uint8_t err_cls_file              = 4;            //�ļ�ϵͳ�������
    const uint8_t err_cls_net               = 5;            //������ʴ������
    const uint8_t err_cls_db                = 6;            //���ݿ���ʴ������
    const uint8_t err_cls_limit             = 7;            //Υ�����ƴ������


    //-----------------------------------------------------
    //������:�ڴ治��,�޷������ڴ�.
    const uint16_t ec_mem_not_enough        = desc_errcode(err_cls_mem, 1);
    //������:���治��,�ռ��С.
    const uint16_t ec_buf_not_enough        = desc_errcode(err_cls_mem, 2);

    //-----------------------------------------------------
    //��δ���
    const uint16_t ec_in_param              = desc_errcode(err_cls_param, 1);

    //-----------------------------------------------------
    //�ļ�������
    const uint16_t ec_file_not_exists       = desc_errcode(err_cls_file, 1);
    //�ļ��򿪴���(�򿪴���/��������/Ȩ�޲���...)
    const uint16_t ec_file_open             = desc_errcode(err_cls_file, 2);
    //�ļ�ɾ������
    const uint16_t ec_file_remove           = desc_errcode(err_cls_file, 3);
    //�ļ���ȡ����
    const uint16_t ec_file_read             = desc_errcode(err_cls_file, 4);
    //�ļ�д�����(д��/׷��/Ȩ�޲���...)
    const uint16_t ec_file_write            = desc_errcode(err_cls_file, 5);
    //�ļ���������
    const uint16_t ec_file_op               = desc_errcode(err_cls_file, 6);

    //-----------------------------------------------------
    //����socket��������
    const uint16_t ec_net_create            = desc_errcode(err_cls_net, 1);
    //����socket��ַ�󶨴���
    const uint16_t ec_net_bind              = desc_errcode(err_cls_net, 2);
    //����socket������������
    const uint16_t ec_net_listen            = desc_errcode(err_cls_net, 3);
    //�������ӳ�ʱ
    const uint16_t ec_net_conn_timeout      = desc_errcode(err_cls_net, 4);
    //�����д��ʱ
    const uint16_t ec_net_rw_timeout        = desc_errcode(err_cls_net, 5);
    //���������ʱ
    const uint16_t ec_net_timeout           = desc_errcode(err_cls_net, 6);
    //�����ȡ����
    const uint16_t ec_net_read              = desc_errcode(err_cls_net, 7);
    //����д�����
    const uint16_t ec_net_write             = desc_errcode(err_cls_net, 8);
    //������������
    const uint16_t ec_net_op                = desc_errcode(err_cls_net, 9);

    //-----------------------------------------------------
    //os����:mmap�򿪴���
    const uint16_t ec_os_mmap_open          = desc_errcode(err_cls_os, 1);
    //os����:mmapӳ�����
    const uint16_t ec_os_mmap_bind          = desc_errcode(err_cls_os, 2);
    //os����:�̴߳�������
    const uint16_t ec_os_thread_create      = desc_errcode(err_cls_os, 3);
    //os����:mutex��������
    const uint16_t ec_os_mutex_create       = desc_errcode(err_cls_os, 4);
    //os����:cond��������
    const uint16_t ec_os_cond_create        = desc_errcode(err_cls_os, 5);
    //os����:sem��������
    const uint16_t ec_os_sem_create         = desc_errcode(err_cls_os, 6);

    //-----------------------------------------------------
    //��������:�ظ�����
    const uint16_t ec_limit_double_op       = desc_errcode(err_cls_limit, 1);
    //��������:���ݻ��߼�״̬����
    const uint16_t ec_limit_data            = desc_errcode(err_cls_limit, 2);
}



#endif
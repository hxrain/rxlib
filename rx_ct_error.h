#ifndef _RX_CT_ERROR_H_
#define _RX_CT_ERROR_H_

#include "rx_cc_macro.h"
#include <string>
//---------------------------------------------------------
//获取系统错误代码
#if RX_OS_WIN
namespace rx
{
    //获取系统错误代码
    inline uint32_t os_error_code() { return GetLastError(); }
    //将系统错误代码格式化为可读信息串
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
    //定义统一的错误码承载结构体
    typedef struct error_code
    {
        union
        {
            struct
            {
                uint16_t    e_reason;                      //附加的错误原因参数
                uint16_t    e_code;                        //错误码
            };
            uint32_t        value;                         //便于使用的统一的错误码整数值
        };
        //布尔类型运算符重载,便于判断是否有错.
        operator bool() { return e_code != 0; }              //结构体的实例可以直接进行逻辑比较
        
        bool operator == (const uint16_t ec) { return e_code == ec; }

        //构造函数
        error_code():value(0){}
        error_code(const error_code& ec):value(ec.value){}
        error_code(const uint16_t code,const uint16_t reason=0):e_reason(reason),e_code(code){}
        //设置错误原因码并返回自身
        error_code reason(const uint16_t reason){e_reason=reason;return value;}
    }error_code;


    //-----------------------------------------------------
    //定义rx库统一使用的错误分类与代码
    #define desc_errcode(classify,no) ((classify)<<8|no)

    //错误码:没有错误,正常完成.
    const uint16_t ec_ok                    = 0;

    //-----------------------------------------------------
    const uint8_t err_cls_mem               = 1;            //内存操作错误分类
    const uint8_t err_cls_param             = 2;            //入参错误分类
    const uint8_t err_cls_os                = 3;            //操作系统错误分类
    const uint8_t err_cls_file              = 4;            //文件系统错误分类
    const uint8_t err_cls_net               = 5;            //网络访问错误分类
    const uint8_t err_cls_db                = 6;            //数据库访问错误分类
    const uint8_t err_cls_limit             = 7;            //违反限制错误分类


    //-----------------------------------------------------
    //错误码:内存不足,无法分配内存.
    const uint16_t ec_mem_not_enough        = desc_errcode(err_cls_mem, 1);
    //错误码:缓存不足,空间过小.
    const uint16_t ec_buf_not_enough        = desc_errcode(err_cls_mem, 2);

    //-----------------------------------------------------
    //入参错误
    const uint16_t ec_in_param              = desc_errcode(err_cls_param, 1);

    //-----------------------------------------------------
    //文件不存在
    const uint16_t ec_file_not_exists       = desc_errcode(err_cls_file, 1);
    //文件打开错误(打开错误/创建错误/权限不足...)
    const uint16_t ec_file_open             = desc_errcode(err_cls_file, 2);
    //文件删除错误
    const uint16_t ec_file_remove           = desc_errcode(err_cls_file, 3);
    //文件读取错误
    const uint16_t ec_file_read             = desc_errcode(err_cls_file, 4);
    //文件写入错误(写入/追加/权限不足...)
    const uint16_t ec_file_write            = desc_errcode(err_cls_file, 5);
    //文件其他错误
    const uint16_t ec_file_op               = desc_errcode(err_cls_file, 6);

    //-----------------------------------------------------
    //网络socket创建错误
    const uint16_t ec_net_create            = desc_errcode(err_cls_net, 1);
    //网络socket地址绑定错误
    const uint16_t ec_net_bind              = desc_errcode(err_cls_net, 2);
    //网络socket启动监听错误
    const uint16_t ec_net_listen            = desc_errcode(err_cls_net, 3);
    //网络连接超时
    const uint16_t ec_net_conn_timeout      = desc_errcode(err_cls_net, 4);
    //网络读写超时
    const uint16_t ec_net_rw_timeout        = desc_errcode(err_cls_net, 5);
    //网络操作超时
    const uint16_t ec_net_timeout           = desc_errcode(err_cls_net, 6);
    //网络读取错误
    const uint16_t ec_net_read              = desc_errcode(err_cls_net, 7);
    //网络写入错误
    const uint16_t ec_net_write             = desc_errcode(err_cls_net, 8);
    //网络其他错误
    const uint16_t ec_net_op                = desc_errcode(err_cls_net, 9);

    //-----------------------------------------------------
    //os错误:mmap打开错误
    const uint16_t ec_os_mmap_open          = desc_errcode(err_cls_os, 1);
    //os错误:mmap映射错误
    const uint16_t ec_os_mmap_bind          = desc_errcode(err_cls_os, 2);
    //os错误:线程创建错误
    const uint16_t ec_os_thread_create      = desc_errcode(err_cls_os, 3);
    //os错误:mutex创建错误
    const uint16_t ec_os_mutex_create       = desc_errcode(err_cls_os, 4);
    //os错误:cond创建错误
    const uint16_t ec_os_cond_create        = desc_errcode(err_cls_os, 5);
    //os错误:sem创建错误
    const uint16_t ec_os_sem_create         = desc_errcode(err_cls_os, 6);

    //-----------------------------------------------------
    //功能限制:重复操作
    const uint16_t ec_limit_double_op       = desc_errcode(err_cls_limit, 1);
    //功能限制:数据或逻辑状态错误
    const uint16_t ec_limit_data            = desc_errcode(err_cls_limit, 2);
}



#endif
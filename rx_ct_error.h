#ifndef _RX_CT_ERROR_H_
#define _RX_CT_ERROR_H_

#include "rx_cc_macro.h"

namespace rx
{
    //-----------------------------------------------------
    //定义统一的错误码承载结构体
    typedef struct rx_error
    {
        union
        {
            struct
            {
                uint16_t    reason;                         //附加的错误原因参数
                uint16_t    code;                           //错误码
            };
            uint32_t        value;                          //便于使用的统一的错误码整数值
        };
        //布尔类型运算符重载,便于判断是否无错.
        operator bool() { return value == 0; }              //结构体的实例可以直接进行逻辑比较
    }rx_error;


    //-----------------------------------------------------
    //定义rx库统一使用的错误分类与代码
    //-----------------------------------------------------
    #define desc_rx_errcode(classify,no) ((classify)<<8|no)
    const uint8_t rx_errcls_mem     = 1;                    //内存操作错误分类
    const uint8_t rx_errcls_param   = 2;                    //入参错误分类
    const uint8_t rx_errcls_os      = 3;                    //操作系统错误分类
    const uint8_t rx_errcls_file    = 4;                    //文件系统错误分类
    const uint8_t rx_errcls_net     = 5;                    //网络访问错误分类
    const uint8_t rx_errcls_db      = 6;                    //数据库访问错误分类

    //错误码:没有错误,正常完成.
    const uint16_t rx_ec_ok         = 0;

    //错误码:内存不足,无法分配内存.
    const uint16_t rx_ec_mem_not_enough     = desc_rx_errcode(rx_errcls_mem, 1);
    //错误码:缓存不足,空间过小.
    const uint16_t rx_ec_buf_not_enough     = desc_rx_errcode(rx_errcls_mem, 2);

    //文件不存在
    const uint16_t rx_ec_file_not_exists    = desc_rx_errcode(rx_errcls_file, 1);
    //文件打开错误(打开错误/创建错误/权限不足...)
    const uint16_t rx_ec_file_open          = desc_rx_errcode(rx_errcls_file, 2);
    //文件删除错误
    const uint16_t rx_ec_file_remove        = desc_rx_errcode(rx_errcls_file, 3);
    //文件读取错误
    const uint16_t rx_ec_file_read          = desc_rx_errcode(rx_errcls_file, 4);
    //文件写入错误(写入/追加/权限不足...)
    const uint16_t rx_ec_file_write         = desc_rx_errcode(rx_errcls_file, 5);
    //文件其他错误
    const uint16_t rx_ec_file_op            = desc_rx_errcode(rx_errcls_file, 6);


}



#endif
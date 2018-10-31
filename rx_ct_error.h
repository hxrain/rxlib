#ifndef _RX_CT_ERROR_H_
#define _RX_CT_ERROR_H_

#include "rx_cc_macro.h"

namespace rx
{
    //-----------------------------------------------------
    //����ͳһ�Ĵ�������ؽṹ��
    typedef struct rx_error
    {
        union
        {
            struct
            {
                uint16_t    reason;                         //���ӵĴ���ԭ�����
                uint16_t    code;                           //������
            };
            uint32_t        value;                          //����ʹ�õ�ͳһ�Ĵ���������ֵ
        };
        //�����������������,�����ж��Ƿ��޴�.
        operator bool() { return value == 0; }              //�ṹ���ʵ������ֱ�ӽ����߼��Ƚ�
    }rx_error;


    //-----------------------------------------------------
    //����rx��ͳһʹ�õĴ�����������
    //-----------------------------------------------------
    #define desc_rx_errcode(classify,no) ((classify)<<8|no)
    const uint8_t rx_errcls_mem     = 1;                    //�ڴ�����������
    const uint8_t rx_errcls_param   = 2;                    //��δ������
    const uint8_t rx_errcls_os      = 3;                    //����ϵͳ�������
    const uint8_t rx_errcls_file    = 4;                    //�ļ�ϵͳ�������
    const uint8_t rx_errcls_net     = 5;                    //������ʴ������
    const uint8_t rx_errcls_db      = 6;                    //���ݿ���ʴ������

    //������:û�д���,�������.
    const uint16_t rx_ec_ok         = 0;

    //������:�ڴ治��,�޷������ڴ�.
    const uint16_t rx_ec_mem_not_enough     = desc_rx_errcode(rx_errcls_mem, 1);
    //������:���治��,�ռ��С.
    const uint16_t rx_ec_buf_not_enough     = desc_rx_errcode(rx_errcls_mem, 2);

    //�ļ�������
    const uint16_t rx_ec_file_not_exists    = desc_rx_errcode(rx_errcls_file, 1);
    //�ļ��򿪴���(�򿪴���/��������/Ȩ�޲���...)
    const uint16_t rx_ec_file_open          = desc_rx_errcode(rx_errcls_file, 2);
    //�ļ�ɾ������
    const uint16_t rx_ec_file_remove        = desc_rx_errcode(rx_errcls_file, 3);
    //�ļ���ȡ����
    const uint16_t rx_ec_file_read          = desc_rx_errcode(rx_errcls_file, 4);
    //�ļ�д�����(д��/׷��/Ȩ�޲���...)
    const uint16_t rx_ec_file_write         = desc_rx_errcode(rx_errcls_file, 5);
    //�ļ���������
    const uint16_t rx_ec_file_op            = desc_rx_errcode(rx_errcls_file, 6);


}



#endif
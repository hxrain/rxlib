#ifndef _RX_CT_ERROR_H_
#define _RX_CT_ERROR_H_

#include "rx_cc_macro.h"

namespace rx
{
	//-----------------------------------------------------
	//����ͳһ�Ĵ�������ؽṹ��
	typedef struct error_t
	{
		union
		{
			struct
			{
				uint16_t    e_reason;                      //���ӵĴ���ԭ�����
				uint16_t    e_code;                        //������,���±�ec_xxx
			};
			uint32_t        value;                         //����ʹ�õ�ͳһ�Ĵ���������ֵ
		};
		//�����������������,�����ж��Ƿ��д�.
		operator bool() { return e_code != 0; }            //�ṹ���ʵ������ֱ�ӽ����߼��Ƚ�

		bool operator == (const uint16_t ec) { return e_code == ec; }
		bool operator != (const uint16_t ec) { return e_code != ec; }

		//���캯��
		error_t() :value(0) {}
		error_t(const error_t& ec) :value(ec.value) {}
		error_t(const uint16_t code, const uint16_t reason = 0) :e_reason(reason), e_code(code) {}
		//���ô���ԭ���벢��������
		error_t reason(const uint16_t reason) { e_reason = reason; return value; }
	}error_t;


	//-----------------------------------------------------
	//�����������
	const uint8_t err_cls_base = 0;							//���������������
	const uint8_t err_cls_mem = 1;							//�ڴ�����������
	const uint8_t err_cls_data = 2;							//���������ݴ���
	const uint8_t err_cls_os = 3;							//����ϵͳ�������
	const uint8_t err_cls_file = 4;							//�ļ�ϵͳ�������
	const uint8_t err_cls_net = 5;							//������ʴ������
	const uint8_t err_cls_db = 6;							//���ݿ���ʴ������
	const uint8_t err_cls_limit = 7;						//Υ�����ƴ������

	//���������(�ɴ�����������������µ�������)
	#define desc_errcode(classify,no) ((classify)<<8|(uint8_t)no)

	//-----------------------------------------------------
	//������:û�д���,�������.
	const uint16_t ec_ok = desc_errcode(err_cls_base, 0);
	//������:δ��ʼ��
	const uint16_t ec_uninit = desc_errcode(err_cls_base, 1);
	//������:��������
	const uint16_t ec_param = desc_errcode(err_cls_base, 2);

	//-----------------------------------------------------
	//������:�ڴ治��,�޷������ڴ�.
	const uint16_t ec_mem_not_enough = desc_errcode(err_cls_mem, 1);
	//������:���治��,�ռ��С.
	const uint16_t ec_buf_not_enough = desc_errcode(err_cls_mem, 2);

	//-----------------------------------------------------
	//��δ���
	const uint16_t ec_data = desc_errcode(err_cls_data, 1);

	//-----------------------------------------------------
	//�ļ�������
	const uint16_t ec_file_not_exists = desc_errcode(err_cls_file, 1);
	//�ļ��򿪴���(�򿪴���/��������/Ȩ�޲���...)
	const uint16_t ec_file_open = desc_errcode(err_cls_file, 2);
	//�ļ�ɾ������
	const uint16_t ec_file_remove = desc_errcode(err_cls_file, 3);
	//�ļ���ȡ����
	const uint16_t ec_file_read = desc_errcode(err_cls_file, 4);
	//�ļ�д�����(д��/׷��/Ȩ�޲���...)
	const uint16_t ec_file_write = desc_errcode(err_cls_file, 5);
	//�ļ���������
	const uint16_t ec_file_op = desc_errcode(err_cls_file, 6);

	//-----------------------------------------------------
	//����socket��������
	const uint16_t ec_net_create = desc_errcode(err_cls_net, 1);
	//����socket��ַ�󶨴���
	const uint16_t ec_net_bind = desc_errcode(err_cls_net, 2);
	//����socket������������
	const uint16_t ec_net_listen = desc_errcode(err_cls_net, 3);
	//���糬ʱ����
	const uint16_t ec_net_timeout = desc_errcode(err_cls_net, 4);
	//�����ȡ����
	const uint16_t ec_net_read = desc_errcode(err_cls_net, 5);
	//����д�����
	const uint16_t ec_net_write = desc_errcode(err_cls_net, 6);
	//������������
	const uint16_t ec_net_other = desc_errcode(err_cls_net, 7);
	//����Է��ѶϿ�
	const uint16_t ec_net_disconn = desc_errcode(err_cls_net, 8);

	//-----------------------------------------------------
	//os����:mmap�򿪴���
	const uint16_t ec_os_mmap_open = desc_errcode(err_cls_os, 1);
	//os����:mmapӳ�����
	const uint16_t ec_os_mmap_bind = desc_errcode(err_cls_os, 2);
	//os����:�̴߳�������
	const uint16_t ec_os_thread_create = desc_errcode(err_cls_os, 3);
	//os����:mutex��������
	const uint16_t ec_os_mutex_create = desc_errcode(err_cls_os, 4);
	//os����:cond��������
	const uint16_t ec_os_cond_create = desc_errcode(err_cls_os, 5);
	//os����:sem��������
	const uint16_t ec_os_sem_create = desc_errcode(err_cls_os, 6);

	//-----------------------------------------------------
	//��������:�ظ�����
	const uint16_t ec_limit_double_op = desc_errcode(err_cls_limit, 1);
	//��������:���ݻ��߼�״̬����
	const uint16_t ec_limit_data = desc_errcode(err_cls_limit, 2);
}



#endif

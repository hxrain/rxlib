#ifndef _RX_CT_ERROR_H_
#define _RX_CT_ERROR_H_

#include "rx_cc_macro.h"

namespace rx
{
	//-----------------------------------------------------
	//定义统一的错误码承载结构体
	typedef struct error_t
	{
		union
		{
			struct
			{
				uint16_t    e_reason;                      //附加的错误原因参数
				uint16_t    e_code;                        //错误码,见下表ec_xxx
			};
			uint32_t        value;                         //便于使用的统一的错误码整数值
		};
		//布尔类型运算符重载,便于判断是否有错.
		operator bool() { return e_code != 0; }            //结构体的实例可以直接进行逻辑比较

		bool operator == (const uint16_t ec) { return e_code == ec; }
		bool operator != (const uint16_t ec) { return e_code != ec; }

		//构造函数
		error_t() :value(0) {}
		error_t(const error_t& ec) :value(ec.value) {}
		error_t(const uint16_t code, const uint16_t reason = 0) :e_reason(reason), e_code(code) {}
		//设置错误原因码并返回自身
		error_t reason(const uint16_t reason) { e_reason = reason; return value; }
	}error_t;


	//-----------------------------------------------------
	//错误领域类别
	const uint8_t err_cls_base = 0;							//基础常见错误分类
	const uint8_t err_cls_mem = 1;							//内存操作错误分类
	const uint8_t err_cls_data = 2;							//依赖的数据错误
	const uint8_t err_cls_os = 3;							//操作系统错误分类
	const uint8_t err_cls_file = 4;							//文件系统错误分类
	const uint8_t err_cls_net = 5;							//网络访问错误分类
	const uint8_t err_cls_db = 6;							//数据库访问错误分类
	const uint8_t err_cls_limit = 7;						//违反限制错误分类

	//定义错误码(由错误领域类别与该类别下的序号组成)
	#define desc_errcode(classify,no) ((classify)<<8|(uint8_t)no)

	//-----------------------------------------------------
	//错误码:没有错误,正常完成.
	const uint16_t ec_ok = desc_errcode(err_cls_base, 0);
	//错误码:未初始化
	const uint16_t ec_uninit = desc_errcode(err_cls_base, 1);
	//错误码:参数错误
	const uint16_t ec_param = desc_errcode(err_cls_base, 2);

	//-----------------------------------------------------
	//错误码:内存不足,无法分配内存.
	const uint16_t ec_mem_not_enough = desc_errcode(err_cls_mem, 1);
	//错误码:缓存不足,空间过小.
	const uint16_t ec_buf_not_enough = desc_errcode(err_cls_mem, 2);

	//-----------------------------------------------------
	//入参错误
	const uint16_t ec_data = desc_errcode(err_cls_data, 1);

	//-----------------------------------------------------
	//文件不存在
	const uint16_t ec_file_not_exists = desc_errcode(err_cls_file, 1);
	//文件打开错误(打开错误/创建错误/权限不足...)
	const uint16_t ec_file_open = desc_errcode(err_cls_file, 2);
	//文件删除错误
	const uint16_t ec_file_remove = desc_errcode(err_cls_file, 3);
	//文件读取错误
	const uint16_t ec_file_read = desc_errcode(err_cls_file, 4);
	//文件写入错误(写入/追加/权限不足...)
	const uint16_t ec_file_write = desc_errcode(err_cls_file, 5);
	//文件其他错误
	const uint16_t ec_file_op = desc_errcode(err_cls_file, 6);

	//-----------------------------------------------------
	//网络socket创建错误
	const uint16_t ec_net_create = desc_errcode(err_cls_net, 1);
	//网络socket地址绑定错误
	const uint16_t ec_net_bind = desc_errcode(err_cls_net, 2);
	//网络socket启动监听错误
	const uint16_t ec_net_listen = desc_errcode(err_cls_net, 3);
	//网络超时错误
	const uint16_t ec_net_timeout = desc_errcode(err_cls_net, 4);
	//网络读取错误
	const uint16_t ec_net_read = desc_errcode(err_cls_net, 5);
	//网络写入错误
	const uint16_t ec_net_write = desc_errcode(err_cls_net, 6);
	//网络其他错误
	const uint16_t ec_net_other = desc_errcode(err_cls_net, 7);
	//网络对方已断开
	const uint16_t ec_net_disconn = desc_errcode(err_cls_net, 8);

	//-----------------------------------------------------
	//os错误:mmap打开错误
	const uint16_t ec_os_mmap_open = desc_errcode(err_cls_os, 1);
	//os错误:mmap映射错误
	const uint16_t ec_os_mmap_bind = desc_errcode(err_cls_os, 2);
	//os错误:线程创建错误
	const uint16_t ec_os_thread_create = desc_errcode(err_cls_os, 3);
	//os错误:mutex创建错误
	const uint16_t ec_os_mutex_create = desc_errcode(err_cls_os, 4);
	//os错误:cond创建错误
	const uint16_t ec_os_cond_create = desc_errcode(err_cls_os, 5);
	//os错误:sem创建错误
	const uint16_t ec_os_sem_create = desc_errcode(err_cls_os, 6);

	//-----------------------------------------------------
	//功能限制:重复操作
	const uint16_t ec_limit_double_op = desc_errcode(err_cls_limit, 1);
	//功能限制:数据或逻辑状态错误
	const uint16_t ec_limit_data = desc_errcode(err_cls_limit, 2);
}



#endif

#ifndef _RX_OS_MMAP_H_
#define _RX_OS_MMAP_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_str_util_std.h"
#include "rx_os_file.h"
#include "rx_os_misc.h"

#if RX_IS_OS_WIN
namespace rx
{
	//-----------------------------------------------------
	//操作系统的文件内存映射管理功能(win版)
	class os_mmap_t
	{
		os_mmap_t& operator=(const os_mmap_t&);
		os_mmap_t(const os_mmap_t&);
	protected:
		HANDLE          m_handle;                           //共享内存文件句柄
		void           *m_mem_ptr;
		uint32_t        m_mem_size;
	public:
		//-------------------------------------------------
		os_mmap_t() :m_handle(NULL), m_mem_ptr(NULL) {}
		~os_mmap_t() { close(); }
		//-------------------------------------------------
		//判断映射是否有效
		bool is_valid() { return m_mem_ptr != NULL&&m_handle != NULL; }
		//-------------------------------------------------
		//获取映射过的尺寸
		uint32_t size() { return m_mem_size; }
		//-------------------------------------------------
		//得到内存指针:从指定的偏移开始
		uint8_t* ptr(uint32_t Offset = 0)
		{
			if (!m_mem_ptr || Offset >= m_mem_size) return NULL;
			return (uint8_t*)m_mem_ptr + Offset;
		}
		//-------------------------------------------------
		//根据文件句柄,创建共享内存
		/*mode:
			r :READONLY
			r+:READWRITE
			w :WRITECOPY
			w+:READWRITE
		*/
		//入口:File文件对象;mode打开模式;MemSize要创建的共享内存的大小,0时参照文件尺寸;FileOffset文件映射的偏移
		error_t open(os_file_t &file, const char* mode = "w+", uint32_t MemSize = 0, uint32_t FileOffset = 0)
		{
			if (m_handle)
				return ec_limit_double_op;

			if (is_empty(mode))
				return error_t(ec_param, -1);

			uint32_t flag_page = 0;
			uint32_t flag_map = 0;
			bool op_plus = false;

			if (st::strchr(mode, '+'))
				op_plus = true;
			if (st::strchr(mode, 'r'))
				flag_page = op_plus ? PAGE_READWRITE : PAGE_READONLY;
			if (st::strchr(mode, 'w'))
				flag_page = op_plus ? PAGE_READWRITE : PAGE_WRITECOPY;

			if (flag_page == PAGE_WRITECOPY)
				flag_map = FILE_MAP_COPY;
			else if (flag_page == PAGE_READWRITE)
				flag_map = FILE_MAP_ALL_ACCESS;
			else
				flag_map = FILE_MAP_READ;


			if (flag_page == PAGE_WRITECOPY && !file.is_valid())
				return error_t(ec_param, -2);				//写拷贝模式必须与文件句柄一起使用!

			if (MemSize == 0 && !file.size(MemSize))
				return error_t(ec_param, -3);

			//创建文件映射
			//os_security_desc_t SSD;
			m_handle = CreateFileMappingA((HANDLE)file.handle(), NULL/*SSD*/, flag_page, 0, MemSize, NULL);
			if (m_handle == NULL)
			{
				close();
				return ec_os_mmap_open;
			}

			//进行内存关联,得到内存映射后的数据指针
			m_mem_ptr = MapViewOfFile(m_handle, flag_map, 0, FileOffset, 0);
			if (m_mem_ptr == NULL)
			{
				close();
				return ec_os_mmap_bind;
			}

			m_mem_size = MemSize;
			return ec_ok;
		}

		//-------------------------------------------------
		//关闭内存映射对象
		void close()
		{
			if (m_mem_ptr)
				UnmapViewOfFile(m_mem_ptr);
			if (m_handle)
				CloseHandle(m_handle);
			m_handle = NULL;
			m_mem_ptr = NULL;
			m_mem_size = 0;
		}
		//-------------------------------------------------
		//写拷贝模式下的回写刷新
		bool flush()
		{
			return !!FlushViewOfFile(m_mem_ptr, 0);
		}
		//-------------------------------------------------
		//写数据:数据缓冲区指针;数据长度;内存起始偏移
		//返回值:0失败;其他为实际写入的长度(由于Offset的存在,导致可能不等于DataLen)
		uint32_t write(const void* Data, uint32_t DataLen, uint32_t Offset = 0)
		{
			if (!m_mem_ptr) return 0;
			if (DataLen > m_mem_size - Offset) DataLen = m_mem_size - Offset;
			memcpy(ptr(Offset), Data, DataLen);
			return DataLen;
		}
		//-------------------------------------------------
		//读数据:缓冲区;需要读取的长度;内存起始偏移
		//返回值:0失败;其他为实际读取的长度(由于Offset的存在,导致可能不等于ReadLen)
		uint32_t read(void* Buffer, uint32_t ReadLen, uint32_t Offset = 0)
		{
			if (!m_mem_ptr) return 0;
			if (ReadLen > m_mem_size - Offset) ReadLen = m_mem_size - Offset;
			memcpy(Buffer, ptr(Offset), ReadLen);
			return ReadLen;
		}
	};
}

#else

#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
namespace rx
{
	//-----------------------------------------------------
	//操作系统的文件内存映射管理功能(linux版)
	class os_mmap_t
	{
		os_mmap_t& operator=(const os_mmap_t&);
		os_mmap_t(const os_mmap_t&);
	protected:
		void           *m_mem_ptr;
		uint32_t        m_mem_size;
	public:
		//-------------------------------------------------
		os_mmap_t() :m_mem_ptr(NULL) {}
		~os_mmap_t() { close(); }
		//-------------------------------------------------
		//判断映射是否有效
		bool is_valid() { return m_mem_ptr != NULL; }
		//-------------------------------------------------
		//获取映射过的尺寸
		uint32_t size() { return m_mem_size; }
		//-------------------------------------------------
		//得到内存指针:从指定的偏移开始
		uint8_t* ptr(uint32_t Offset = 0)
		{
			if (!m_mem_ptr || Offset >= m_mem_size) return NULL;
			return (uint8_t*)m_mem_ptr + Offset;
		}
		//-------------------------------------------------
		//根据文件句柄,创建共享内存
		/*mode:
			r :READONLY
			r+:READWRITE
			w :WRITECOPY
			w+:READWRITE
		*/
		//入口:File文件对象;mode打开模式;MemSize要创建的共享内存的大小,0时参照文件尺寸;FileOffset文件映射的偏移
		error_t open(os_file_t &file, const char* mode = "w+", uint32_t MemSize = 0, uint32_t FileOffset = 0)
		{
			if (m_mem_ptr)
				return ec_limit_double_op;

			if (is_empty(mode))
				return error_t(ec_param, -1);

			uint32_t flag_prot = 0;
			uint32_t flag_map = 0;
			bool op_plus = false;

			if (st::strchr(mode, '+'))
				op_plus = true;
			if (st::strchr(mode, 'r'))
				flag_prot = op_plus ? PROT_READ | PROT_WRITE : PROT_READ;
			if (st::strchr(mode, 'w'))
				flag_prot = op_plus ? PROT_READ | PROT_WRITE : PROT_WRITE;

			if (flag_prot == PROT_WRITE)
				flag_map = MAP_PRIVATE;
			else
				flag_map = MAP_SHARED;

			if (flag_prot == PROT_WRITE&&flag_map == MAP_PRIVATE && !file.is_valid())
				return error_t(ec_param, -2);				//写拷贝模式必须与文件句柄一起使用!

			if (!file.is_valid())
				flag_map = MAP_ANONYMOUS;                   //文件句柄无效就当作匿名映射使用
			else
			{//文件句柄有效,则需要进行内存映射尺寸的校正处理
				uint32_t file_size = 0;
				if (!file.size(file_size))                  //获取文件尺寸
					return error_t(ec_file_op, -3);

				if (MemSize == 0)
					MemSize = file_size;
				else if (MemSize + FileOffset > file_size)
				{//如果指定的内存尺寸大于文件的实际尺寸,则尝试进行文件扩容
					error_t ec = alloc_file_size(file, MemSize + FileOffset);
					if (ec)
						return ec;
				}
			}

			if (MemSize == 0)
				return error_t(ec_param, -5);

			//进行内存关联,得到内存映射后的数据指针
			m_mem_ptr = mmap(NULL, MemSize, flag_prot, flag_map, file.handle(), FileOffset);
			if (m_mem_ptr == NULL)
			{
				close();
				return ec_os_mmap_bind;
			}

			m_mem_size = MemSize;
			return ec_ok;
		}

		//-------------------------------------------------
		//关闭内存映射对象
		void close()
		{
			if (m_mem_ptr)
				munmap(m_mem_ptr, m_mem_size);
			m_mem_ptr = NULL;
			m_mem_size = 0;
		}
		//-------------------------------------------------
		//写拷贝模式下的回写刷新
		bool flush()
		{
			return msync(m_mem_ptr, m_mem_size, MS_SYNC) != -1;
		}
		//-------------------------------------------------
		//写数据:数据缓冲区指针;数据长度;内存起始偏移
		//返回值:0失败;其他为实际写入的长度(可能不等于DataLen)
		uint32_t write(const void* Data, uint32_t DataLen, uint32_t Offset = 0)
		{
			if (!m_mem_ptr) return 0;
			if (DataLen > m_mem_size - Offset)
				DataLen = m_mem_size - Offset;
			void *dst = ptr(Offset);
			memcpy(dst, Data, DataLen);
			return DataLen;
		}
		//-------------------------------------------------
		//读数据:缓冲区;需要读取的长度;内存起始偏移
		//返回值:0失败;其他为实际读取的长度(可能不等于ReadLen)
		uint32_t read(void* Buffer, uint32_t ReadLen, uint32_t Offset = 0)
		{
			if (!m_mem_ptr) return 0;
			if (ReadLen > m_mem_size - Offset) ReadLen = m_mem_size - Offset;
			memcpy(Buffer, ptr(Offset), ReadLen);
			return ReadLen;
		}
	};
}
#endif

#endif

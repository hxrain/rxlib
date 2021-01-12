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
	//����ϵͳ���ļ��ڴ�ӳ�������(win��)
	class os_mmap_t
	{
		os_mmap_t& operator=(const os_mmap_t&);
		os_mmap_t(const os_mmap_t&);
	protected:
		HANDLE          m_handle;                           //�����ڴ��ļ����
		void           *m_mem_ptr;
		uint32_t        m_mem_size;
	public:
		//-------------------------------------------------
		os_mmap_t() :m_handle(NULL), m_mem_ptr(NULL) {}
		~os_mmap_t() { close(); }
		//-------------------------------------------------
		//�ж�ӳ���Ƿ���Ч
		bool is_valid() { return m_mem_ptr != NULL&&m_handle != NULL; }
		//-------------------------------------------------
		//��ȡӳ����ĳߴ�
		uint32_t size() { return m_mem_size; }
		//-------------------------------------------------
		//�õ��ڴ�ָ��:��ָ����ƫ�ƿ�ʼ
		uint8_t* ptr(uint32_t Offset = 0)
		{
			if (!m_mem_ptr || Offset >= m_mem_size) return NULL;
			return (uint8_t*)m_mem_ptr + Offset;
		}
		//-------------------------------------------------
		//�����ļ����,���������ڴ�
		/*mode:
			r :READONLY
			r+:READWRITE
			w :WRITECOPY
			w+:READWRITE
		*/
		//���:File�ļ�����;mode��ģʽ;MemSizeҪ�����Ĺ����ڴ�Ĵ�С,0ʱ�����ļ��ߴ�;FileOffset�ļ�ӳ���ƫ��
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
				return error_t(ec_param, -2);				//д����ģʽ�������ļ����һ��ʹ��!

			if (MemSize == 0 && !file.size(MemSize))
				return error_t(ec_param, -3);

			//�����ļ�ӳ��
			//os_security_desc_t SSD;
			m_handle = CreateFileMappingA((HANDLE)file.handle(), NULL/*SSD*/, flag_page, 0, MemSize, NULL);
			if (m_handle == NULL)
			{
				close();
				return ec_os_mmap_open;
			}

			//�����ڴ����,�õ��ڴ�ӳ��������ָ��
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
		//�ر��ڴ�ӳ�����
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
		//д����ģʽ�µĻ�дˢ��
		bool flush()
		{
			return !!FlushViewOfFile(m_mem_ptr, 0);
		}
		//-------------------------------------------------
		//д����:���ݻ�����ָ��;���ݳ���;�ڴ���ʼƫ��
		//����ֵ:0ʧ��;����Ϊʵ��д��ĳ���(����Offset�Ĵ���,���¿��ܲ�����DataLen)
		uint32_t write(const void* Data, uint32_t DataLen, uint32_t Offset = 0)
		{
			if (!m_mem_ptr) return 0;
			if (DataLen > m_mem_size - Offset) DataLen = m_mem_size - Offset;
			memcpy(ptr(Offset), Data, DataLen);
			return DataLen;
		}
		//-------------------------------------------------
		//������:������;��Ҫ��ȡ�ĳ���;�ڴ���ʼƫ��
		//����ֵ:0ʧ��;����Ϊʵ�ʶ�ȡ�ĳ���(����Offset�Ĵ���,���¿��ܲ�����ReadLen)
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
	//����ϵͳ���ļ��ڴ�ӳ�������(linux��)
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
		//�ж�ӳ���Ƿ���Ч
		bool is_valid() { return m_mem_ptr != NULL; }
		//-------------------------------------------------
		//��ȡӳ����ĳߴ�
		uint32_t size() { return m_mem_size; }
		//-------------------------------------------------
		//�õ��ڴ�ָ��:��ָ����ƫ�ƿ�ʼ
		uint8_t* ptr(uint32_t Offset = 0)
		{
			if (!m_mem_ptr || Offset >= m_mem_size) return NULL;
			return (uint8_t*)m_mem_ptr + Offset;
		}
		//-------------------------------------------------
		//�����ļ����,���������ڴ�
		/*mode:
			r :READONLY
			r+:READWRITE
			w :WRITECOPY
			w+:READWRITE
		*/
		//���:File�ļ�����;mode��ģʽ;MemSizeҪ�����Ĺ����ڴ�Ĵ�С,0ʱ�����ļ��ߴ�;FileOffset�ļ�ӳ���ƫ��
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
				return error_t(ec_param, -2);				//д����ģʽ�������ļ����һ��ʹ��!

			if (!file.is_valid())
				flag_map = MAP_ANONYMOUS;                   //�ļ������Ч�͵�������ӳ��ʹ��
			else
			{//�ļ������Ч,����Ҫ�����ڴ�ӳ��ߴ��У������
				uint32_t file_size = 0;
				if (!file.size(file_size))                  //��ȡ�ļ��ߴ�
					return error_t(ec_file_op, -3);

				if (MemSize == 0)
					MemSize = file_size;
				else if (MemSize + FileOffset > file_size)
				{//���ָ�����ڴ�ߴ�����ļ���ʵ�ʳߴ�,���Խ����ļ�����
					error_t ec = alloc_file_size(file, MemSize + FileOffset);
					if (ec)
						return ec;
				}
			}

			if (MemSize == 0)
				return error_t(ec_param, -5);

			//�����ڴ����,�õ��ڴ�ӳ��������ָ��
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
		//�ر��ڴ�ӳ�����
		void close()
		{
			if (m_mem_ptr)
				munmap(m_mem_ptr, m_mem_size);
			m_mem_ptr = NULL;
			m_mem_size = 0;
		}
		//-------------------------------------------------
		//д����ģʽ�µĻ�дˢ��
		bool flush()
		{
			return msync(m_mem_ptr, m_mem_size, MS_SYNC) != -1;
		}
		//-------------------------------------------------
		//д����:���ݻ�����ָ��;���ݳ���;�ڴ���ʼƫ��
		//����ֵ:0ʧ��;����Ϊʵ��д��ĳ���(���ܲ�����DataLen)
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
		//������:������;��Ҫ��ȡ�ĳ���;�ڴ���ʼƫ��
		//����ֵ:0ʧ��;����Ϊʵ�ʶ�ȡ�ĳ���(���ܲ�����ReadLen)
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

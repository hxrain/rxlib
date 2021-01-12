#ifndef _RX_OS_MISC_H_
#define _RX_OS_MISC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_str_util_ext.h"
#include "rx_cc_base.h"

#if RX_IS_OS_WIN
namespace rx
{
	//-------------------------------------------------
	//获取当前线程id
	inline size_t get_tid() { return GetCurrentThreadId(); }
	//获取当前进程id
	inline size_t get_pid() { return GetCurrentProcessId(); }
	//获取当前线程ID
	inline uint64_t get_thread_id() { return GetCurrentThreadId(); }
	//获取系统错误号
	inline uint32_t get_os_errno() { return GetLastError(); }
	//根据系统错误号获取系统错误信息
	inline uint32_t get_os_errmsg(char *buff, uint32_t size, uint32_t eno)
	{
		if (!buff) return 0;
		if (eno == 0)
			return st::strcpy(buff, size, "OK");

		uint32_t len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, eno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)buff, size, NULL);
		if (buff[len - 1] == '\n')
			--len;
		if (buff[len - 1] == '\r')
			--len;
		buff[len] = 0;
		return len;
	}

	//-------------------------------------------------
	//安全描述符管理类
	//这个类的目的:在后台服务中与前台标准APP共同访问一个全局对象时,使用这一个标准描述符,就能避免系统权限的问题
	class os_security_desc_t
	{
	protected:
		PVOID m_SDPtr;
		SECURITY_ATTRIBUTES m_SA;
		SECURITY_DESCRIPTOR m_SD;
		//---------------------------------------------
		//生成受限的SD
		inline static PVOID MakeSD(SECURITY_DESCRIPTOR *pSD)
		{
			SID_IDENTIFIER_AUTHORITY SiaWorld = { 0 };
			SiaWorld.Value[5] = 1;

			uint32_t dwAclLength;
			PSID psidEveryone = NULL;
			PACL ACL_Ptr = NULL;
			BOOL bResult = FALSE;

			try
			{
				//初始化安全描述符
				if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
				{
					rx_alert("InitializeSecurityDescriptor FAIL!");
					throw 1;
				}

				//得到授权用户组的SID
				if (!AllocateAndInitializeSid(&SiaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone))
				{
					rx_alert("AllocateAndInitializeSid FAIL!");
					throw 2;
				}

				//计算ACL的长度
				dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(uint32_t) + GetLengthSid(psidEveryone);

				//给ACL分配内存
				ACL_Ptr = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
				if (ACL_Ptr == NULL)
				{
					rx_alert("HeapAlloc FAIL!");
					throw 3;
				}

				if (!InitializeAcl(ACL_Ptr, dwAclLength, ACL_REVISION))
				{
					rx_alert("InitializeAcl FAIL!");
					throw 4;
				}

				// GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access
				if (!AddAccessAllowedAce(ACL_Ptr, ACL_REVISION, GENERIC_ALL, psidEveryone))
				{
					rx_alert("AddAccessAllowedAce FAIL!");
					throw 5;
				}

				if (!SetSecurityDescriptorDacl(pSD, TRUE, ACL_Ptr, FALSE))
				{
					rx_alert("SetSecurityDescriptorDacl FAIL!");
					throw 6;
				}

				bResult = TRUE;
			}
			catch (...) {}

			if (psidEveryone)
				FreeSid(psidEveryone);

			if (bResult == FALSE)
			{
				Free(ACL_Ptr);
				ACL_Ptr = NULL;
			}
			return (PVOID)ACL_Ptr;
		}
		//---------------------------------------------
		//释放描述符
		static VOID Free(PVOID ptr)
		{
			if (ptr)
				HeapFree(GetProcessHeap(), 0, ptr);
		}
		//---------------------------------------------
	public:
		os_security_desc_t()
		{
			m_SDPtr = NULL;
			m_SA.nLength = sizeof(m_SA);
			m_SA.lpSecurityDescriptor = &m_SD;
			m_SA.bInheritHandle = FALSE; // build a restricted security descriptor
			m_SDPtr = MakeSD(&m_SD);
			rx_assert_msg(m_SDPtr != NULL, "os_security_desc_t init FAIL!");
		}
		//---------------------------------------------
		virtual ~os_security_desc_t() { Free(m_SDPtr); }
		//---------------------------------------------
		operator SECURITY_ATTRIBUTES*()
		{
			if (m_SDPtr)
				return  &m_SA;
			else
				return NULL;
		}
	};

	//-------------------------------------------------
	//根据前缀与名字得到内核对象的名字
	static inline char* MakeCoreObjectName(char* ResultStr, const char* Prefix, const char* Name)
	{
		rx_assert(ResultStr != NULL&&Name != NULL);
		ResultStr[0] = 0;
		strcpy(ResultStr, "Global\\");
		if (Prefix)
			strcat(ResultStr, Prefix);
		strcat(ResultStr, Name);
		return ResultStr;
	}
}

#else

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
namespace rx
{
	//-------------------------------------------------
	//获取当前线程id(全系统唯一)
	inline size_t get_tid() { return syscall(SYS_gettid); }
	//获取当前进程id
	inline size_t get_pid() { return syscall(SYS_getpid); }
	//获取当前线程ID(进程内唯一)
	inline uint64_t get_thread_id() { return pthread_self(); }
	//获取系统错误号
	inline uint32_t get_os_errno() { return errno; }
	//根据系统错误号获取系统错误信息
	inline uint32_t get_os_errmsg(char *buff, uint32_t size, uint32_t eno)
	{
		if (!buff) return 0;
		buff[0] = 0;
		uint32_t ret = st::strcpy(buff, size, strerror(eno));
		if (!buff[0]) return 0;
		return ret;
	}
}

#endif

namespace rx
{
	//-------------------------------------------------
	//通过系统错误代码得到错误消息描述
	class os_errmsg_t
	{
		char m_buff[512];
	public:
		os_errmsg_t() { m_buff[0] = 0; }
		os_errmsg_t(const char* tip) { msg(tip); }
		operator const char* () { return m_buff; }
		//---------------------------------------------
		//得到错误消息
		inline char* msg()
		{
			uint32_t eno;
			return msg(eno);
		}
		//同时得到错误号与错误消息
		inline char* msg(uint32_t &eno)
		{
			sncat<0> cat(m_buff);
			eno = get_os_errno();
			cat("os:<%u>:", eno);
			get_os_errmsg(cat.str + cat.size, sizeof(m_buff) - cat.size, eno);
			return m_buff;
		}
		//---------------------------------------------
		//得到完整的错误消息并格式化用户提示信息
		inline char* msg(const char* tip)
		{
			sncat<0> cat(m_buff);
			uint32_t eno = get_os_errno();
			char tmp[256];
			get_os_errmsg(tmp, sizeof(tmp), eno);
			cat("os<%u:", eno)("%s>", tmp)(" tip<%s>", tip);
			return m_buff;
		}
	};
}

#endif

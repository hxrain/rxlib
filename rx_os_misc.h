#ifndef _RX_OS_MISC_H_
#define _RX_OS_MISC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_str_util_std.h"


#if RX_OS_WIN
namespace rx
{
    //-----------------------------------------------------
    //安全描述符管理类
    //这个类的目的:在后台服务中与前台标准APP共同访问一个全局对象时,使用这一个标准描述符,就能避免系统权限的问题
    class os_security_desc_t
    {
    protected:
        PVOID m_SDPtr;
        SECURITY_ATTRIBUTES m_SA;
        SECURITY_DESCRIPTOR m_SD;
        //-------------------------------------------------
        //生成受限的SD
        inline static PVOID MakeSD(SECURITY_DESCRIPTOR *pSD)
        {
            SID_IDENTIFIER_AUTHORITY SiaWorld = { 0 };
            SiaWorld.Value[5] = 1;

            DWORD dwAclLength;
            PSID psidEveryone = NULL;
            PACL ACL_Ptr = NULL;
            BOOL bResult = FALSE;

            try
            {
                //初始化安全描述符
                if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
                {
                    rx_alert("InitializeSecurityDescriptor 初始化失败!"); throw 1;
                }

                //得到授权用户组的SID
                if (!AllocateAndInitializeSid(&SiaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone))
                {
                    rx_alert("AllocateAndInitializeSid 初始化失败!"); throw 2;
                }

                //计算ACL的长度
                dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(psidEveryone);

                //给ACL分配内存
                ACL_Ptr = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
                if (ACL_Ptr == NULL) { rx_alert("HeapAlloc 内存分配失败!"); throw 3; }

                if (!InitializeAcl(ACL_Ptr, dwAclLength, ACL_REVISION))
                {
                    rx_alert("InitializeAcl 失败!"); throw 4;
                }

                // GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access
                if (!AddAccessAllowedAce(ACL_Ptr, ACL_REVISION, GENERIC_ALL, psidEveryone))
                {
                    rx_alert("AddAccessAllowedAce 失败"); throw 5;
                }

                if (!SetSecurityDescriptorDacl(pSD, TRUE, ACL_Ptr, FALSE))
                {
                    rx_alert("SetSecurityDescriptorDacl 失败!"); throw 6;
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
        //-------------------------------------------------
        //释放描述符
        static VOID Free(PVOID ptr)
        {
            if (ptr)
                HeapFree(GetProcessHeap(), 0, ptr);
        }
        //-------------------------------------------------
    public:
        os_security_desc_t()
        {
            m_SDPtr = NULL;
            m_SA.nLength = sizeof(m_SA);
            m_SA.lpSecurityDescriptor = &m_SD;
            m_SA.bInheritHandle = FALSE; // build a restricted security descriptor
            m_SDPtr = MakeSD(&m_SD);
            rx_assert_msg(m_SDPtr != NULL, "os_security_desc_t初始化失败!");
        }
        //-------------------------------------------------
        virtual ~os_security_desc_t() { Free(m_SDPtr); }
        //-------------------------------------------------
        operator SECURITY_ATTRIBUTES*()
        {
            if (m_SDPtr)
                return  &m_SA;
            else
                return NULL;
        }
    };

    //-----------------------------------------------------
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
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <unistd.h>
namespace rx
{
}

#endif

#endif

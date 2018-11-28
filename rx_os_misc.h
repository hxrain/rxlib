#ifndef _RX_OS_MISC_H_
#define _RX_OS_MISC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_str_util_std.h"


#if RX_OS_WIN
namespace rx
{
    //-----------------------------------------------------
    //��ȫ������������
    //������Ŀ��:�ں�̨��������ǰ̨��׼APP��ͬ����һ��ȫ�ֶ���ʱ,ʹ����һ����׼������,���ܱ���ϵͳȨ�޵�����
    class os_security_desc_t
    {
    protected:
        PVOID m_SDPtr;
        SECURITY_ATTRIBUTES m_SA;
        SECURITY_DESCRIPTOR m_SD;
        //-------------------------------------------------
        //�������޵�SD
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
                //��ʼ����ȫ������
                if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
                {
                    rx_alert("InitializeSecurityDescriptor ��ʼ��ʧ��!"); throw 1;
                }

                //�õ���Ȩ�û����SID
                if (!AllocateAndInitializeSid(&SiaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone))
                {
                    rx_alert("AllocateAndInitializeSid ��ʼ��ʧ��!"); throw 2;
                }

                //����ACL�ĳ���
                dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(psidEveryone);

                //��ACL�����ڴ�
                ACL_Ptr = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
                if (ACL_Ptr == NULL) { rx_alert("HeapAlloc �ڴ����ʧ��!"); throw 3; }

                if (!InitializeAcl(ACL_Ptr, dwAclLength, ACL_REVISION))
                {
                    rx_alert("InitializeAcl ʧ��!"); throw 4;
                }

                // GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access
                if (!AddAccessAllowedAce(ACL_Ptr, ACL_REVISION, GENERIC_ALL, psidEveryone))
                {
                    rx_alert("AddAccessAllowedAce ʧ��"); throw 5;
                }

                if (!SetSecurityDescriptorDacl(pSD, TRUE, ACL_Ptr, FALSE))
                {
                    rx_alert("SetSecurityDescriptorDacl ʧ��!"); throw 6;
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
        //�ͷ�������
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
            rx_assert_msg(m_SDPtr != NULL, "os_security_desc_t��ʼ��ʧ��!");
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
    //����ǰ׺�����ֵõ��ں˶��������
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

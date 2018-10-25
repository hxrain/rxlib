#ifndef _RX_OS_MMAP_H_
#define _RX_OS_MMAP_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_str_util.h"
#include "rx_os_file.h"

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

    //-----------------------------------------------------
    class os_mmap_t
    {
        os_mmap_t& operator=(const os_mmap_t&);
        os_mmap_t(const os_mmap_t&);
    protected:
        HANDLE          m_handle;                           //�����ڴ��ļ����
        void           *m_mem_ptr;
        uint32_t        m_mem_size;
        //-------------------------------------------------
        //���ݸ����Ķ�д����,ȷ���ڴ�ҳ��������
        static uint32_t m_PageMode(HShareMemMode OpenMode)
        {
            uint32_t Flag;
            if (OpenMode == SMM_WriteCopy)
                Flag = PAGE_WRITECOPY;
            else if (OpenMode == SMM_ReadWrite)
                Flag = PAGE_READWRITE;
            else
                Flag = PAGE_READONLY;
            return Flag;
        }
        //-------------------------------------------------
        //���ݸ����Ķ�д����,ȷ��ӳ����
        static uint32_t m_MapMode(HShareMemMode OpenMode)
        {
            uint32_t Flag;
            if (OpenMode == SMM_WriteCopy)
                Flag = FILE_MAP_COPY;
            else if (OpenMode == SMM_ReadWrite)
                Flag = FILE_MAP_ALL_ACCESS;
            else
                Flag = FILE_MAP_READ;
            return Flag;
        }
    public:
        //-------------------------------------------------
        os_mmap_t():m_handle(NULL){}
        ~os_mmap_t() { close(); }
        //-------------------------------------------------
        bool is_valid() { return m_mem_ptr != NULL&&m_handle != NULL; }
        //-------------------------------------------------
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
        //���:File�ļ�����;mode��ģʽ;MemSizeҪ�����Ĺ����ڴ�Ĵ�С,0ʱ�����ļ��ߴ�;FileOffset�ļ�ӳ���ƫ��
        //����ֵ:<=0����;>0�ɹ�
        int open(os_file_t &file,const char* mode,uint32_t MemSize=0, uint32_t FileOffset = 0)
        {
            if (m_handle)
                return 0;

            if (FileHandle == (HANDLE)0xFFFFFFFF && OpenMode == SMM_WriteCopy)
            {
                rx_alert("д����ģʽ�������ļ����һ��ʹ��!���ṩһ����Ч���ļ����!");
                return false;
            }

            //os_security_desc_t SSD;
            m_handle = CreateFileMappingA(FileHandle, NULL/*SSD*/, m_PageMode(OpenMode), 0, MemSize, NULL); //���������ڴ�
            if (m_handle == NULL)
            {
                close();
                return false;
            }

            m_mem_ptr = MapViewOfFile(m_handle, m_MapMode(OpenMode), 0, FileOffset, 0);   //�õ��ڴ�ӳ��������ָ��
            if (m_mem_ptr == NULL)
            {
                close();
                return false;
            }

            m_mem_size = MemSize;
            return true;
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
            return FlushViewOfFile(m_mem_ptr, 0);
        }
        //-------------------------------------------------
        //д����:���ݻ�����ָ��;���ݳ���;�ڴ���ʼƫ��
        //����ֵ:0ʧ��;����Ϊʵ��д��ĳ���(���ܲ�����DataLen)
        uint32_t write(const uint8_t* Data, uint32_t DataLen, uint32_t Offset = 0)
        {
            if (!m_mem_ptr) return 0;
            if (DataLen>m_mem_size - Offset) DataLen = m_mem_size - Offset;
            memcpy(ptr(Offset), Data, DataLen);
            return DataLen;
        }
        //-------------------------------------------------
        //������:������;��Ҫ��ȡ�ĳ���;�ڴ���ʼƫ��
        //����ֵ:0ʧ��;����Ϊʵ�ʶ�ȡ�ĳ���(���ܲ�����ReadLen)
        uint32_t read(uint8_t* Buffer, uint32_t ReadLen, uint32_t Offset = 0)
        {
            if (!m_mem_ptr) return 0;
            if (ReadLen>m_mem_size - Offset) ReadLen = m_mem_size - Offset;
            memcpy(Buffer, ptr(Offset), ReadLen);
            return ReadLen;
        }
    };

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

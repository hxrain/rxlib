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

    //-----------------------------------------------------
    class os_mmap_t
    {
        os_mmap_t& operator=(const os_mmap_t&);
        os_mmap_t(const os_mmap_t&);
    protected:
        HANDLE          m_handle;                           //共享内存文件句柄
        void           *m_mem_ptr;
        uint32_t        m_mem_size;
        //-------------------------------------------------
        //根据给定的读写类型,确定内存页面操作标记
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
        //根据给定的读写类型,确定映射标记
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
        //得到内存指针:从指定的偏移开始
        uint8_t* ptr(uint32_t Offset = 0)
        {
            if (!m_mem_ptr || Offset >= m_mem_size) return NULL;
            return (uint8_t*)m_mem_ptr + Offset;
        }
        //-------------------------------------------------
        //根据文件句柄,创建共享内存
        //入口:File文件对象;mode打开模式;MemSize要创建的共享内存的大小,0时参照文件尺寸;FileOffset文件映射的偏移
        //返回值:<=0错误;>0成功
        int open(os_file_t &file,const char* mode,uint32_t MemSize=0, uint32_t FileOffset = 0)
        {
            if (m_handle)
                return 0;

            if (FileHandle == (HANDLE)0xFFFFFFFF && OpenMode == SMM_WriteCopy)
            {
                rx_alert("写拷贝模式必须与文件句柄一起使用!请提供一个有效的文件句柄!");
                return false;
            }

            //os_security_desc_t SSD;
            m_handle = CreateFileMappingA(FileHandle, NULL/*SSD*/, m_PageMode(OpenMode), 0, MemSize, NULL); //创建共享内存
            if (m_handle == NULL)
            {
                close();
                return false;
            }

            m_mem_ptr = MapViewOfFile(m_handle, m_MapMode(OpenMode), 0, FileOffset, 0);   //得到内存映射后的数据指针
            if (m_mem_ptr == NULL)
            {
                close();
                return false;
            }

            m_mem_size = MemSize;
            return true;
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
            return FlushViewOfFile(m_mem_ptr, 0);
        }
        //-------------------------------------------------
        //写数据:数据缓冲区指针;数据长度;内存起始偏移
        //返回值:0失败;其他为实际写入的长度(可能不等于DataLen)
        uint32_t write(const uint8_t* Data, uint32_t DataLen, uint32_t Offset = 0)
        {
            if (!m_mem_ptr) return 0;
            if (DataLen>m_mem_size - Offset) DataLen = m_mem_size - Offset;
            memcpy(ptr(Offset), Data, DataLen);
            return DataLen;
        }
        //-------------------------------------------------
        //读数据:缓冲区;需要读取的长度;内存起始偏移
        //返回值:0失败;其他为实际读取的长度(可能不等于ReadLen)
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

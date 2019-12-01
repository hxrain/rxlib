#ifndef _RX_OS_FILE_H_
#define _RX_OS_FILE_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_str_util_std.h"
#include "rx_ct_error.h"

#if RX_OS_WIN
namespace rx
{
    //-----------------------------------------------------
    //�����ļ�ϵͳAPI��װ�ļ�����������
    class os_file_t
    {
        HANDLE  m_handle;
    public:
        //-------------------------------------------------
        //�ļ�ʱ�����׼ʱ����໥ת��
        static void  ft2t(const FILETIME &ft, time_t &t)
        {
            rx_static_assert(0x019DB1DED53E8000 == 116444736000000000);
            ULARGE_INTEGER I;
            I.LowPart = ft.dwLowDateTime;
            I.HighPart = ft.dwHighDateTime;
            //��Ԫ1601�굽��Ԫ1970�����369�꣬ת��Ϊ������Ϊ11644473600��
            //ft����UTC��Coordinated Universal Time��ʱ��1601��1��1�տ�ʼ��100����Ϊ��λ��ʱ��
            t = (time_t)(LONGLONG(I.QuadPart - 116444736000000000) / 10000000);
        }
        static void  t2ft(time_t t, FILETIME &ft)
        {
            ULARGE_INTEGER I;
            I.QuadPart = t * 10000000 + 116444736000000000;
            ft.dwLowDateTime = I.LowPart;
            ft.dwHighDateTime = I.HighPart;
        }
        //-------------------------------------------------
        os_file_t() :m_handle(INVALID_HANDLE_VALUE) {}
        virtual ~os_file_t() { close(); }
        bool is_valid() { return m_handle != INVALID_HANDLE_VALUE; }
        //-------------------------------------------------
        size_t handle() { return (size_t)m_handle; }
        //-------------------------------------------------
        //����һ���ļ�,write_over=false�ļ��򿪻򴴽�;write_over=true�ļ����ǻ򴴽�
        //flagsĬ�ϴ�Ϊ�ɶ�д;modeĬ�ϱ��˿��Թ����,0Ϊ��ռ;�ļ�����Ĭ��Ϊ����
        error_code create(const char* filename, bool write_over, uint32_t flags = GENERIC_READ | GENERIC_WRITE, uint32_t mode = FILE_SHARE_READ| FILE_SHARE_WRITE, uint32_t attrib = FILE_ATTRIBUTE_NORMAL)
        {
            if (is_empty(filename))
                return ec_in_param;
            m_handle = CreateFileA(filename, flags, mode, NULL, write_over ? CREATE_ALWAYS : OPEN_ALWAYS, attrib, NULL);
            return m_handle == INVALID_HANDLE_VALUE ? ec_file_open : ec_ok;
        }
        //-------------------------------------------------
        //��һ���ļ�,���ֲ����ھ�ʧ��
        //flagsĬ�ϴ�Ϊ�ɶ�д;modeĬ�ϱ��˿��Թ����,0Ϊ��ռ;�ļ�����Ĭ��Ϊ����
        error_code open(const char* filename, uint32_t flags = GENERIC_READ | GENERIC_WRITE, uint32_t mode = FILE_SHARE_READ| FILE_SHARE_WRITE, uint32_t attrib = FILE_ATTRIBUTE_NORMAL)
        {
            if (is_empty(filename))
                return ec_in_param;
            m_handle = CreateFileA(filename, flags, mode, NULL, OPEN_EXISTING, attrib, NULL);
            return m_handle == INVALID_HANDLE_VALUE ? ec_file_open : ec_ok;
        }
        //-------------------------------------------------
        //��std/fopen���ļ��򿪲���
        error_code open(const char* filename, const char* Mode)
        {   //����vc2008
            //r : read
            //w : write
            //a : append
            //r+: read/write
            //w+: open empty for read/write
            //a+: read/append

            //����bcb6
            //r	: open for reading only.
            //w	: create for writing. If a file by that name already exists, it will be overwritten.
            //a	: Append; open for writing at end-of-file or create for writing if the file does not exist.
            //r+: open an existing file for update (reading and writing).
            //w+: create a new file for update (reading and writing). If a file by that name already exists, it will be overwritten.
            //a+: open for append; open (or create if the file does not exist) for update at the end of the file.

            if (is_empty(filename)||is_empty(Mode))
                return ec_in_param;

            uint32_t flag_write = 0;
            uint32_t flag_read = 0;
            bool op_plus = false;
            bool op_append = false;
            bool op_write = false;
            bool op_read = false;

            if (st::strchr(Mode, '+'))
                op_plus = true;
            if (st::strchr(Mode, 'r'))
            {
                op_read = true;
                flag_read = GENERIC_READ;
            }
            if (st::strchr(Mode, 'w'))
            {
                op_write = true;
                flag_write = GENERIC_WRITE;
            }
            if (st::strchr(Mode, 'a'))
            {
                op_append = true;
                flag_write = FILE_APPEND_DATA;
            }

            if (op_plus)
            {
                if (op_read)
                    flag_write = GENERIC_WRITE;
                if (op_write)
                    flag_read = GENERIC_READ;
                if (op_append)
                    flag_read = GENERIC_READ;
            }

            if (op_read)                                    //"r"ģʽ,���򿪲�����
                return open(filename, flag_read | flag_write);
            else                                            //"w"ģʽ,����Ҫ������;"a"������������
                return create(filename, op_write, flag_write | flag_read);
        }
        //-------------------------------------------------
        void close()
        {
            if (m_handle != INVALID_HANDLE_VALUE)
            {
                CloseHandle(m_handle);
                m_handle = INVALID_HANDLE_VALUE;
            }
        }
        //-------------------------------------------------
        //�����ݵ�������:������,ϣ����ȡ������,ʵ�ʶ�ȡ�ĳߴ�.
        //����ֵ:�Ƿ�ɹ�
        bool read(void *Buf, uint32_t NeedSize,uint32_t &RealReadedSize)
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;

            if (!ReadFile(m_handle, Buf, NeedSize, (DWORD*)&RealReadedSize, NULL))
                return false;
            return true;
        }
        //-------------------------------------------------
        //д���ݵ��ļ�:������,ϣ��д��������
        bool write(const void *Buf, size_t Size)
        {
            rx_assert(m_handle != INVALID_HANDLE_VALUE);
            DWORD WNum = 0;
            bool Ret = !!WriteFile(m_handle, Buf, (DWORD)Size, &WNum, NULL);
            return Ret&&WNum == Size;
        }
        //-------------------------------------------------
        //��ѯ�ļ���С
        bool size(uint32_t &Ret)
        {
            Ret = ::GetFileSize(m_handle, NULL);
            return (Ret != INVALID_FILE_SIZE);
        }
        bool size(uint64_t &Ret)
        {
            DWORD HS = 0;
            Ret = ::GetFileSize(m_handle, &HS);
            if (Ret == INVALID_FILE_SIZE) return false;
            Ret = (uint64_t)HS << 32 | Ret;
            return true;
        }
        //-------------------------------------------------
        //�����ļ���дָ��ƫ��:orgΪ�������,0-�ļ�ͷ,1-�ļ���ǰλ��,2-�ļ�β
        bool seek(LONG pos, uint32_t org=0, uint32_t *ret=NULL)
        {
            rx_assert(m_handle != INVALID_HANDLE_VALUE);
            uint32_t OM = 0;
            switch (org)
            {
            case 0:
                OM = FILE_BEGIN;
                break;
            case 1:
                OM = FILE_CURRENT;
                break;
            case 2:
                OM = FILE_END;
                break;
            default:
                rx_alert("offset type error");
                break;
            }
            OM = SetFilePointer(m_handle, pos, NULL, OM);
            if (ret) *ret = OM;
            return OM != (uint32_t)-1;
        }
        //-------------------------------------------------
        //�õ���ǰ���ļ�ָ��λ��
        //����ֵ:�Ƿ��ȡ�ɹ�
        bool tell(uint32_t &ret)
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;
            ret=SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);
            return ret != (uint32_t)-1;
        }
        //-------------------------------------------------
        //�����ļ��ڵ�ǰ��д�㴦����
        bool truncate()
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;
            return !!SetEndOfFile(m_handle);
        }
        //-------------------------------------------------
        //������д���浽����
        bool flush()
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;
            return !!FlushFileBuffers(m_handle);
        }
        //-------------------------------------------------
        //�����ļ�ʱ��
        bool time_modif(const time_t *Create, const time_t* Modify = NULL, const time_t* Access = NULL)
        {
            FILETIME FTCreate, FTModify, FTAccess;
            if (Create) t2ft(*Create, FTCreate);
            if (Modify) t2ft(*Modify, FTModify);
            if (Access) t2ft(*Access, FTAccess);

            rx_assert(m_handle != INVALID_HANDLE_VALUE);
            return !!::SetFileTime(m_handle, Create?&FTCreate:NULL, Access?&FTAccess:NULL, Modify?&FTModify:NULL);
        }
        //-------------------------------------------------
        //���µõ���ǰ�ļ���ʱ��
        bool time_query(time_t *Create, time_t* Modify = NULL, time_t* Access = NULL)
        {
            FILETIME FTCreate, FTModify, FTAccess;

            rx_assert(m_handle != INVALID_HANDLE_VALUE);
            if (!GetFileTime(m_handle, &FTCreate, &FTAccess, &FTModify))
                return false;
            if (Create) ft2t(FTCreate, *Create);
            if (Modify) ft2t(FTModify, *Modify);
            if (Access) ft2t(FTAccess, *Access);
            return true;
        }
        //-------------------------------------------------
        //��ʽ����ӡ������ļ�
        template<uint32_t BuffSize>
        bool print(const char* Fmt, ...)
        {
            va_list	ap;
            va_start(ap, Fmt);
            char Buff[BuffSize];
            int R = st::vsnprintf(Buff, BuffSize, Fmt, ap);
            va_end(ap);
            if (R < 0)
                return false;
            return write(Buff, R);
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
    /*
    flags:
        O_RDONLY        ֻ��ģʽ
        O_WRONLY        ֻдģʽ
        O_RDWR          ��дģʽ
        ���������ǻ���ģ���������ͬʱʹ�á�
        O_APPEND        ÿ��д������д���ļ���ĩβ
        O_CREAT         ���ָ���ļ������ڣ��򴴽�����ļ�
        O_EXCL          ���Ҫ�������ļ��Ѵ��ڣ��򷵻� -1�������޸� errno ��ֵ
        O_TRUNC         ����ļ����ڣ�������ֻд/��д��ʽ�򿪣�������ļ�ȫ������
        O_NOCTTY        ���·����ָ���ն��豸����Ҫ������豸���������նˡ�
        O_NONBLOCK      ���·����ָ�� FIFO/���ļ�/�ַ��ļ�������ļ��Ĵ򿪺ͺ�� I/O����Ϊ������ģʽ��nonblocking mode����
        //��������ͬ���������
        O_DSYNC         �ȴ����� I/O �������� write���ڲ�Ӱ���ȡ��д������ݵ�ǰ���£����ȴ��ļ����Ը��¡�
        O_RSYNC         read �ȴ�����д��ͬһ�����д������ɺ��ٽ���
        O_SYNC          �ȴ����� I/O �������� write�����������ļ����Ե� I/O
    attrib:
        S_IRUSR:        ��Ȩ�ޣ��ļ�����
        S_IWUSR:        дȨ�ޣ��ļ�����
        S_IXUSR:        ִ��Ȩ�ޣ��ļ�����
        S_IRGRP:        ��Ȩ�ޣ��ļ�������
        S_IWGRP:        дȨ�ޣ��ļ�������
        S_IXGRP:        ִ��Ȩ�ޣ��ļ�������
        S_IROTH:        ��Ȩ�ޣ������û�
        S_IWOTH:        дȨ�ޣ������û�
        S_IXOTH:        ִ��Ȩ�ޣ������û�
    error:
        EEXIST          ����filename ��ָ���ļ��Ѵ��ڣ�ȴʹ����O_CREAT��O_EXCL��ꡣ
        EACCESS         ����filename��ָ���ļ���������Ҫ����Ե�Ȩ�ޡ�
        EROFS           ������д��Ȩ�޵��ļ�������ֻ���ļ�ϵͳ�ڡ�
        EFAULT          ����filenameָ�볬���ɴ�ȡ�ڴ�ռ䡣
        EINVAL          ����mode ����ȷ��
        ENAMETOOLONG    ����filename̫����
        ENOTDIR         ����filename����Ŀ¼��
        ENOMEM          �����ڴ治�㡣
        ELOOP           ����filename�й�������������⡣
        EIO             I/O��ȡ����
    */

    //-----------------------------------------------------
    //�����ļ�ϵͳAPI��װ�ļ�����������
    class os_file_t
    {
        int  m_handle;
    public:
        //-------------------------------------------------
        os_file_t() :m_handle(-1) {}
        virtual ~os_file_t() { close(); }
        bool is_valid() { return m_handle != -1; }
        //-------------------------------------------------
        size_t handle() { return (size_t)m_handle; }
        //-------------------------------------------------
        //��һ���ļ�,���ֲ����ھ�ʧ��
        //flagsĬ�ϴ�Ϊ�ɶ�д;�ļ�����Ĭ��Ϊ�û���д
        error_code open(const char* filename, uint32_t flags = O_RDWR|O_CREAT, uint32_t attrib = S_IRUSR|S_IWUSR)
        {
            if (is_empty(filename))
                return ec_in_param;
            m_handle = ::open(filename,flags,attrib);
            return m_handle == -1 ? ec_file_open : ec_ok;
        }
        //-------------------------------------------------
        bool open(const char* filename, const char* Mode)
        {   //����vc2008
            //r : read
            //w : write
            //a : append
            //r+: read/write
            //w+: open empty for read/write
            //a+: read/append

            //����bcb6
            //r	: open for reading only.
            //w	: create for writing. If a file by that name already exists, it will be overwritten.
            //a	: Append; open for writing at end-of-file or create for writing if the file does not exist.
            //r+: open an existing file for update (reading and writing).
            //w+: create a new file for update (reading and writing). If a file by that name already exists, it will be overwritten.
            //a+: open for append; open (or create if the file does not exist) for update at the end of the file.

            if (is_empty(filename)||is_empty(Mode))
                return ec_in_param;

            uint32_t flag_write = 0;
            uint32_t flag_read = 0;
            bool op_plus = false;

            if (st::strchr(Mode, '+'))
                op_plus = true;

            if (st::strchr(Mode, 'r'))
            {
                flag_read = op_plus ? O_RDWR : O_RDONLY;
            }
            if (st::strchr(Mode, 'w'))
            {
                flag_write = op_plus ? O_RDWR | O_CREAT : O_WRONLY | O_CREAT;
            }
            if (st::strchr(Mode, 'a'))
            {
                flag_write = op_plus ? O_RDWR | O_APPEND | O_CREAT : O_APPEND | O_CREAT;
            }

            return open(filename, flag_read | flag_write);
        }
        //-------------------------------------------------
        void close()
        {
            if (m_handle != -1)
            {
                ::close(m_handle);
                m_handle = -1;
            }
        }
        //�����ݵ�������:������,ϣ����ȡ������,ʵ�ʶ�ȡ������
        //����ֵ:�Ƿ�ɹ�
        bool read(void *Buf, uint32_t NeedSize,uint32_t &RealReadSize)
        {
            if (m_handle == -1) return false;
            ssize_t ret = ::read(m_handle, Buf, NeedSize);
            if (ret == -1)
                return false;
            RealReadSize = ret;
            return true;
        }
        //-------------------------------------------------
        //д���ݵ��ļ�:������,ϣ��д��������
        bool write(const void *Buf, size_t Size)
        {
            rx_assert(m_handle != -1);
            ssize_t WNum = ::write(m_handle, Buf, Size);
            return (size_t)WNum == Size;
        }
        //-------------------------------------------------
        //��ѯ�ļ���С
        bool size(uint32_t &Ret)
        {
            struct stat st;
            if (fstat(m_handle, &st) < 0)
                return false;
            Ret = st.st_size;
            return true;
        }
        bool size(uint64_t &Ret)
        {
            struct stat st;
            if (fstat(m_handle, &st) < 0)
                return false;
            Ret = st.st_size;
            return true;
        }
        //-------------------------------------------------
        //�����ļ���дָ��ƫ��:orgΪ�������,0-�ļ�ͷ,1-�ļ���ǰλ��,2-�ļ�β
        bool seek(off_t pos, uint32_t org=0, uint32_t *ret=NULL)
        {
            rx_assert(m_handle != -1);
            uint32_t OM = 0;
            switch (org)
            {
            case 0:
                OM = SEEK_SET;
                break;
            case 1:
                OM = SEEK_CUR;
                break;
            case 2:
                OM = SEEK_END;
                break;
            default:
                rx_alert("�ļ�ָ��ƫ�����ʹ���");
                break;
            }
            off_t r = lseek(m_handle, pos, OM);
            if (ret) *ret = r;
            return r != -1;
        }
        //-------------------------------------------------
        //�õ���ǰ���ļ�ָ��λ��
        //����ֵ:�Ƿ��ȡ�ɹ�
        bool tell(uint32_t &ret)
        {
            if (m_handle == -1) return false;
            off_t r = lseek(m_handle, 0, SEEK_CUR);
            ret = r;
            return r != -1;
        }
        //-------------------------------------------------
        //�����ļ��ڵ�ǰ��д�㴦����
        bool truncate()
        {
            if (m_handle == -1) return false;
            uint32_t l;
            if (!tell(l)) return false;
            return ftruncate(m_handle,l) != -1;
        }
        //-------------------------------------------------
        //������д���浽����
        bool flush()
        {
            if (m_handle == -1) return false;
            return fsync(m_handle)!=-1;
        }
        //-------------------------------------------------
        //�����ļ�ʱ��
        bool time_modif(const time_t *dummy, const time_t* Modify = NULL, const time_t* Access = NULL)
        {
            struct timespec tims[2];
            memset(tims,0,sizeof(tims));

            if (Access == NULL || Modify == NULL)
            {
                if (!time_query(NULL,&tims[1].tv_sec, &tims[0].tv_sec))
                    return false;
            }

            if (Access) tims[0].tv_sec = *Access;
            if (Modify) tims[1].tv_sec = *Modify;

            return futimens(m_handle, tims) != -1;
        }
        //-------------------------------------------------
        //���µõ���ǰ�ļ���ʱ��
        bool time_query(time_t *Change, time_t* Modify = NULL, time_t* Access = NULL)
        {
            rx_assert(m_handle != -1);
            struct stat st;
            if (fstat(m_handle, &st) < 0)
                return false;

            if (Change) *Change=st.st_ctime;
            if (Modify) *Modify=st.st_mtime;
            if (Access) *Access=st.st_atime;
            return true;
        }
        //-------------------------------------------------
        //��ʽ����ӡ������ļ�
        template<uint32_t BuffSize>
        bool print(const char* Fmt, ...)
        {
            va_list	ap;
            va_start(ap, Fmt);
            char Buff[BuffSize];
            int R = st::vsnprintf(Buff, BuffSize, Fmt, ap);
            va_end(ap);
            if (R < 0)
                return false;
            return write(Buff, R);
        }
    };
}
#endif
namespace rx
{
    //---------------------------------------------------------
    //��������os_file_t����Ԥ���䴦��(�����ļ��ߴ�)
    inline error_code alloc_file_size(os_file_t& file, uint32_t NewSize)
    {
        uint32_t Pos;
        if (!file.tell(Pos))
            return error_code(ec_file_op,-1);
        if (!file.seek(NewSize))
            return error_code(ec_file_op,-2);
        if (!file.truncate())
            return error_code(ec_file_op,-3);
        if (!file.flush())
            return error_code(ec_file_op,-4);
        if (!file.seek(Pos))
            return error_code(ec_file_op,-5);
        return ec_ok;
    }
    //---------------------------------------------------------
    //����ָ�����ֵ��ļ�,��Ԥ����ָ���Ŀռ�
    inline error_code alloc_file_size(const char* file, uint32_t NewSize)
    {
        os_file_t f;
        error_code ec=f.open(file,"w+");
        if (ec)
            return ec;
        return alloc_file_size(f, NewSize);
    }
    //---------------------------------------------------------
    //�������ݵ��ļ�
    inline error_code save_to_file(const char* filename,const void* data,size_t datalen)
    {
        os_file_t f;
        error_code ec=f.open(filename, "w+");
        if (ec)
            return ec;

        if (!f.write(data, datalen))
            return ec_file_write;
        return ec_ok;
    }
    inline error_code save_to_file(const char* filename, const char* data) { return save_to_file(filename,data,strlen(data)); }
    //---------------------------------------------------------
    //���ļ�װ�����ݵ��ַ���
    inline error_code load_from_file(const char* filename, std::string &str)
    {
        os_file_t f;
        error_code ec=f.open(filename, "r");
        if (ec)
            return ec;

        uint32_t fsize;
        if (!f.size(fsize))
            return ec_file_op;

        try { str.resize(fsize); }
        catch (...) { return ec_mem_not_enough; }

        str.clear();

        char buff[512];
        while (fsize)
        {
            uint32_t ds;
            if (!f.read(buff, Min(fsize, (uint32_t)sizeof(buff)), ds))
                return ec_file_read;

            str.append(buff, ds);
            fsize -= ds;
        }
        return ec_ok;
    }
}
#endif

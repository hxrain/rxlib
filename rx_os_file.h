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
    //利用文件系统API封装文件操作功能类
    class os_file_t
    {
        HANDLE  m_handle;
    public:
        //-------------------------------------------------
        //文件时间与标准时间的相互转化
        static void  ft2t(const FILETIME &ft, time_t &t)
        {
            rx_static_assert(0x019DB1DED53E8000 == 116444736000000000);
            ULARGE_INTEGER I;
            I.LowPart = ft.dwLowDateTime;
            I.HighPart = ft.dwHighDateTime;
            //公元1601年到公元1970年相差369年，转化为秒数即为11644473600秒
            //ft是自UTC（Coordinated Universal Time）时间1601年1月1日开始的100纳秒为单位的时间
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
        //创建一个文件,write_over=false文件打开或创建;write_over=true文件覆盖或创建
        //flags默认打开为可读写;mode默认别人可以共享读,0为独占;文件属性默认为正常
        error_code create(const char* filename, bool write_over, uint32_t flags = GENERIC_READ | GENERIC_WRITE, uint32_t mode = FILE_SHARE_READ| FILE_SHARE_WRITE, uint32_t attrib = FILE_ATTRIBUTE_NORMAL)
        {
            if (is_empty(filename))
                return ec_in_param;
            m_handle = CreateFileA(filename, flags, mode, NULL, write_over ? CREATE_ALWAYS : OPEN_ALWAYS, attrib, NULL);
            return m_handle == INVALID_HANDLE_VALUE ? ec_file_open : ec_ok;
        }
        //-------------------------------------------------
        //打开一个文件,文字不存在就失败
        //flags默认打开为可读写;mode默认别人可以共享读,0为独占;文件属性默认为正常
        error_code open(const char* filename, uint32_t flags = GENERIC_READ | GENERIC_WRITE, uint32_t mode = FILE_SHARE_READ| FILE_SHARE_WRITE, uint32_t attrib = FILE_ATTRIBUTE_NORMAL)
        {
            if (is_empty(filename))
                return ec_in_param;
            m_handle = CreateFileA(filename, flags, mode, NULL, OPEN_EXISTING, attrib, NULL);
            return m_handle == INVALID_HANDLE_VALUE ? ec_file_open : ec_ok;
        }
        //-------------------------------------------------
        //仿std/fopen的文件打开操作
        error_code open(const char* filename, const char* Mode)
        {   //参照vc2008
            //r : read
            //w : write
            //a : append
            //r+: read/write
            //w+: open empty for read/write
            //a+: read/append

            //参照bcb6
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

            if (op_read)                                    //"r"模式,仅打开不创建
                return open(filename, flag_read | flag_write);
            else                                            //"w"模式,内容要被覆盖;"a"创建但不覆盖
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
        //读数据到缓冲区:缓冲区,希望读取的数量,实际读取的尺寸.
        //返回值:是否成功
        bool read(void *Buf, uint32_t NeedSize,uint32_t &RealReadedSize)
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;

            if (!ReadFile(m_handle, Buf, NeedSize, (DWORD*)&RealReadedSize, NULL))
                return false;
            return true;
        }
        //-------------------------------------------------
        //写数据到文件:缓冲区,希望写出的数量
        bool write(const void *Buf, size_t Size)
        {
            rx_assert(m_handle != INVALID_HANDLE_VALUE);
            DWORD WNum = 0;
            bool Ret = !!WriteFile(m_handle, Buf, (DWORD)Size, &WNum, NULL);
            return Ret&&WNum == Size;
        }
        //-------------------------------------------------
        //查询文件大小
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
        //设置文件读写指针偏移:org为起点类型,0-文件头,1-文件当前位置,2-文件尾
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
        //得到当前的文件指针位置
        //返回值:是否获取成功
        bool tell(uint32_t &ret)
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;
            ret=SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);
            return ret != (uint32_t)-1;
        }
        //-------------------------------------------------
        //设置文件在当前读写点处结束
        bool truncate()
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;
            return !!SetEndOfFile(m_handle);
        }
        //-------------------------------------------------
        //立即回写缓存到磁盘
        bool flush()
        {
            if (m_handle == INVALID_HANDLE_VALUE) return false;
            return !!FlushFileBuffers(m_handle);
        }
        //-------------------------------------------------
        //设置文件时间
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
        //更新得到当前文件的时间
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
        //格式化打印输出到文件
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
        O_RDONLY        只读模式
        O_WRONLY        只写模式
        O_RDWR          读写模式
        以上三者是互斥的，即不可以同时使用。
        O_APPEND        每次写操作都写入文件的末尾
        O_CREAT         如果指定文件不存在，则创建这个文件
        O_EXCL          如果要创建的文件已存在，则返回 -1，并且修改 errno 的值
        O_TRUNC         如果文件存在，并且以只写/读写方式打开，则清空文件全部内容
        O_NOCTTY        如果路径名指向终端设备，不要把这个设备用作控制终端。
        O_NONBLOCK      如果路径名指向 FIFO/块文件/字符文件，则把文件的打开和后继 I/O设置为非阻塞模式（nonblocking mode）。
        //以下用于同步输入输出
        O_DSYNC         等待物理 I/O 结束后再 write。在不影响读取新写入的数据的前提下，不等待文件属性更新。
        O_RSYNC         read 等待所有写入同一区域的写操作完成后再进行
        O_SYNC          等待物理 I/O 结束后再 write，包括更新文件属性的 I/O
    attrib:
        S_IRUSR:        读权限，文件属主
        S_IWUSR:        写权限，文件属主
        S_IXUSR:        执行权限，文件属主
        S_IRGRP:        读权限，文件所属组
        S_IWGRP:        写权限，文件所属组
        S_IXGRP:        执行权限，文件所属组
        S_IROTH:        读权限，其它用户
        S_IWOTH:        写权限，其它用户
        S_IXOTH:        执行权限，其它用户
    error:
        EEXIST          参数filename 所指的文件已存在，却使用了O_CREAT和O_EXCL旗标。
        EACCESS         参数filename所指的文件不符合所要求测试的权限。
        EROFS           欲测试写入权限的文件存在于只读文件系统内。
        EFAULT          参数filename指针超出可存取内存空间。
        EINVAL          参数mode 不正确。
        ENAMETOOLONG    参数filename太长。
        ENOTDIR         参数filename不是目录。
        ENOMEM          核心内存不足。
        ELOOP           参数filename有过多符号连接问题。
        EIO             I/O存取错误。
    */

    //-----------------------------------------------------
    //利用文件系统API封装文件操作功能类
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
        //打开一个文件,文字不存在就失败
        //flags默认打开为可读写;文件属性默认为用户读写
        error_code open(const char* filename, uint32_t flags = O_RDWR|O_CREAT, uint32_t attrib = S_IRUSR|S_IWUSR)
        {
            if (is_empty(filename))
                return ec_in_param;
            m_handle = ::open(filename,flags,attrib);
            return m_handle == -1 ? ec_file_open : ec_ok;
        }
        //-------------------------------------------------
        bool open(const char* filename, const char* Mode)
        {   //参照vc2008
            //r : read
            //w : write
            //a : append
            //r+: read/write
            //w+: open empty for read/write
            //a+: read/append

            //参照bcb6
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
        //读数据到缓冲区:缓冲区,希望读取的数量,实际读取的数量
        //返回值:是否成功
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
        //写数据到文件:缓冲区,希望写出的数量
        bool write(const void *Buf, size_t Size)
        {
            rx_assert(m_handle != -1);
            ssize_t WNum = ::write(m_handle, Buf, Size);
            return (size_t)WNum == Size;
        }
        //-------------------------------------------------
        //查询文件大小
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
        //设置文件读写指针偏移:org为起点类型,0-文件头,1-文件当前位置,2-文件尾
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
                rx_alert("文件指针偏移类型错误");
                break;
            }
            off_t r = lseek(m_handle, pos, OM);
            if (ret) *ret = r;
            return r != -1;
        }
        //-------------------------------------------------
        //得到当前的文件指针位置
        //返回值:是否获取成功
        bool tell(uint32_t &ret)
        {
            if (m_handle == -1) return false;
            off_t r = lseek(m_handle, 0, SEEK_CUR);
            ret = r;
            return r != -1;
        }
        //-------------------------------------------------
        //设置文件在当前读写点处结束
        bool truncate()
        {
            if (m_handle == -1) return false;
            uint32_t l;
            if (!tell(l)) return false;
            return ftruncate(m_handle,l) != -1;
        }
        //-------------------------------------------------
        //立即回写缓存到磁盘
        bool flush()
        {
            if (m_handle == -1) return false;
            return fsync(m_handle)!=-1;
        }
        //-------------------------------------------------
        //设置文件时间
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
        //更新得到当前文件的时间
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
        //格式化打印输出到文件
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
    //将给定的os_file_t进行预分配处理(调整文件尺寸)
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
    //产生指定名字的文件,并预分配指定的空间
    inline error_code alloc_file_size(const char* file, uint32_t NewSize)
    {
        os_file_t f;
        error_code ec=f.open(file,"w+");
        if (ec)
            return ec;
        return alloc_file_size(f, NewSize);
    }
    //---------------------------------------------------------
    //保存数据到文件
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
    //从文件装载内容到字符串
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

﻿#ifndef _RX_OS_LOCKER_H_
#define _RX_OS_LOCKER_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_lock_base.h"



#define rx_sleep rx_thread_yield							//sleep(毫秒)
inline void rx_thread_yield(uint32_t ms);					//让线程休眠指定的时间(毫秒)
inline bool rx_thread_yield();								//让当前线程让步给本核上的其他线程,返回值告知是否切换成功
inline void rx_thread_yield_us(uint32_t us);				//尝试进行微秒级休眠(win上不准确)

class locker_t;												//进程内可递归锁
class rw_locker_t;											//进程内不可递归读写锁

#if RX_OS_POSIX
    #include <pthread.h>									//-lpthread 需要链接库
    #include <unistd.h>

    //让线程休眠指定的时间(毫秒)
    inline void rx_thread_yield(uint32_t ms)
    {
        if (!ms)
            sched_yield();
        else
            usleep(ms*1000);
    }
    //尝试进行微秒级休眠
    inline void rx_thread_yield_us(uint32_t us)
    {
        if (!us)
            sched_yield();
        else
            usleep(us);
    }
    //让当前线程让步给本核上的其他线程,返回值告知是否切换成功
    inline bool rx_thread_yield() { return !sched_yield(); }

#elif RX_IS_OS_WIN

    #include <process.h>
    //让线程休眠指定的时间(毫秒)
    inline void rx_thread_yield(uint32_t ms) { Sleep(ms); }
    //尝试进行微秒级休眠(win上不准确)
    inline void rx_thread_yield_us(uint32_t us)
    {
        Sleep((us+999)/1000);
    }
    //让当前线程让步给本核上的其他线程,返回值告知是否切换成功
    inline bool rx_thread_yield() {return !!SwitchToThread();}

#endif


#if RX_OS_POSIX
namespace rx
{
    //------------------------------------------------------
    //封装一个进程内的递归锁
    class locker_t:public lock_i
    {
        class lock_attr
        {
            pthread_mutexattr_t m_attr;
            bool m_is_valid;
        public:
            lock_attr(){m_is_valid=(pthread_mutexattr_init(&m_attr)==0);}
            bool operator()(){return m_is_valid;}
            bool set_private(){return 0==pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_PRIVATE);}
            bool set_recursive(){return 0==pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);}
            pthread_mutexattr_t& attr(){return m_attr;}
            ~lock_attr()
            {
                if (!m_is_valid) return;
                m_is_valid=false;
                pthread_mutexattr_destroy(&m_attr);
            }
        };

        //--------------------------------------------------
        //进行递归锁的初始化,返回值0成功.
        int m_init(bool is_recursive)
        {
            //初始化互斥锁的属性对象
            lock_attr la;
            if (!la()) return -1;
            if (!la.set_private()) return -2;
            if (is_recursive&&!la.set_recursive()) return -3;
            //初始化互斥锁,应用锁属性
            return pthread_mutex_init(&m_handle,&la.attr());
        }
        //--------------------------------------------------
        //进行递归锁的解除
        void m_uninit()
        {
            pthread_mutex_destroy(&m_handle);
        }
        //--------------------------------------------------
        pthread_mutex_t     m_handle;
    public:
        //--------------------------------------------------
        //构造函数,默认进行初始化
        locker_t(bool is_recursive=true){m_init(is_recursive);}
        ~locker_t(){m_uninit();}
        pthread_mutex_t *handle(){return &m_handle;}
        //--------------------------------------------------
        //锁定
        bool lock(bool is_wr_lock=true){return pthread_mutex_lock(&m_handle)==0;}
        //--------------------------------------------------
        //解锁
        bool unlock(){return pthread_mutex_unlock(&m_handle)==0;}
        //--------------------------------------------------
        //尝试锁定
        bool trylock(bool is_wr_lock=true){return pthread_mutex_trylock(&m_handle)==0;}
        //--------------------------------------------------
    };

    //------------------------------------------------------
    //封装进程内的读写锁(非递归)
    class rw_locker_t:public lock_i
    {
        class lock_attr
        {
            pthread_rwlockattr_t m_attr;
            bool m_is_valid;
        public:
            lock_attr(){m_is_valid=(pthread_rwlockattr_init(&m_attr)==0);}
            bool operator()(){return m_is_valid;}
            bool set_private(){return 0==pthread_rwlockattr_setpshared(&m_attr, PTHREAD_PROCESS_PRIVATE);}
            bool set_prefer_write(){return 0==pthread_rwlockattr_setkind_np(&m_attr, PTHREAD_MUTEX_RECURSIVE);}
            pthread_rwlockattr_t& attr(){return m_attr;}
            ~lock_attr()
            {
                if (!m_is_valid) return;
                m_is_valid=false;
                pthread_rwlockattr_destroy(&m_attr);
            }
        };

        //--------------------------------------------------
        //进行锁的初始化,返回值0成功.
        int m_init()
        {
            //初始化互斥锁的属性对象
            lock_attr la;
            if (!la()) return -1;
            if (!la.set_private()) return -2;
            if (!la.set_prefer_write()) return -3;
            //初始化互斥锁,应用锁属性
            return pthread_rwlock_init(&m_handle,&la.attr());
        }
        //--------------------------------------------------
        //进行递归锁的解除
        void m_uninit()
        {
            pthread_rwlock_destroy(&m_handle);
        }
        //--------------------------------------------------
        pthread_rwlock_t     m_handle;
    public:
        //--------------------------------------------------
        //构造函数,默认进行初始化
        rw_locker_t(){m_init();}
        ~rw_locker_t(){m_uninit();}
        pthread_rwlock_t *handle(){return &m_handle;}
        //--------------------------------------------------
        //锁定,默认情况下使用写锁
        bool lock(bool is_wr_lock=true)
        {
            if (is_wr_lock)
                return pthread_rwlock_wrlock(&m_handle)==0;
            else
                return pthread_rwlock_rdlock(&m_handle)==0;
        }
        //--------------------------------------------------
        //解锁
        bool unlock(){return pthread_rwlock_unlock(&m_handle)==0;}
        //--------------------------------------------------
        //尝试锁定
        bool trylock(bool is_wr_lock=true)
        {
            if (is_wr_lock)
                return pthread_rwlock_trywrlock(&m_handle)==0;
            else
                return pthread_rwlock_tryrdlock(&m_handle)==0;
        }
        //--------------------------------------------------
    };
}
#elif RX_IS_OS_WIN
namespace rx
{
    //------------------------------------------------------
    //封装一个进程内的递归锁
    class locker_t :public lock_i
    {
        //--------------------------------------------------
        //进行递归锁的初始化,返回值0成功.
        int m_init(DWORD spinCount = 1000)
        {
            //初始化互斥锁,应用锁属性
            if (!InitializeCriticalSectionAndSpinCount(&m_handle, spinCount))
                return -1;

            return 0;
        }
        //--------------------------------------------------
        //进行递归锁的解除
        void m_uninit()
        {
            DeleteCriticalSection(&m_handle);
        }
        //--------------------------------------------------
        CRITICAL_SECTION     m_handle;
    public:
        //--------------------------------------------------
        //构造函数,默认进行初始化
        locker_t(bool is_recursive = true) { m_init(); }
        ~locker_t() { m_uninit(); }
        CRITICAL_SECTION *handle() { return &m_handle; }
        //--------------------------------------------------
        //锁定
        bool lock(bool is_wr_lock = true) { EnterCriticalSection(&m_handle); return true; }
        //--------------------------------------------------
        //解锁
        bool unlock() { LeaveCriticalSection(&m_handle); return true; }
        //--------------------------------------------------
        //尝试锁定
        bool trylock(bool is_wr_lock = true) { return !!TryEnterCriticalSection(&m_handle); }
        //--------------------------------------------------
    };

    //------------------------------------------------------
    //封装进程内的读写锁(非递归)
    class rw_locker_t :public lock_i
    {
        //--------------------------------------------------
        //进行锁的初始化,返回值0成功.
        int m_init()
        {
            InitializeSRWLock(&m_handle);
            return 0;
        }
        //--------------------------------------------------
        SRWLOCK     m_handle;
        bool        m_is_write;
    public:
        //--------------------------------------------------
        //构造函数,默认进行初始化
        rw_locker_t() { m_init(); }
        ~rw_locker_t() {}
        SRWLOCK *handle() { return &m_handle; }
        //--------------------------------------------------
        //锁定,默认情况下使用写锁
        bool lock(bool is_wr_lock = true)
        {
            if (is_wr_lock)
                ::AcquireSRWLockExclusive(&m_handle);
            else
                AcquireSRWLockShared(&m_handle);
            m_is_write = is_wr_lock;
            return true;
        }
        //--------------------------------------------------
        //解锁
        bool unlock()
        {
            if (m_is_write)
                ReleaseSRWLockExclusive(&m_handle);
            else
                ReleaseSRWLockShared(&m_handle);
            return true;
        }
        //--------------------------------------------------
        //尝试锁定
        bool trylock(bool is_wr_lock = true)
        {
            if (is_wr_lock)
            {
                if (!TryAcquireSRWLockExclusive(&m_handle))
                    return false;
            }
            else
            {
                if (!TryAcquireSRWLockShared(&m_handle))
                    return false;
            }

            m_is_write = is_wr_lock;
            return true;
        }
        //--------------------------------------------------
    };
}
#endif

#endif

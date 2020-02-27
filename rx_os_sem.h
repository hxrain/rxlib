#ifndef _RX_OS_SEM_H_
#define _RX_OS_SEM_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include <limits.h>

#if RX_OS_POSIX
#include <pthread.h>
#include <semaphore.h>
#include <memory.h>
#include <errno.h>
//-lpthread
#endif


namespace rx
{
    #if RX_OS_POSIX
    //---------------------------------------------------------
    //��SEM���/�ź������ж��󻯹��ܷ�װ
    class semp_t
    {
        sem_t   m_handle;
    public:
        //-----------------------------------------------------
        semp_t() { memset(&m_handle, 0, sizeof(m_handle)); }
        semp_t(uint32_t init_value) { memset(&m_handle, 0, sizeof(m_handle)); init(init_value); }
        ~semp_t() { uninit(); }
        //-----------------------------------------------------
        //��ʼ�����,��֪��ʼֵ
        bool init(uint32_t init_value = 0) { return 0 == sem_init(&m_handle, 0, init_value); }
        //-----------------------------------------------------
        //�������ʼ��
        void uninit() { sem_destroy(&m_handle); }
        //-----------------------------------------------------
        //��ȡ����,�ɽ��г�ʱ�ȴ�(Ĭ�����޵ȴ�)
        //����ֵ:�Ƿ���ȷȡ���˼���
        bool take(int wait_ms = -1)
        {
            if (wait_ms == 0)
                return 0 == sem_trywait(&m_handle);
            else if (wait_ms == -1)
                return 0 == sem_wait(&m_handle);
            else
            {
                //ʹ��ԭ����ʱ�ȴ����ױ�ϵͳʱ��ĵ�����������,����ʹ�ö��ѭ��ģ��(��̫��׼)
                for (int i = 0; i < wait_ms; ++i)
                {
                    if (0 == sem_trywait(&m_handle))
                        return true;
                    else if (EINVAL == errno)
                        return false;
                    usleep(1000);
                }
                return false;
            }
        }
        //-----------------------------------------------------
        //�ύ����,�ɽ���Է��ĵȴ�
        bool post()
        {
            return 0 == sem_post(&m_handle);
        }
    };
    #elif RX_IS_OS_WIN
    //---------------------------------------------------------
    //��SEM���/�ź������ж��󻯹��ܷ�װ
    class semp_t
    {
        HANDLE      m_handle;
    public:
        //-----------------------------------------------------
        semp_t():m_handle(NULL) {}
        semp_t(uint32_t init_value) :m_handle(NULL) {init(init_value); }
        ~semp_t() { uninit(); }
        //-----------------------------------------------------
        //��ʼ�����,��֪��ʼֵ
        bool init(uint32_t init_value = 0)
        {
            rx_assert(m_handle == NULL);
            m_handle=CreateSemaphore(NULL,(long)init_value,LONG_MAX,NULL);
            return NULL != m_handle;
        }
        //-----------------------------------------------------
        //�������ʼ��
        void uninit() {CloseHandle(m_handle); m_handle= NULL;}
        //-----------------------------------------------------
        //��ȡ����,�ɽ��г�ʱ�ȴ�(Ĭ�����޵ȴ�)
        //����ֵ:�Ƿ���ȷȡ���˼���
        bool take(int wait_ms = -1)
        {
            if (wait_ms == 0)
                return WAIT_OBJECT_0 == WaitForSingleObject(m_handle,0);
            else if (wait_ms == -1)
                return WAIT_OBJECT_0 == WaitForSingleObject(m_handle, INFINITE);
            else
                return WAIT_OBJECT_0 == WaitForSingleObject(m_handle, wait_ms);
        }
        //-----------------------------------------------------
        //�ύ����,�ɽ���Է��ĵȴ�
        bool post()
        {
            return !!ReleaseSemaphore(m_handle,1,NULL);
        }
    };
    #endif
}
#endif // _HX_SEM_H_

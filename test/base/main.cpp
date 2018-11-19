#define RX_USE_TDD_TICK 1

#include "../../rx_dtl_array.h"

//���ո�Ч��gbk/unicode�ַ���ת������
#include "../ut_str_uni2gbk.h"
#include "../../rx_str_cs_gbk2uni.h"
#include "../../rx_str_cs_uni2gbk.h"
//---------------------------------------------------------
//key����bit�ֶεĻ�����
#include "../ut_dtl_radix_bit.h"
#include "../../rx_dtl_raxbit_map.h"
#include "../../rx_dtl_raxbit_set.h"
#include "../../rx_dtl_raxbit_raw.h"
//---------------------------------------------------------
//��������Ĺ�ϣӳ������,��������
#include "../ut_dtl_hashmap.h"
#include "../../rx_dtl_hashmap.h"
//---------------------------------------------------------
//�����ڴ��ļ�ӳ��Ķ�����ϣ��,����Ϊ�������־û�k/v���ݿ�
#include "../ut_ext_hashtbl_mm.h"
#include "../../rx_ext_hashtbl_mm.h"
//---------------------------------------------------------
//ͳһ�Ĵ����붨��
#include "../../rx_ct_error.h"
//---------------------------------------------------------
//ϵͳ�ļ��������ܷ�װ
#include "../ut_os_file.h"
#include "../../rx_os_file.h"
//---------------------------------------------------------
//�ļ��ڴ�ӳ�书�ܷ�װ
#include "../ut_os_mmap.h"
#include "../../rx_os_mmap.h"
//---------------------------------------------------------
//ϵͳ�����
#include "../../rx_os_misc.h"
//---------------------------------------------------------
//ģ��c++11��typeof��auto�Ĺ��ܷ�װ
#include "../../rx_ct_typeof.h"
//---------------------------------------------------------
//����ʱ���ֵĶ�ʱ�����ܿ��
#include "../ut_timer_tw.h"
#include "../../rx_timer_tw.h"
//---------------------------------------------------------
//���״̬�����ܷ�װ
#include "../ut_hsm_core.h"
#include "../../rx_hsm_core.h"
//---------------------------------------------------------
//����/ջ/���еĻ����������ܷ�װ
#include "../ut_dtl_list.h"
#include "../../rx_dtl_list.h"
#include "../../rx_dtl_stack.h"
#include "../../rx_dtl_queue.h"
//---------------------------------------------------------
//��ϣ���������Լ�std::map�����ĶԱȲ���
#include "../ut_dtl_htbl_sklist_map.h"
//---------------------------------------------------------
//�������������
#include "../ut_dtl_skiplist.h"
#include "../../rx_dtl_skipset.h"
#include "../../rx_dtl_skiplist.h"
#include "../../rx_dtl_skiplist_raw.h"
//---------------------------------------------------------
//��ϣ�����ϣ������ع���
#include "../ut_dtl_hashtbl_tiny.h"
#include "../../rx_dtl_hashtbl_ex.h"
#include "../../rx_dtl_hashset.h"
#include "../../rx_dtl_hashlink.h"
#include "../../rx_dtl_hashtbl.h"
#include "../../rx_dtl_hashtbl_raw.h"
//---------------------------------------------------------
//�ַ�����ع����뼫С�ַ������ܷ�װ,������������ʹ��
#include "../ut_str_util.h"
#include "../../rx_str_tiny.h"
#include "../../rx_str_util.h"
//---------------------------------------------------------
//�������������м�ʱ����
#include "../../rx_tdd_tick.h"
//---------------------------------------------------------
//����bipbuffer�㷨�Ļ��λ�����
#include "../ut_dtl_ringbuff.h"
#include "../../rx_dtl_ringbuff.h"
//---------------------------------------------------------
//���ζ���
#include "../ut_dtl_ringqueue.h"
#include "../../rx_dtl_ringqueue.h"
//---------------------------------------------------------
//�����߳���ͬ��ԭ���������������ܷ�װ
#include "../ut_os_tasks.h"
#include "../../rx_os_tasks.h"
//---------------------------------------------------------
//ϵͳ�̹߳��ܷ�װ
#include "../../rx_os_thread.h"
//---------------------------------------------------------
//ϵͳ��������ͬ��ԭ��
#include "../../rx_os_cond.h"
//---------------------------------------------------------
//ϵͳ�ź���/���ͬ��ԭ��
#include "../../rx_os_sem.h"
//---------------------------------------------------------
//ϵͳͬ���߳���������
#include "../ut_os_lock.h"
#include "../../rx_os_lock.h"
#include "../../rx_os_spinlock.h"
//---------------------------------------------------------
//two level bitmap
#include "../ut_hash_tlmap.h"
#include "../../rx_hash_tlmap.h"
//---------------------------------------------------------
//������ϣ�����ݿ��ϣ
#include "../ut_hash.h"
#include "../../rx_hash_rand.h"
#include "../../rx_hash_data.h"
#include "../../rx_hash_int.h"
//---------------------------------------------------------
//bitλ����
#include "../ut_bits_op.h"
#include "../../rx_ct_bitop.h"
#include "../../rx_ct_bitarr.h"
//---------------------------------------------------------
//�ڴ�������ӿ����ڴ��������
#include "../ut_mem_alloc.h"
#include "../../rx_mem_alloc_cntr.h"
#include "../../rx_mem_alloc.h"
//---------------------------------------------------------
//�̶��ڴ�鷶Χ���ڴ������
#include "../ut_mem_pool_h4.h"
#include "../../rx_mem_pool_h4.h"
//---------------------------------------------------------
//�ڴ������
#include "../ut_mem_pool_cntr.h"
#include "../../rx_mem_pool_cntr.h"
//---------------------------------------------------------
//�ڴ�ػ�������
#include "../ut_mem_pool.h"
#include "../../rx_mem_pool_fx1.h"
#include "../../rx_dtl_list_raw.h"
#include "../../rx_mem_pool.h"
//---------------------------------------------------------
//�����ڹ���:������ȡ/�����ڼ���/��ί��
#include "../ut_ct_obj.h"
#include "../../rx_ct_delegate.h"
#include "../../rx_ct_util.h"
#include "../../rx_ct_traits.h"
//---------------------------------------------------------
//ϵͳʱ�����
#include "../ut_datetime.h"
#include "../../rx_datetime.h"
#include "../../rx_datetime_ex.h"
//---------------------------------------------------------
//ԭ�ӱ���
#include "../ut_atomic.h"
#include "../../rx_ct_atomic.h"
//---------------------------------------------------------
//ԭ�Ӳ���
#include "../ut_cc_atomic.h"
#include "../../rx_cc_atomic.h"
//---------------------------------------------------------
//������������/��Ԫ���Կ��
#include "../../rx_tdd.h"
//---------------------------------------------------------
//���Դ���
#include "../../rx_assert.h"
//---------------------------------------------------------
//������Ԥ����깦�ܷ�װ
#include "../ut_cc_macro.h"
#include "../../rx_cc_macro.h"

//---------------------------------------------------------
#include <stdio.h>
#include <iostream>
//---------------------------------------------------------


int main()
{
    std::cout << "rxlib unit test!���!Hello world!\n" << rx_cc_desc() << std::endl << std::endl;

    rx_static_assert(1);

    rx_tdd_run(tdd_level_std);

    getchar();
    return 0;
}

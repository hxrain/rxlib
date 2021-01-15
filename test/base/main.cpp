#define RX_USE_TDD_TICK 1

#define UT_ALL 1
#define UT_SEL 42

//---------------------------------------------------------
#if UT_ALL||UT_SEL==42
    //���������ղ��ұ�
	#include "../ut_dtl_hat.h"
	#include "../../rx_dtl_hat_map.h"
    #include "../../rx_dtl_hat_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==41
    //TCP���繦��
    #include "../ut_net_tcp.h"
    #include "../../rx_net_tcp_svrsock.h"
    #include "../../rx_net_tcp_client.h"
    #include "../../rx_net_tcp_server.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==40
    //��־��¼���������
    #include "../ut_logger_std.h"
    #include "../../rx_logger_std.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==40
    //��׼��ͬ��socket���ܷ�װ
	#include "../ut_net_sock_std.h"
    #include "../../rx_net_sock_std.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==40||UT_SEL==41
    //ʹ�����繦��,��Ҫ��ʼ��socket����
    rx::socket_env_t G_sock_env;
    #if RX_CC==RX_CC_VC
        #pragma comment(lib,"ws2_32.lib")
    #endif
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==39
	//�򵥵Ļ��������ܷ�װ
	#include "../../rx_dtl_buff.h"

	//�������������㷨
	#include "../ut_dtl_alg.h"
    #include "../../rx_dtl_alg.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==38
    //��װ�������������
    #include "../ut_dtl_array.h"
    #include "../../rx_dtl_array.h"
    #include "../../rx_dtl_array_ex.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==37
    //���ո�Ч��gbk/unicode�ַ���ת������
    #include "../ut_str_uni2gbk.h"
    #include "../../rx_ucs2_gbk2uni.h"
    #include "../../rx_ucs2_uni2gbk.h"
    #include "../../rx_ucs2_rawgbk.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==36
    //ucs�ַ��������,�ж��ַ�����,ȫ���ת��,unicode�ֿ�ȵ�.
    #include "../ut_ucs_misc.h"
    #include "../../rx_ucs2_misc.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==35
    //key����bit�ֶεĻ�����
    #include "../ut_dtl_radix_bit.h"
    #include "../../rx_dtl_raxbit_map.h"
    #include "../../rx_dtl_raxbit_set.h"
    #include "../../rx_dtl_raxbit_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==34
    //��������Ĺ�ϣӳ������,��������
    #include "../ut_dtl_hashmap.h"
    #include "../../rx_dtl_hashmap.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==33
    //�����ڴ��ļ�ӳ��Ķ�����ϣ��,����Ϊ�������־û�k/v���ݿ�
    #include "../ut_ext_hashtbl_mm.h"
    #include "../../rx_ext_hashtbl_mm.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==32
    //ͳһ�Ĵ����붨��
    #include "../../rx_ct_error.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==31
    //ϵͳ�ļ��������ܷ�װ
    #include "../ut_os_file.h"
    #include "../../rx_os_file.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==30
    //�ļ��ڴ�ӳ�书�ܷ�װ
    #include "../ut_os_mmap.h"
    #include "../../rx_os_mmap.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==29
    //ϵͳ�����
    #include "../../rx_os_misc.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==28
    //ģ��c++11��typeof��auto�Ĺ��ܷ�װ
    #include "../../rx_ct_typeof.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==27
    //����ʱ���ֵĶ�ʱ�����ܿ��
    #include "../ut_timer_tw.h"
    #include "../../rx_timer_tw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==26
    //���״̬�����ܷ�װ
    #include "../ut_hsm_core.h"
    #include "../../rx_hsm_core.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==25
    //����/ջ/���еĻ����������ܷ�װ
    #include "../ut_dtl_list.h"
    #include "../../rx_dtl_list.h"
    #include "../../rx_dtl_stack.h"
    #include "../../rx_dtl_queue.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==24
    //��ϣ���������Լ�std::map�����ĶԱȲ���
    #include "../ut_dtl_htbl_sklist_map.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==23
    //�������������
    #include "../ut_dtl_skiplist.h"
    #include "../../rx_dtl_skipset.h"
    #include "../../rx_dtl_skiplist.h"
    #include "../../rx_dtl_skiplist_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==22
    //��ϣ�����ϣ������ع���
    #include "../ut_dtl_hashtbl_tiny.h"
    #include "../../rx_dtl_hashtbl_ex.h"
    #include "../../rx_dtl_hashset.h"
    #include "../../rx_dtl_hashlink.h"
    #include "../../rx_dtl_hashtbl.h"
    #include "../../rx_dtl_hashtbl_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==21
    //�ַ�����ع����뼫С�ַ������ܷ�װ,������������ʹ��
    #include "../ut_str_util.h"
    #include "../../rx_str_tiny.h"
    #include "../../rx_str_util_std.h"
    #include "../../rx_str_util_ext.h"
    #include "../../rx_str_util_bin.h"
    #include "../../rx_str_util_misc.h"
#endif
#if UT_ALL||UT_SEL==20
    //���ⲿ�������ַ�����ʽ�����ܷ�װ
    #include "../ut_str_util_fmt.h"
    #include "../../rx_str_util_fmt.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==19
    //�������������м�ʱ����
    #include "../../rx_tdd_tick.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==18
    //����bipbuffer�㷨�Ļ��λ�����
    #include "../ut_dtl_ringbuff.h"
    #include "../../rx_dtl_ringbuff.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==17
    //���ζ���
    #include "../ut_dtl_ringqueue.h"
    #include "../../rx_dtl_ringqueue.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==16
    //�����߳���ͬ��ԭ���������������ܷ�װ
    #include "../ut_os_tasks.h"
    #include "../../rx_os_tasks.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==15
    //ϵͳ�̹߳��ܷ�װ
    #include "../ut_os_thread.h"
    #include "../../rx_os_thread.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==14
    //ϵͳ��������ͬ��ԭ��
    #include "../../rx_os_cond.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==13
    //ϵͳ�ź���/���ͬ��ԭ��
    #include "../../rx_os_sem.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==12
    //ϵͳͬ���߳���������
    #include "../ut_os_lock.h"
	#include "../../rx_lock_base.h"
	#include "../../rx_lock_os.h"
    #include "../../rx_lock_spin.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==11
    //two level bitmap
    #include "../ut_hash_tlmap.h"
    #include "../../rx_hash_tlmap.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==10
    //������ϣ�����ݿ��ϣ
    #include "../ut_hash.h"
    #include "../../rx_hash_rand.h"
    #include "../../rx_hash_data.h"
    #include "../../rx_hash_int.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==9
    //bitλ����
    #include "../ut_bits_op.h"
    #include "../../rx_ct_bitop.h"
    #include "../../rx_ct_bitarr.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==8
    //�ڴ�������ӿ����ڴ��������
    #include "../ut_mem_alloc.h"
    #include "../../rx_mem_alloc_base.h"
	#include "../../rx_mem_alloc.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==7
    //�̶��ڴ�鷶Χ���ڴ������
    #include "../ut_mem_pool_h4.h"
    #include "../../rx_mem_pool_h4.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==6
    //�ڴ������
    #include "../ut_mem_pool_cntr.h"
    #include "../../rx_mem_pool_cntr.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==5
    //�ڴ�ػ�������
    #include "../ut_mem_pool.h"
    #include "../../rx_mem_pool_fx1.h"
    #include "../../rx_dtl_list_raw.h"
    #include "../../rx_mem_pool.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==4
    //�����ڹ���:������ȡ/�����ڼ���/��ί��
    #include "../ut_ct_obj.h"
    #include "../../rx_ct_delegate.h"
    #include "../../rx_ct_util.h"
    #include "../../rx_ct_traits.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==3
    //ϵͳʱ�����
    #include "../ut_datetime.h"
    #include "../../rx_datetime.h"
    #include "../../rx_datetime_ex.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==2
    //ԭ�ӱ���
    #include "../ut_atomic.h"
    #include "../../rx_ct_atomic.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==1
    //ԭ�Ӳ���
    #include "../ut_cc_atomic.h"
    #include "../../rx_cc_atomic.h"
#endif

//---------------------------------------------------------
//������������/��Ԫ���Կ��
#include "../../rx_tdd.h"
//---------------------------------------------------------
//���û�������
#include "../../rx_cc_base.h"
//---------------------------------------------------------
//���Թ���
#include "../../rx_assert.h"
//---------------------------------------------------------
//������Ԥ�����
#include "../ut_cc_macro.h"
#include "../../rx_cc_macro.h"
//---------------------------------------------------------

int main()
{
    return rx_tdd_run(tdd_level_std);
}

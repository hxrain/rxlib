#ifndef _H_RX_CT_TRAITS_H_
#define _H_RX_CT_TRAITS_H_
/*
������ģ��������ȡ����Ĺ���
*/
#include "rx_cc_macro.h"
namespace rx
{
	//----------------------------------------------------------
	struct ct_type_true_t {};
	struct ct_type_false_t {};

	//���ͼ��ģ��,���ڷ�������鹹�캯������
	template <class type>
	struct ct_type_check_t
	{
		typedef ct_type_true_t     this_dummy_member_must_be_first;
		typedef ct_type_false_t    has_trivial_default_constructor;
		typedef ct_type_false_t    has_trivial_copy_constructor;
		typedef ct_type_false_t    has_trivial_assignment_operator;
		typedef ct_type_false_t    has_trivial_destructor;
		typedef ct_type_false_t    is_POD_type;
	};

	template<> struct ct_type_check_t<char>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;
	};

	template<> struct ct_type_check_t<int8_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint8_t unsigned_t;
		typedef int8_t  signed_t;
		typedef uint8_t invsigned_t;
	};

	template<> struct ct_type_check_t<uint8_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint8_t unsigned_t;
		typedef int8_t  signed_t;
		typedef int8_t  invsigned_t;
	};

	template<> struct ct_type_check_t<int16_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint16_t unsigned_t;
		typedef int16_t  signed_t;
		typedef uint16_t invsigned_t;
	};

	template<> struct ct_type_check_t<uint16_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint16_t unsigned_t;
		typedef int16_t  signed_t;
		typedef int16_t  invsigned_t;
	};

	template<> struct ct_type_check_t<int32_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint32_t unsigned_t;
		typedef int32_t  signed_t;
		typedef uint32_t invsigned_t;
	};

	template<> struct ct_type_check_t<uint32_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint32_t unsigned_t;
		typedef int32_t  signed_t;
		typedef int32_t  invsigned_t;
	};

	template<> struct ct_type_check_t<int64_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint64_t unsigned_t;
		typedef int64_t  signed_t;
		typedef uint64_t invsigned_t;
	};

	template<> struct ct_type_check_t<uint64_t>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;

		typedef uint64_t unsigned_t;
		typedef int64_t  signed_t;
		typedef int64_t  invsigned_t;
	};

	template<> struct ct_type_check_t<float>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;
	};

	template<> struct ct_type_check_t<double>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;
	};

	template<> struct ct_type_check_t<long double>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;
	};

	template <class T>
	struct ct_type_check_t<T*>
	{
		typedef ct_type_true_t    has_trivial_default_constructor;
		typedef ct_type_true_t    has_trivial_copy_constructor;
		typedef ct_type_true_t    has_trivial_assignment_operator;
		typedef ct_type_true_t    has_trivial_destructor;
		typedef ct_type_true_t    is_POD_type;
	};

	//----------------------------------------------------------
	/*
	������ȡ����ģ��ct_type_pick_t�ܹ��õ����������µĸ�������
	base_�����������  /  org_���������ԭ����  /  const_����const����  /  not_const_t�����const��ԭ��������  /  const_t����const���ԭ��������

	T=C             val_t=C    ptr_t=C*   ref_t=C&
	T=C*            val_t=C    ptr_t=C*   ref_t=C&
	T=C**           val_t=C*   ptr_t=C**  ref_t=C* &

	T=const C       val_t=C    ptr_t=C*   ref_t=C&
	T=const C*      val_t=C    ptr_t=C*   ref_t=C&
	T=const C**     val_t=C*   ptr_t=C**  ref_t=C* &
	*/
	template <class T>
	struct ct_type_pick_t
	{
		typedef T                          base_val_t;
		typedef T*                         base_ptr_t;
		typedef T&                         base_ref_t;
		typedef T                          org_val_t;
		typedef T*                         org_ptr_t;
		typedef T&                         org_ref_t;
		typedef const T                    const_val_t;
		typedef const T*                   const_ptr_t;
		typedef const T&                   const_ref_t;
		typedef T                          not_const_t;
		typedef const T                    const_t;
	};


	template <class T>
	struct ct_type_pick_t<const T>
	{
		typedef T                          base_val_t;
		typedef T*                         base_ptr_t;
		typedef T&                         base_ref_t;
		typedef const T                    org_val_t;
		typedef const T*                   org_ptr_t;
		typedef const T&                   org_ref_t;
		typedef const T                    const_val_t;
		typedef const T*                   const_ptr_t;
		typedef const T&                   const_ref_t;
		typedef T                          not_const_t;
		typedef const T                    const_t;
	};


	template <class T>
	struct ct_type_pick_t<T*>
	{
		typedef T                          base_val_t;
		typedef T*                         base_ptr_t;
		typedef T&                         base_ref_t;
		typedef T                          org_val_t;
		typedef T*                         org_ptr_t;
		typedef T&                         org_ref_t;
		typedef const T                    const_val_t;
		typedef const T*                   const_ptr_t;
		typedef const T&                   const_ref_t;
		typedef T*                         not_const_t;
		typedef const T*                   const_t;
	};


	template <class T>
	struct ct_type_pick_t<const T*>
	{
		typedef T                          base_val_t;
		typedef T*                         base_ptr_t;
		typedef T&                         base_ref_t;
		typedef const T                    org_val_t;
		typedef const T*                   org_ptr_t;
		typedef const T&                   org_ref_t;
		typedef const T                    const_val_t;
		typedef const T*                   const_ptr_t;
		typedef const T&                   const_ref_t;
		typedef T*                         not_const_t;
		typedef const T*                   const_t;
	};

	template <class T>
	struct ct_type_pick_t<T&>
	{
		typedef T                          base_val_t;
		typedef T*                         base_ptr_t;
		typedef T&                         base_ref_t;
		typedef T                          org_val_t;
		typedef T*                         org_ptr_t;
		typedef T&                         org_ref_t;
		typedef const T                    const_val_t;
		typedef const T*                   const_ptr_t;
		typedef const T&                   const_ref_t;
		typedef T&                         not_const_t;
		typedef const T&                   const_t;
	};


	template <class T>
	struct ct_type_pick_t<const T&>
	{
		typedef T                          base_val_t;
		typedef T*                         base_ptr_t;
		typedef T&                         base_ref_t;
		typedef const T                    org_val_t;
		typedef const T*                   org_ptr_t;
		typedef const T&                   org_ref_t;
		typedef const T                    const_val_t;
		typedef const T*                   const_ptr_t;
		typedef const T&                   const_ref_t;
		typedef T&                         not_const_t;
		typedef const T&                   const_t;
	};

	//��ct_type_pick_t������ȡ�ļ򻯶���
	//�ͱ����:���������ͱ�,����ԭʼ�ͱ�
	template< class T > struct ct_remove_reference { typedef T type; };
	template< class T > struct ct_remove_reference<T&> { typedef T type; };

	//�ͱ����:���ڷ�ָ���ͱ���ָ���ͱ�
	template< class T > struct ct_add_pointer
	{
		typedef typename ct_remove_reference<T>::type * type;
	};
	//�ͱ����:����const����,����ԭʼ�ͱ�
	template< class T > struct ct_remove_const { typedef T type; };
	template< class T > struct ct_remove_const<const T> { typedef T type; };

	//�ͱ����:����volatile����,����ԭʼ�ͱ�
	template< class T > struct ct_remove_volatile { typedef T type; };
	template< class T > struct ct_remove_volatile<volatile T> { typedef T type; };

	//�ͱ����:����const��volatile����,����ԭʼ�ͱ�
	template< class T >
	struct ct_remove_cv
	{
		typedef typename ct_remove_volatile<typename ct_remove_const<T>::type>::type type;
	};

	//�ͱ������ж�,����Cond���������,����ѡȡThen��Else�ͱ����
	template< bool Cond, class Then, class Else >
	struct ct_cond_pick;

	template< class Then, class Else >
	struct ct_cond_pick< true, Then, Else > { typedef Then type; };

	template< class Then, class Else >
	struct ct_cond_pick< false, Then, Else > { typedef Else type; };
}
#endif

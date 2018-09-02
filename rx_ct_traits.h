#ifndef _H_RX_CT_TRAITS_H_
#define _H_RX_CT_TRAITS_H_
/*
定义了模板类型萃取方面的工具
*/
#include "rx_cc_macro.h"
namespace rx
{
//----------------------------------------------------------
    struct rx_type_true_t {};
    struct rx_type_false_t {};

    //类型检查模板,用于分配器检查构造函数类型
    template <class type>
    struct rx_type_check_t
    {
        typedef rx_type_true_t     this_dummy_member_must_be_first;
        typedef rx_type_false_t    has_trivial_default_constructor;
        typedef rx_type_false_t    has_trivial_copy_constructor;
        typedef rx_type_false_t    has_trivial_assignment_operator;
        typedef rx_type_false_t    has_trivial_destructor;
        typedef rx_type_false_t    is_POD_type;
    };

    template<> struct rx_type_check_t<char>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;
    };

    template<> struct rx_type_check_t<int8_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint8_t unsigned_t;
        typedef int8_t  signed_t;
        typedef uint8_t invsigned_t;
    };

    template<> struct rx_type_check_t<uint8_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint8_t unsigned_t;
        typedef int8_t  signed_t;
        typedef int8_t  invsigned_t;
    };

    template<> struct rx_type_check_t<int16_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint16_t unsigned_t;
        typedef int16_t  signed_t;
        typedef uint16_t invsigned_t;
    };

    template<> struct rx_type_check_t<uint16_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint16_t unsigned_t;
        typedef int16_t  signed_t;
        typedef int16_t  invsigned_t;
    };

    template<> struct rx_type_check_t<int32_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint32_t unsigned_t;
        typedef int32_t  signed_t;
        typedef uint32_t invsigned_t;
    };

    template<> struct rx_type_check_t<uint32_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint32_t unsigned_t;
        typedef int32_t  signed_t;
        typedef int32_t  invsigned_t;
    };

    template<> struct rx_type_check_t<int64_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint64_t unsigned_t;
        typedef int64_t  signed_t;
        typedef uint64_t invsigned_t;
    };

    template<> struct rx_type_check_t<uint64_t>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;

        typedef uint64_t unsigned_t;
        typedef int64_t  signed_t;
        typedef int64_t  invsigned_t;
    };

    template<> struct rx_type_check_t<float>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;
    };

    template<> struct rx_type_check_t<double>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;
    };

    template<> struct rx_type_check_t<long double>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;
    };

    template <class T>
    struct rx_type_check_t<T*>
    {
        typedef rx_type_true_t    has_trivial_default_constructor;
        typedef rx_type_true_t    has_trivial_copy_constructor;
        typedef rx_type_true_t    has_trivial_assignment_operator;
        typedef rx_type_true_t    has_trivial_destructor;
        typedef rx_type_true_t    is_POD_type;
    };

    //----------------------------------------------------------
    /*
    类型提取工具模板rx_type_pick_t能够得到各种条件下的各种类型
    base_代表基本类型  /  org_代表输入的原类型  /  const_代表const类型  /  not_const_t代表非const的原输入类型  /  const_t代表const后的原输入类型

    T=C             val_t=C    ptr_t=C*   ref_t=C&
    T=C*            val_t=C    ptr_t=C*   ref_t=C&
    T=C**           val_t=C*   ptr_t=C**  ref_t=C* &

    T=const C       val_t=C    ptr_t=C*   ref_t=C&
    T=const C*      val_t=C    ptr_t=C*   ref_t=C&
    T=const C**     val_t=C*   ptr_t=C**  ref_t=C* &
    */
    template <class T>
    struct rx_type_pick_t
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
    struct rx_type_pick_t<const T>
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
    struct rx_type_pick_t<T*>
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
    struct rx_type_pick_t<const T*>
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
    struct rx_type_pick_t<T&>
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
    struct rx_type_pick_t<const T&>
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

}
#endif

#ifndef _RX_UT_DTL_LIST_H_
#define _RX_UT_DTL_LIST_H_

#include "../rx_tdd.h"
#include "../rx_dtl_list.h"
#include "../rx_dtl_stack.h"
#include "../rx_dtl_queue.h"
#include "../rx_ct_util.h"

//---------------------------------------------------------
//进行容器的遍历,同时对比给定的数组元素内容
template<class ct, class vt>
inline void test_dtl_list_loop_test_1(rx_tdd_t &rt, const ct &cntr, vt* arr, uint32_t count, bool asc = true, uint32_t asc_offset = 0)
{
	rt.tdd_assert(cntr.size() == count);
	typename ct::iterator I = cntr.begin();
	if (asc)
	{
		for (uint32_t i = asc_offset;I != cntr.end();++i, ++I)
			rt.tdd_assert(*I == arr[i]);
	}
	else
	{
		for (uint32_t i = 0;I != cntr.end();++i, ++I)
			rt.tdd_assert(*I == arr[count - i - 1]);
	}
}

//---------------------------------------------------------
//使用模板特化进行容器测试数据类型的区分
template<class dt>
struct tst_list_data {};
template<>
struct tst_list_data<int>
{
	static int val(int v) { return v; }
	typedef int val_t;
};
template<>
struct tst_list_data<const char*>
{
	static const char* val(uint32_t v)
	{
		switch (v)
		{
		case 0:return "0";
		case 1:return "1";
		case 2:return "2";
		case 3:return "3";
		case 4:return "4";
		}
		return "BAD";
	}
	typedef const char* val_t;
};

//---------------------------------------------------------
//对容器进行栈操作的测试
template<class ct, class dv>
inline void test_dtl_stack_base_1(rx_tdd_t &rt)
{
	typename dv::val_t tst_arr[10];
	uint32_t count = 0;
	ct cntr;
	rt.tdd_assert(cntr.size() == 0);
	rt.tdd_assert(cntr.begin() == cntr.end());

	tst_arr[count] = dv::val(1);
	typename ct::iterator I;
	I = cntr.push_front(tst_arr[count++]);
	rt.tdd_assert(I == cntr.begin());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, false);

	tst_arr[count] = dv::val(2);
	I = cntr.push_front(tst_arr[count++]);
	rt.tdd_assert(I == cntr.begin());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, false);

	tst_arr[count] = dv::val(3);
	I = cntr.push_front(tst_arr[count++]);
	rt.tdd_assert(I == cntr.begin());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, false);

	--count;
	rt.tdd_assert(*cntr.begin() == tst_arr[count]);
	rt.tdd_assert(cntr.pop_front());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, false);

	--count;
	rt.tdd_assert(*cntr.begin() == tst_arr[count]);
	rt.tdd_assert(cntr.pop_front());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, false);

	cntr.clear();
	rt.tdd_assert(cntr.size() == 0);
	rt.tdd_assert(cntr.begin() == cntr.end());
}

//---------------------------------------------------------
//对容器进行栈操作的测试
template<class ct, class dv>
inline void test_dtl_queue_base_1(rx_tdd_t &rt)
{
	typename dv::val_t tst_arr[10];
	uint32_t count = 0;
	ct cntr;
	rt.tdd_assert(cntr.size() == 0);
	rt.tdd_assert(cntr.begin() == cntr.end());

	tst_arr[count] = dv::val(1);
	typename ct::iterator I;
	I = cntr.push_back(tst_arr[count++]);
	rt.tdd_assert(I == cntr.rbegin());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, true);

	tst_arr[count] = dv::val(2);
	I = cntr.push_back(tst_arr[count++]);
	rt.tdd_assert(I == cntr.rbegin());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, true);

	tst_arr[count] = dv::val(3);
	I = cntr.push_back(tst_arr[count++]);
	rt.tdd_assert(I == cntr.rbegin());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, true);

	--count;
	rt.tdd_assert(*cntr.begin() == tst_arr[0]);
	rt.tdd_assert(cntr.pop_front());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, true, 1);

	--count;
	rt.tdd_assert(*cntr.begin() == tst_arr[1]);
	rt.tdd_assert(cntr.pop_front());
	rt.tdd_assert(cntr.size() == count);
	rt.tdd_assert(cntr.begin() != cntr.end());
	test_dtl_list_loop_test_1(rt, cntr, tst_arr, count, true, 2);

	cntr.clear();
	rt.tdd_assert(cntr.size() == 0);
	rt.tdd_assert(cntr.begin() == cntr.end());
}

//---------------------------------------------------------
//对list容器进行rx_foreach遍历测试
template<class T, int name_idx>
inline bool int_list_print_tst(uint32_t idx, uint32_t size, const T& v)
{
	static const char* names[] = { "list ","queue","stack" };
	if (idx == 0)
		printf("%s loop: ", names[name_idx]);
	printf("%d/%d=%d; ", idx + 1, size, v);;
	if (idx == size - 1)
		printf("\n");
	return true;
}

template<class CT, int name_idx>
inline void test_dtl_cntr_foreach_1(rx_tdd_t &rt)
{
	CT cntr;
	for (int i = 0;i < 10;++i)
		cntr.push_back(i);
	rx_foreach(cntr, int_list_print_tst<int, name_idx>);
}
template<class CT, int name_idx>
inline void test_dtl_cntr_foreach_2(rx_tdd_t &rt)
{
	rx::queue_int32_t cntr;
	for (int i = 0;i < 10;++i)
		cntr.push_front(i);
	rx_foreach(cntr, int_list_print_tst<int, name_idx>);
}
//---------------------------------------------------------
rx_tdd(dtl_list_base)
{
	//测试stack_t的栈操作
	test_dtl_stack_base_1<rx::stack_int32_t, tst_list_data<int> >(*this);
	test_dtl_stack_base_1<rx::stack_ct, tst_list_data<const char*> >(*this);

	//测试queue_t的栈操作
	test_dtl_stack_base_1<rx::queue_int32_t, tst_list_data<int> >(*this);
	test_dtl_stack_base_1<rx::queue_ct, tst_list_data<const char*> >(*this);

	//测试list_t的栈操作
	test_dtl_stack_base_1<rx::list_int32_t, tst_list_data<int> >(*this);
	test_dtl_stack_base_1<rx::list_ct, tst_list_data<const char*> >(*this);

	//测试queue_t的队列操作
	test_dtl_queue_base_1<rx::queue_int32_t, tst_list_data<int> >(*this);
	test_dtl_queue_base_1<rx::queue_ct, tst_list_data<const char*> >(*this);

	//测试list_t的队列操作
	test_dtl_queue_base_1<rx::list_int32_t, tst_list_data<int> >(*this);
	test_dtl_queue_base_1<rx::list_ct, tst_list_data<const char*> >(*this);

	//对list容器进行rx_foreach遍历测试.
	test_dtl_cntr_foreach_1<rx::list_int32_t, 0>(*this);
	//对queue容器进行rx_foreach遍历测试.
	test_dtl_cntr_foreach_1<rx::queue_int32_t, 1>(*this);
	//对stack容器进行rx_foreach遍历测试.
	test_dtl_cntr_foreach_2<rx::stack_int32_t, 2>(*this);
}

#endif

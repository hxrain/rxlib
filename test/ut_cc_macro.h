#ifndef _RX_UT_CC_MACRO_H_
#define _RX_UT_CC_MACRO_H_

#include "../rx_tdd.h"
#include "../rx_cc_macro.h"



//LINE: 9: ------------------------------------------------
//LINE:10: ���������к����ڲ�������λ�ò�Ҫ�ı�,������Բ���ͨ��.
void rx_test_CT_LINE(rx_tdd_t &rt)
{
	int RX_CT_LINE = __LINE__;                              //RX_CT_LINE ����ݵ�ǰ�к����ɱ�ʶ��,���Բ�����������в�����ͻ
	int RX_CT_LINE = __LINE__;
	rt.assert(RX_CT_LINE_EX(13) != RX_CT_LINE_EX(14));
	rt.assert(RX_CT_LINE_EX(13) == 13);
	rt.assert(RX_CT_LINE_EX(14) == 14);

	int RX_CT_SYM(int_) = __LINE__;
	rt.assert(RX_CT_SYM_EX(int_, 19) == 19);
}
//---------------------------------------------------------

void rx_test_CT_BASE(rx_tdd_t &rt)
{
	rt.assert((!0) == 1);
	rt.assert((!!0) == 0);

	rt.assert((!2) == 0);
	rt.assert((!!2) == 1);
}

/* MACRO_MAP ����:
#define STRING(x) char const *x##_string = #x;
RX_MACRO_MAP(STRING, foo, bar)
���:
char const *foo_string = "foo"; char const *bar_string = "bar";
�м����:
					   _MACRO_MAP_EAT1(_MACRO_MAP_END ()()(), _MACRO_MAP1) -> _MACRO_MAP_EAT0(0, _MACRO_MAP_END0, _MACRO_MAP1, 0) -> _MACRO_MAP_END0 RX_MACRO_DUMMY -> _MACRO_MAP_END0
		   STRING(bar) _MACRO_MAP_EAT(()()(), _MACRO_MAP1)(STRING, ()()(),  ()()(), ()()(), 0)
					   _MACRO_MAP_EAT1(_MACRO_MAP_END bar, _MACRO_MAP0) -> _MACRO_MAP_EAT0(_MACRO_MAP_END bar, _MACRO_MAP0, 0) -> _MACRO_MAP0 RX_MACRO_DUMMY -> _MACRO_MAP0
		   STRING(foo) _MACRO_MAP_EAT(bar, _MACRO_MAP0)(STRING, bar,  ()()(), ()()(), ()()(), 0)
RX_MACRO_EVAL(_MACRO_MAP1(STRING,  foo, bar, ()()(), ()()(), ()()(), 0))

RX_MACRO_MAPL����:
#define sp(x) int x
void tst(RX_MACRO_MAPL(sp,a,b,c));
���:
void tst(int a , int b , int c);
*/
//�����ӳ��ʹ�õĲ��Ժ���
int rx_test_macromap_add1(int a, int b, int c) { return a + b + c; }

void rx_test_MACROMAP_BASE(rx_tdd_t &rt)
{
	rx_static_assert(RX_MACRO_NARG(1, 2) == 2);
	rx_static_assert(RX_MACRO_NARG(1, 2, 3) == 3);

	#define STRING(x) char const *x##_string = #x;
	RX_MACRO_MAP(STRING, foo, bar);
	rt.assert(strcmp(foo_string, "foo") == 0);
	rt.assert(strcmp(bar_string, "bar") == 0);

	//���ú�ӳ��,��������ԭ��
	#define sp(x) int x
	int rx_test_macromap_add1(RX_MACRO_MAPL(sp, a, b, c));

	//���ú�ӳ��,���ú���
	rt.assert(rx_test_macromap_add1(RX_MACRO_MAPL(RX_MACRO_EXPAND, 1, 2, 3)) == 6);
}

rx_tdd(test_CC_MACRO_base)
{
	rx_test_MACROMAP_BASE(*this);
	rx_test_CT_LINE(*this);
	rx_test_CT_BASE(*this);
}
#endif // _RX_UT_CC_MACRO_H_

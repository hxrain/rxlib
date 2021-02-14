#ifndef _RX_CT_TYPELIST_H_
#define _RX_CT_TYPELIST_H_

namespace rx
{
	//-----------------------------------------------------
	//�ڲ�ռλʹ�õĿ�����
	struct ct_nulltype {};

	//-----------------------------------------------------
	// ct_typelist �ͱ��б��������
	template< class Head, class Tail >
	struct ct_typelist
	{
		typedef Head head;									//�б�ͷ
		typedef Tail tail;									//�б�β
	};

	//-----------------------------------------------------
	//���ͱ��б���������������һǶ������
	#define ct_typelist_make1( T1 ) ct_typelist< T1, ct_nulltype >							//һ���ͱ�������ͱ��б�,�б�βʹ��ռλ�Ŀ�����
	#define ct_typelist_make2( T1, T2) ct_typelist< T1, ct_typelist_make1( T2) >			//�����ͱ�������ͱ��б�,�б�βʹ��Ƕ�׵ĵ������ͱ��б�
	#define ct_typelist_make3( T1, T2, T3) ct_typelist< T1, ct_typelist_make2( T2, T3) >	//�����ͱ�������ͱ��б�,�б�βʹ��Ƕ�׵�˫�����ͱ��б�
	#define ct_typelist_make4( T1, T2, T3, T4) ct_typelist< T1, ct_typelist_make3( T2, T3, T4) >
	#define ct_typelist_make5( T1, T2, T3, T4, T5) ct_typelist< T1, ct_typelist_make4( T2, T3, T4, T5) >
	#define ct_typelist_make6( T1, T2, T3, T4, T5, T6) ct_typelist< T1, ct_typelist_make5( T2, T3, T4, T5, T6) >
	#define ct_typelist_make7( T1, T2, T3, T4, T5, T6, T7) ct_typelist< T1, ct_typelist_make6( T2, T3, T4, T5, T6, T7) >
	#define ct_typelist_make8( T1, T2, T3, T4, T5, T6, T7, T8) ct_typelist< T1, ct_typelist_make7( T2, T3, T4, T5, T6, T7, T8) >
	#define ct_typelist_make9( T1, T2, T3, T4, T5, T6, T7, T8, T9) ct_typelist< T1, ct_typelist_make8( T2, T3, T4, T5, T6, T7, T8, T9) >
	#define ct_typelist_make10( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) ct_typelist< T1, ct_typelist_make9( T2, T3, T4, T5, T6, T7, T8, T9, T10) >
	#define ct_typelist_make11( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) ct_typelist< T1, ct_typelist_make10( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >
	#define ct_typelist_make12( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) ct_typelist< T1, ct_typelist_make11( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) >
	#define ct_typelist_make13( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) ct_typelist< T1, ct_typelist_make12( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) >
	#define ct_typelist_make14( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) ct_typelist< T1, ct_typelist_make13( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) >
	#define ct_typelist_make15( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) ct_typelist< T1, ct_typelist_make14( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) >
	#define ct_typelist_make16( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) ct_typelist< T1, ct_typelist_make15( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) >

	//-----------------------------------------------------
	//�����ͱ����ռλʹ�õ�Ĭ�ϲ�����.
	typedef struct {} ct_dummy_T1;
	typedef struct {} ct_dummy_T2;
	typedef struct {} ct_dummy_T3;
	typedef struct {} ct_dummy_T4;
	typedef struct {} ct_dummy_T5;
	typedef struct {} ct_dummy_T6;
	typedef struct {} ct_dummy_T7;
	typedef struct {} ct_dummy_T8;
	typedef struct {} ct_dummy_T9;
	typedef struct {} ct_dummy_T10;
	typedef struct {} ct_dummy_T11;
	typedef struct {} ct_dummy_T12;
	typedef struct {} ct_dummy_T13;
	typedef struct {} ct_dummy_T14;
	typedef struct {} ct_dummy_T15;

	//-----------------------------------------------------
	// ɸѡ�ͱ��б��е�����ͱ�ߴ�ֵ���Ӧ����
	template< class List >
	struct ct_typelist_max;

	template<>
	struct ct_typelist_max< ct_nulltype >
	{
		enum V { value = 0 };								//ƫ�ػ�ɸѡ������,�б����,0����
		typedef void type;
	};

	template< class Head, class Tail >						//ƫ�ػ�ɸѡ�ͱ��б�,���׵�������,�õ��ߴ������ͱ����Ӧ�ĳߴ�
	struct ct_typelist_max< ct_typelist<Head, Tail> >
	{
	private:
		enum _ { tail_value = size_t(ct_typelist_max<Tail>::value) };		//Ƕ������β�ڵ��ͱ�,�õ�β�ڵ��ͱ�ߴ�

		typedef typename ct_typelist_max<Tail>::type tail_type;				//Ƕ������β�ڵ��ͱ�,�õ����ߴ��Сɸѡ����δ�ڵ��ͱ����

	public:
		enum V { value = (sizeof(Head) > tail_value) ? sizeof(Head) : size_t(tail_value) };			//����ͷβ�ڵ�ߴ�ɸѡ,�����ߴ���

		typedef typename ct_cond_pick< (sizeof(Head) > tail_value), Head, tail_type>::type type;	//���ݵ�ǰͷβ�ڵ�ߴ��С,ѡȡ��ߴ��Ӧ�ͱ�Ϊ����ͱ�
	};

	//-----------------------------------------------------
	// ɸѡ�ͱ��б���ָ���ͱ��˳������(���ͱ�����)
	template< class List, class T >
	struct ct_typelist_of;

	template< class T >
	struct ct_typelist_of< ct_nulltype, T >
	{
		enum V { value = -1 };								//�ͱ�T�����ͱ��б���,����Ϊ-1
	};

	template< class Tail, class T >
	struct ct_typelist_of< ct_typelist<T, Tail>, T >
	{
		enum V { value = 0 };								//����ͱ�T��ָ����β�ͱ���ͬ,����Ϊ0
	};

	template< class Head, class Tail, class T >
	struct ct_typelist_of< ct_typelist<Head, Tail>, T >
	{
	private:
		enum _ { nextVal = ct_typelist_of<Tail, T>::value };//Ƕ������β�ڵ��ͱ��ָ���ͱ�T,���ײ�ѯǶ�����

	public:
		enum V { value = nextVal == -1 ? -1 : 1 + nextVal };//����Ƕ����ȵõ����ս��:�ͱ�T���ͱ��б��е�˳��λ��
	};

	//-----------------------------------------------------
	//��ȡ�ͱ��б���ָ������˳����ͱ�(����Ų��ͱ�)
	template< class List, size_t i>
	struct ct_typelist_at;

	template< class Head, class Tail >
	struct ct_typelist_at< ct_typelist<Head, Tail>, 0 >
	{
		typedef Head type;									//�б�˳������Ϊ0,��ʹ��ͷ�ͱ�
	};

	template< class Head, class Tail, size_t i >
	struct ct_typelist_at< ct_typelist<Head, Tail>, i >
	{
		typedef typename ct_typelist_at<Tail, i - 1>::type type;	//Ƕ������β�ڵ��ͱ�͵ݼ���˳������,���ײ�ѯǶ����ȶ�Ӧ��ͷ�ͱ���Ϊ���
	};

}

#endif
#ifndef _RX_RAND_H_
#define _RX_RAND_H_

#include "rx_cc_macro.h"
#include <stdlib.h>

namespace rx
{

    //-----------------------------------------------------
    //������������ӿ�
    class rand_i
    {
        rand_i& operator=(const rand_i&);
    protected:
        virtual ~rand_i(){}
    public:
        //-------------------------------------------------
        //���ø������������������
        virtual void seed(uint32_t s){}
        //-------------------------------------------------
        //�õ�uint32_t���͵������,�����Χ��[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)=0;
    };

    //-----------------------------------------------------
    //����HGE��������㷨
    class rand_hge:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
        uint32_t  m_seed;
    public:
        rand_hge(uint32_t s=0){seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s){m_seed=s;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=214013*m_seed+2531011;
            return Min+(m_seed ^ (m_seed>>15))%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //��װstd������㷨
    class rand_std:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
    public:
        rand_std(uint32_t s=0){seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s){srand(s);}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            return Min+ rand() % (Max - Min+1);
        }
    };

    //-----------------------------------------------------
    //����skeeto��hash������㷨(b)
    class rand_skeeto_b:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
        uint32_t  m_seed;

        static uint32_t hash(uint32_t x)
        {//Avalanche score = 1.1875
            x = ~x;
            x ^= x << 16;
            x ^= x >> 1;
            x ^= x << 13;
            x ^= x >> 4;
            x ^= x >> 12;
            x ^= x >> 2;
            return x;
        }
    public:
        rand_skeeto_b(uint32_t s=0){seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s){m_seed=s;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //����skeeto��hash������㷨(triple)
    class rand_skeeto_triple:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
        uint32_t  m_seed;

        //���й�ͳ����֤�Ĺ�ϣ����(exact bias: 0.020829410544597495)
        static uint32_t hash(uint32_t x)
        {
            ++x;
            x ^= x >> 17;
            x *= uint32_t(0xed5ad4bb);
            x ^= x >> 11;
            x *= uint32_t(0xac4c1b51);
            x ^= x >> 15;
            x *= uint32_t(0x31848bab);
            x ^= x >> 14;
            return x;
        }
    public:
        rand_skeeto_triple(uint32_t s=0){seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s){m_seed=s;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //�������ʹ��������������ı�ݺ���
    template<class rnd_t>
    rand_i& rnd(){static rnd_t rnd;return rnd;}

}
    //�﷨��,���ٷ��������������(��������߳�ֱ�Ӳ���ʹ��,�轨���Լ����̶߳���������)
    #define rx_rnd_hge() rx::rnd<rx::rand_hge>()
    #define rx_rnd_std() rx::rnd<rx::rand_std>()
    #define rx_rnd_b32() rx::rnd<rx::rand_skeeto_b>()
    #define rx_rnd_t32() rx::rnd<rx::rand_skeeto_triple>()

#endif
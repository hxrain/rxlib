#ifndef _RX_HASH_RAND_H_
#define _RX_HASH_RAND_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include <stdlib.h>
#include <math.h>

namespace rx
{
    //-----------------------------------------------------
    //������������ӿ�
    class rand_i
    {
        rand_i& operator=(const rand_i&);
    protected:
        virtual ~rand_i() {}
    public:
        //-------------------------------------------------
        //���ø������������������
        virtual void seed(uint32_t s) {}
        //-------------------------------------------------
        //�õ�uint32_t���͵������,�����Χ��[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)=0;
    };

    //-----------------------------------------------------
    //����HGE��������㷨
    class rand_hge_t:public rand_i
    {
        rand_hge_t& operator=(const rand_hge_t&);
        uint32_t  m_seed;
    public:
        rand_hge_t(uint32_t s=0) {seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s) {m_seed=s;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=214013*m_seed+2531011;
            return Min+(m_seed ^ (m_seed>>15))%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //��װstd������㷨
    class rand_std_t:public rand_i
    {
        rand_std_t& operator=(const rand_std_t&);
    public:
        rand_std_t(uint32_t s=0) {seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s) {srand(s);}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            return Min+ rand() % (Max - Min+1);
        }
    };

    //-----------------------------------------------------
    //����skeeto��hash������㷨(b)
    class rand_skeeto_b32_t:public rand_i
    {
        rand_skeeto_b32_t& operator=(const rand_skeeto_b32_t&);
        uint32_t  m_seed;

        static uint32_t hash(uint32_t x)
        {
            //Avalanche score = 1.1875
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
        rand_skeeto_b32_t(uint32_t s=0) {seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s) {m_seed=s;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //����skeeto��hash������㷨(triple)
    class rand_skeeto_triple_t:public rand_i
    {
        rand_skeeto_triple_t& operator=(const rand_skeeto_triple_t&);
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
        rand_skeeto_triple_t(uint32_t s=0) {seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s) {m_seed=s;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };
    //-----------------------------------------------------
    //����redis/skiplist������㷨
    class rand_skiplist_t:public rand_i
    {
        rand_skiplist_t& operator=(const rand_skiplist_t&);
        uint32_t  m_seed;
        static const uint32_t M = 2147483647L;     // 2^31-1
        static const uint64_t A = 16807;           // bits 14, 8, 7, 5, 2, 1, 0

        static uint32_t hash(uint32_t x)
        {
            uint64_t product = x * A;
            // Compute (product % M) using the fact that ((x << 31) % M) == x.
            x = static_cast<uint32_t>((product >> 31) + (product & M));
            // The first reduction may overflow by 1 bit, so we may need to
            // repeat.  mod == M is not possible; using > allows the faster
            // sign-bit-based test.
            if (x > M) x -= M;
            return x;
        }
    public:
        rand_skiplist_t(uint32_t s=0) {seed(s);}
        //��ʼ������
        virtual void seed(uint32_t s) {m_seed=(s+1)&M;}
        //���������,[Min,Max](Ĭ��Ϊ[0,(2^31)-1])
        virtual uint32_t get(uint32_t Max=0x7ffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //���ɷֲ��ĸ��ʹ�ʽ:  P(N(t) = n) = (((L*t)^n)*e^-(L*t))/n!
    class p_poisson_t
    {
    public:
        //-------------------------------------------------
        //������ʱ��t�ķ�Χ��,����n���¼��ĸ���;lambdaΪ�¼�������ƽ��Ƶ��(ƽ����).
        static double pobability(uint8_t n, double lambda = 1.0, uint32_t t = 1)
        {
            return pow(lambda*t, (int)n) * exp(-lambda*t) / factorial(n);
        }
        //-------------------------------------------------
        //������ʱ��t�ķ�Χ��,����<=n���¼����ۻ�����;lambdaΪ�¼�������ƽ��Ƶ��;with_zero��֪�Ƿ��ۼ�0�ζ�Ӧ�ĸ���.
        static double cumulative(uint8_t n, double lambda = 1.0,uint32_t t = 1, bool with_zero=false)
        {
            double e = exp(-lambda*t);
            double sum = 0.0;
            for(uint32_t i = (with_zero||n==0) ? 0 : 1;i<=n;++i)
                sum += pow(lambda*t, (int)i) / factorial(i);
            return e * sum;
        }
    };

    //-----------------------------------------------------
    //���ڲ��ɷֲ��������������(lambdaΪ�¼�������ƽ��Ƶ�ʻ��ڴ���ƽ����)
    template<class rnd_t>
    class rand_poisson_t :public rand_i
    {
        rand_poisson_t& operator=(const rand_poisson_t&);
        rnd_t   m_rnd;
        double  m_lambda;
    public:
        rand_poisson_t(uint32_t s = 0 , double lambda = 10) { seed(s,lambda); }
        //-------------------------------------------------
        //��ʼ�����Ӳ����ø��ʷֲ�����(�ڵ�λʱ����,lambdaΪ�¼�������ƽ��Ƶ�ʻ�ƽ����)
        virtual void seed(uint32_t s, double lambda = 1.0)
        { 
            m_rnd.seed(s);
            m_lambda = -lambda;
        }
        //-------------------------------------------------
        //���ɲ��ɷֲ������(�ڵ�λʱ�����¼������Ĵ���),��Χ��[Min,Max](Ĭ��Ϊ[0,(2^31)-1])
        virtual uint32_t get(uint32_t Max = 0x7ffffffe, uint32_t Min = 0)
        {
            uint32_t k = -1;
            double p=0;
            
            do
            {
                ++k;
                p+=log(m_rnd.get(1000000, 1)/1000000.0);    //�������ΧҪ��(0,1]֮��(���ܵ���0),����log�������.
            }
            while (p >= m_lambda && k<Max);
            return Min + k % (Max - Min + 1);
        }
    };
    typedef rand_poisson_t<rand_skiplist_t> rand_poisson_skt;

    //-----------------------------------------------------
    //�������ʹ��������������ı�ݺ���(���̰߳�ȫ)
    template<class rnd_t>
    rand_i& rnd() {static rnd_t rd; return rd;}

}
//�﷨��,���ٷ��������������(��������߳�ֱ�Ӳ���ʹ��,�轨���Լ����̶߳����������������)
#define rx_rnd_hge() rx::rnd<rx::rand_hge_t>()
#define rx_rnd_std() rx::rnd<rx::rand_std_t>()
#define rx_rnd_b32() rx::rnd<rx::rand_skeeto_b32_t>()
#define rx_rnd_t32() rx::rnd<rx::rand_skeeto_triple_t>()

#endif

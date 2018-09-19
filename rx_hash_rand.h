#ifndef _RX_HASH_RAND_H_
#define _RX_HASH_RAND_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include <stdlib.h>
#include <math.h>

namespace rx
{
    //-----------------------------------------------------
    //随机数发生器接口
    class rand_i
    {
        rand_i& operator=(const rand_i&);
    protected:
        virtual ~rand_i() {}
    public:
        //-------------------------------------------------
        //设置该随机数发生器的种子
        virtual void seed(uint32_t s) {}
        //-------------------------------------------------
        //得到uint32_t类型的随机数,结果范围在[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)=0;
    };

    //-----------------------------------------------------
    //参照HGE的随机数算法
    class rand_hge_t:public rand_i
    {
        rand_hge_t& operator=(const rand_hge_t&);
        uint32_t  m_seed;
    public:
        rand_hge_t(uint32_t s=0) {seed(s);}
        //初始化种子
        virtual void seed(uint32_t s) {m_seed=s;}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=214013*m_seed+2531011;
            return Min+(m_seed ^ (m_seed>>15))%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //封装std随机数算法
    class rand_std_t:public rand_i
    {
        rand_std_t& operator=(const rand_std_t&);
    public:
        rand_std_t(uint32_t s=0) {seed(s);}
        //初始化种子
        virtual void seed(uint32_t s) {srand(s);}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            return Min+ rand() % (Max - Min+1);
        }
    };

    //-----------------------------------------------------
    //参照skeeto的hash随机数算法(b)
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
        //初始化种子
        virtual void seed(uint32_t s) {m_seed=s;}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //参照skeeto的hash随机数算法(triple)
    class rand_skeeto_triple_t:public rand_i
    {
        rand_skeeto_triple_t& operator=(const rand_skeeto_triple_t&);
        uint32_t  m_seed;

        //进行过统计验证的哈希函数(exact bias: 0.020829410544597495)
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
        //初始化种子
        virtual void seed(uint32_t s) {m_seed=s;}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };
    //-----------------------------------------------------
    //参照redis/skiplist随机数算法
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
        //初始化种子
        virtual void seed(uint32_t s) {m_seed=(s+1)&M;}
        //生成随机数,[Min,Max](默认为[0,(2^31)-1])
        virtual uint32_t get(uint32_t Max=0x7ffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //泊松分布的概率公式:  P(N(t) = n) = (((L*t)^n)*e^-(L*t))/n!
    class p_poisson_t
    {
    public:
        //-------------------------------------------------
        //计算在时间t的范围内,发生n次事件的概率;lambda为事件发生的平均频率(平均数).
        static double pobability(uint8_t n, double lambda = 1.0, uint32_t t = 1)
        {
            return pow(lambda*t, (int)n) * exp(-lambda*t) / factorial(n);
        }
        //-------------------------------------------------
        //计算在时间t的范围内,发生<=n次事件的累积概率;lambda为事件发生的平均频率;with_zero告知是否累计0次对应的概率.
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
    //基于泊松分布的随机数生成器(lambda为事件发生的平均频率或期待的平均数)
    template<class rnd_t>
    class rand_poisson_t :public rand_i
    {
        rand_poisson_t& operator=(const rand_poisson_t&);
        rnd_t   m_rnd;
        double  m_lambda;
    public:
        rand_poisson_t(uint32_t s = 0 , double lambda = 10) { seed(s,lambda); }
        //-------------------------------------------------
        //初始化种子并设置概率分布参数(在单位时间内,lambda为事件发生的平均频率或平均数)
        virtual void seed(uint32_t s, double lambda = 1.0)
        { 
            m_rnd.seed(s);
            m_lambda = -lambda;
        }
        //-------------------------------------------------
        //生成泊松分布随机数(在单位时间内事件发生的次数),范围在[Min,Max](默认为[0,(2^31)-1])
        virtual uint32_t get(uint32_t Max = 0x7ffffffe, uint32_t Min = 0)
        {
            uint32_t k = -1;
            double p=0;
            
            do
            {
                ++k;
                p+=log(m_rnd.get(1000000, 1)/1000000.0);    //随机数范围要在(0,1]之间(不能等于0),否则log运算出错.
            }
            while (p >= m_lambda && k<Max);
            return Min + k % (Max - Min + 1);
        }
    };
    typedef rand_poisson_t<rand_skiplist_t> rand_poisson_skt;

    //-----------------------------------------------------
    //方便快速使用随机数发生器的便捷函数(单线程安全)
    template<class rnd_t>
    rand_i& rnd() {static rnd_t rd; return rd;}

}
//语法糖,快速访问随机数发生器(不建议多线程直接并发使用,需建立自己的线程独立的随机数发生器)
#define rx_rnd_hge() rx::rnd<rx::rand_hge_t>()
#define rx_rnd_std() rx::rnd<rx::rand_std_t>()
#define rx_rnd_b32() rx::rnd<rx::rand_skeeto_b32_t>()
#define rx_rnd_t32() rx::rnd<rx::rand_skeeto_triple_t>()

#endif

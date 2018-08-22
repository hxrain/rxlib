#ifndef _RX_RAND_H_
#define _RX_RAND_H_

#include "rx_cc_macro.h"
#include <stdlib.h>

namespace rx
{

    //-----------------------------------------------------
    //随机数发生器接口
    class rand_i
    {
        rand_i& operator=(const rand_i&);
    protected:
        virtual ~rand_i(){}
    public:
        //-------------------------------------------------
        //设置该随机数发生器的种子
        virtual void seed(uint32_t s){}
        //-------------------------------------------------
        //得到uint32_t类型的随机数,结果范围在[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)=0;
    };

    //-----------------------------------------------------
    //参照HGE的随机数算法
    class rand_hge:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
        uint32_t  m_seed;
    public:
        rand_hge(uint32_t s=0){seed(s);}
        //初始化种子
        virtual void seed(uint32_t s){m_seed=s;}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=214013*m_seed+2531011;
            return Min+(m_seed ^ (m_seed>>15))%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //封装std随机数算法
    class rand_std:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
    public:
        rand_std(uint32_t s=0){seed(s);}
        //初始化种子
        virtual void seed(uint32_t s){srand(s);}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            return Min+ rand() % (Max - Min+1);
        }
    };

    //-----------------------------------------------------
    //参照skeeto的hash随机数算法(b)
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
        //初始化种子
        virtual void seed(uint32_t s){m_seed=s;}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //参照skeeto的hash随机数算法(triple)
    class rand_skeeto_triple:public rand_i
    {
        rand_hge& operator=(const rand_hge&);
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
        rand_skeeto_triple(uint32_t s=0){seed(s);}
        //初始化种子
        virtual void seed(uint32_t s){m_seed=s;}
        //生成随机数,[Min,Max](默认为[0,(2^32)-1])
        virtual uint32_t get(uint32_t Max=0xfffffffe, uint32_t Min=0)
        {
            m_seed=hash(m_seed);
            return Min+m_seed%(Max-Min+1);
        }
    };

    //-----------------------------------------------------
    //方便快速使用随机数发生器的便捷函数
    template<class rnd_t>
    rand_i& rnd(){static rnd_t rnd;return rnd;}

}
    //语法糖,快速访问随机数发生器(不建议多线程直接并发使用,需建立自己的线程独立发生器)
    #define rx_rnd_hge() rx::rnd<rx::rand_hge>()
    #define rx_rnd_std() rx::rnd<rx::rand_std>()
    #define rx_rnd_b32() rx::rnd<rx::rand_skeeto_b>()
    #define rx_rnd_t32() rx::rnd<rx::rand_skeeto_triple>()

#endif
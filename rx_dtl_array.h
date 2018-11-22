#ifndef _RX_DTL_ARRAY_H_
#define _RX_DTL_ARRAY_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_mem_alloc_cntr.h"

/*
    //静态数组
    template<class T,uint32_t size>
    class array_ft;

    //动态分配的内存数组
    template<class T>
    class array_t;

*/

namespace rx
{
    //-----------------------------------------------------
    //基础的数组访问操作
    template<class T>
    class array_i
    {
    protected:
        T*          m_ptr;                                  //数组指针
        uint32_t    m_capacity;                             //数组的最大容量
    public:
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_capacity; }
        //直接获取数组指针
        T* array()const{return m_ptr;}
        //直接访问数组元素
        T& at(uint32_t idx)const { return m_ptr[idx]; }
        //-------------------------------------------------
        //根据索引访问元素;idx从-1开始可以反向访问元素
        T& operator []( int32_t idx )
        {
            if (idx >= 0)
            {
                rx_assert_msg((uint32_t)idx < m_capacity, "array_i 下标越界!");
                return m_ptr[idx];
            }
            else
            {
                rx_assert_msg(m_capacity + idx >= 0, "array_i 下标越界!");
                return m_ptr[m_capacity + idx];
            }
        }
        //-------------------------------------------------
        //统一给数组元素赋值
        void set(const T &dat)
        {
            for (uint32_t i = 0; i < m_capacity; ++i)
                m_ptr[i] = dat;
        }
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const array_i       *m_parent;
            uint32_t             m_pos;
        public:
            //---------------------------------------------
            iterator(const array_i &s, uint32_t pos) :m_parent(&s), m_pos(pos) {}
            iterator(const iterator &i) :m_parent(i.m_parent), m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_parent == i.m_parent && m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i)); }
            //---------------------------------------------
            iterator& operator=(const iterator &i) { m_parent = i.m_parent; m_pos = i.m_pos; return *this; }
            //---------------------------------------------
            //*提领运算符重载
            const T& operator*() const
            {
                rx_assert(m_pos<m_parent->capacity());
                return m_parent->at(m_pos);
            }
            //获取当前迭代器在容器中对应的位置索引
            uint32_t pos() const { return m_pos; }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                ++m_pos;
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        iterator begin() const {return iterator(*this,0);}
        iterator end() const { return iterator(*this, m_capacity); }
        //-------------------------------------------------
    protected:
        //构造与析构函数
        array_i(T* arr,uint32_t cap) :m_ptr(arr), m_capacity(cap){}
        virtual ~array_i(){}
        //-------------------------------------------------
    };

    //-----------------------------------------------------
    //静态数组
    template<class T,uint32_t size>
    class array_ft :public array_i<T>
    {
        typedef array_i<T> super_t;
        T       m_items[size];
    public:
        array_ft() :super_t(m_items, size){}
        //-------------------------------------------------
        //赋值符重载:直接将数组元素数据进行复制
        array_ft& operator=(const super_t& S)
        {
            uint32_t mincap = min(super_t::m_capacity,S.m_capacity);
            for (uint32_t i = 0; i<mincap; i++)
                super_t::m_ptr.at(i) = S.m_ptr.at(i);
            return *this;
        }
    };

    //-----------------------------------------------------
    //动态分配的内存数组
    template<class T>
    class array_t:public array_i<T>
    {
        typedef array_i<T> super_t;
        mem_allotter_i &m_mem;
    public:
        array_t() :super_t(NULL, 0), m_mem(rx_global_mem_allotter()) {}
        array_t(mem_allotter_i &m) :super_t(NULL, 0), m_mem(m) {}
        array_t(uint32_t Count, mem_allotter_i &m) :super_t(NULL, 0), m_mem(m) { make(Count); }
        virtual ~array_t() { clear(); }

        //-------------------------------------------------
        bool make(uint32_t Count)
        {
            clear();
            rx_assert_msg(Count!=0,"不能初始化0长度的数组");
            if (!Count) return true;

            super_t::m_capacity=Count;
            super_t::m_ptr = m_mem.new0<T>(Count);
            return super_t::m_ptr!=NULL;
        }
        //-------------------------------------------------
        template<class PT1>
        bool make(uint32_t Count,PT1& P1)
        {
            clear();
            rx_assert_msg(Count != 0, "不能初始化0长度的数组");
            if (!Count) return true;

            super_t::m_capacity = Count;
            super_t::m_ptr = m_mem.new1<T>(P1, Count);
            return super_t::m_ptr != NULL;
        }
        //-------------------------------------------------
        //完全清除数据
        inline void clear()
        {
            if (super_t::m_ptr)
            {
                m_mem.del(super_t::m_ptr);
                super_t::m_ptr = NULL;
                super_t::m_capacity = 0;
            }
        }
        //-------------------------------------------------
        //赋值符重载:直接将数组元素数据进行复制
        array_t& operator=(const super_t& S)
        {
            if (super_t::m_capacity!=S.m_capacity)
                make(S.m_capacity);
            for(uint32_t i=0;i<super_t::m_capacity;i++)
                super_t::m_ptr.at(i)=S.m_ptr.at(i);
            return *this;
        }
        //-------------------------------------------------
    };
}
#endif

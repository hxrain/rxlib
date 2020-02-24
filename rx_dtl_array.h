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
    //基础的数组访问操作,(内部不维护元素的构造与析构)
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
        T& at(uint32_t idx) const
        {
            rx_assert_msg((uint32_t)idx < m_capacity, "array_i 下标越界!");
            return m_ptr[idx];
        }
        //-------------------------------------------------
        //根据索引访问元素;idx从-1开始可以反向访问元素
        T& operator []( int32_t idx ) const
        {
            idx = (m_capacity + idx) % m_capacity;
            rx_assert_msg((uint32_t)idx < m_capacity, "array_i 下标越界!");
            return m_ptr[idx];
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
    template<class T,uint32_t max_size>
    class array_ft :public array_i<T>
    {
        typedef array_i<T> super_t;
        T       m_items[max_size];
    public:
        array_ft() :super_t(m_items, max_size){}
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
        bool make(uint32_t max_size, bool can_reuse=false)
        {
            if (can_reuse&&max_size < super_t::m_capacity)
                return true;                                //可复用且容量大于要求,则直接返回

            clear();
            rx_assert_msg(max_size!=0,"不能初始化0长度的数组");
            if (!max_size) return false;

            super_t::m_capacity=max_size;
            super_t::m_ptr = m_mem.new0<T>(max_size);
            return super_t::m_ptr!=NULL;
        }
        //-------------------------------------------------
        //带有初始值的初始化构造函数
        template<class PT1>
        bool make(uint32_t max_size,PT1& P1, bool can_reuse = false)
        {
            if (can_reuse&&max_size < super_t::m_capacity)
                return true;                                //可复用且容量大于要求,则直接返回

            clear();
            rx_assert_msg(max_size != 0, "不能初始化0长度的数组");
            if (!max_size) return true;

            super_t::m_capacity = max_size;
            super_t::m_ptr = m_mem.new1<T>(P1, max_size);
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

    //-----------------------------------------------------
    //基于数组容器的简单stack功能封装(内部不维护元素的构造与析构)
    template<class T>
    class array_stack_t
    {
    public:
        typedef array_i<T> cntr_i;
        typedef typename cntr_i::iterator iterator;
    protected:
        uint32_t    m_size;
        cntr_i     &m_cntr;
    public:
        //-------------------------------------------------
        array_stack_t(cntr_i &c):m_size(0),m_cntr(c){}
        //栈的当前元素数量
        uint32_t size(){return m_size;}
        void clear(){m_size=0;}
        //-------------------------------------------------
        bool push_back(const T &data)
        {
            if (m_size>=m_cntr.capacity())
                return false;
            m_cntr[m_size++]=data;
            return true;
        }
        //-------------------------------------------------
        bool pop_back()
        {
            if (m_size==0)
                return false;
            --m_size;
            return true;
        }
        //-------------------------------------------------
        //正向访问栈底(数组元素索引0)
        iterator begin() const {return m_cntr.begin();}
        //-------------------------------------------------
        //反向访问栈顶(数组元素索引size()-1)
        iterator rbegin() const {return cntr_i::iterator(m_cntr,m_size-1);}
        //-------------------------------------------------
        //正向访问栈顶结束点(数组元素索引size())
        iterator end() const {return cntr_i::iterator(m_cntr,m_size);}
        //-------------------------------------------------
        //栈的最大容量
        uint32_t capacity() const {return m_cntr.capacity();}
    };

    //-----------------------------------------------------
    //内部持有定长数组空间的栈
    template<class T,uint32_t max_size>
    class array_stack_ft:public array_stack_t<T>
    {
        typedef array_ft<T,max_size>    cntr_t;
        cntr_t      m_cntr_space;
    public:
        array_stack_ft():array_stack_t(m_cntr_space){}
    };
}
#endif

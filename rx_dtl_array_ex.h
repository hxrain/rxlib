#ifndef _RX_DTL_ARRAY_EX_H_
#define _RX_DTL_ARRAY_EX_H_

#include "rx_cc_macro.h"
#include "rx_dtl_array.h"
#include "rx_dtl_hashtbl.h"
#include "rx_dtl_hashtbl_ex.h"

namespace rx
{
    //-----------------------------------------------------
    //带有关联别名的DT元素数组,在索引序号访问的基础上,增加了别名关联访问的功能
    //DT为数据元素;max_alias_size为别名的最大长度;ic告知字符串大小写敏感性(0敏感;1脱敏为小写;2脱敏为大写)
    template<typename DT,uint32_t max_alias_size,uint32_t ic,typename CT=char>
    class alias_array_i
    {
    protected:
        //数组容器的节点类型
        typedef struct array_node_t
        {
            uint32_t    alias_pos;                          //指向maps中的别名位置
            DT          item;                               //数据元素
            array_node_t():alias_pos(-1){}
            //特殊构造,给元素对象传递内存分配器
            array_node_t(mem_allotter_i &ma) :alias_pos(-1),item(ma) {}
        }array_node_t;

        //字符串脱敏工具类型
        typedef icstr<max_alias_size, CT, ic> icstr_t;

        //基础的数组容器类型
        typedef array_i<array_node_t> base_cntr_t;
        //别名定长串类型
        typedef tiny_string_t<CT, max_alias_size> alias_t;
        //哈希表关联容器类型
        typedef hashtbl_base_t<alias_t, uint32_t, hashtbl_cmp_t, false> base_map_t;

        base_cntr_t         &m_array;                       //定义数组容器基础对象
        base_map_t          &m_maps;                        //定义map容器基础对象

        //构造与析构函数,必须被子类继承后使用
        alias_array_i(base_cntr_t &arr, base_map_t &map):m_array(arr),m_maps(map) {}
        virtual ~alias_array_i() {}
    public:
        //-------------------------------------------------
        //数组的元素数量
        uint32_t capacity() const { return m_array.capacity(); }
        //-------------------------------------------------
        //给索引idx对应的元素绑定别名alias
        bool bind(uint32_t idx, const CT *alias)
        {
            if (idx >= capacity())
                return false;                               //下标越界
            if (m_array.at(idx).alias_pos != (uint32_t)-1)
                return false;                               //元素已经绑定过了
            //绑定元素对应的别名
            return NULL != m_maps.insert(icstr_t(alias).c_str(), idx, m_array.at(idx).alias_pos);
        }
        //-------------------------------------------------
        //绑定过的名字数量
        uint32_t size() const { return m_maps.size(); }
        //-------------------------------------------------
        //根据别名查找元素
        DT* operator[](const CT *alias) const
        {
            uint32_t pos;
            typename base_map_t::node_t *node = m_maps.find(icstr_t(alias).c_str(), pos);
            if (!node) return false;                        //未找到别名对应的索引
            rx_assert(node->val < capacity());
            rx_assert(m_array.at(node->val).alias_pos==pos);
            return &m_array.at(node->val).item;
        }
        //-------------------------------------------------
        //根据索引访问元素
        DT& operator[](const uint32_t idx) const
        {
            return m_array.at(idx).item;
        }
        //-------------------------------------------------
        //根据索引获取别名
        const CT* alias(const uint32_t idx) const
        {
            if (idx >= capacity())
                return NULL;                                //下标越界
            uint32_t pos = m_array.at(idx).alias_pos;
            if (pos == (uint32_t)-1)
                return NULL;                                //该元素未绑定别名
            return m_maps.at(pos)->value.key.c_str();
        }
        //-------------------------------------------------
        //根据别名获取索引
        //返回值:capacity()别名不存在;
        uint32_t index(const CT *alias) const
        {
            uint32_t pos;
            typename base_map_t::node_t *node = m_maps.find(icstr_t(alias).c_str(), pos);
            return node == NULL ? capacity() : node->value.val;
        }
        //-------------------------------------------------
        //清理绑定关系,之后可以重新绑定
        void reset()
        {
            for (uint32_t i = 0; i < capacity(); ++i)
                m_array.at(i).alias_pos = (uint32_t)-1;
            m_maps.clear();
        }
        //-------------------------------------------------
        //统一给数组元素赋值
        void set(const DT &dat)
        {
            for (uint32_t i = 0; i < capacity(); ++i)
                m_array.at(i).item = dat;
        }
    };

    //-----------------------------------------------------
    //固定容量的别名数组(factor为哈希表扩容百分比)
    template<uint32_t max_items,typename DT=uint32_t, uint32_t max_alias_size = 32, uint32_t ic = 0,uint32_t factor=30, typename CT = char>
    class alias_array_ft:public alias_array_i<DT,max_alias_size,ic,CT>
    {
        //描述父类
        typedef alias_array_i<DT, max_alias_size, ic, CT> super_t;
        //描述数组容器实体类
        typedef array_ft<typename super_t::array_node_t, max_items> array_t;
        //计算哈希容器的容量上限
        static const uint32_t hashtbl_max_items = (uint32_t)(max_items*(1 + factor / 100.0));
        //描述哈希容器实体类
        typedef tiny_hashtbl_st<hashtbl_max_items, uint32_t, max_alias_size, false, CT> hashtbl_t;

        array_t     m_array;                                //定义数组容器实体
        hashtbl_t   m_hashtbl;                              //定义哈希容器实体
    public:
        alias_array_ft() :super_t(m_array, m_hashtbl) {}
    };

    //-----------------------------------------------------
    //动态内存分配的别名数组
    template<typename DT = uint32_t, uint32_t max_alias_size = 32, uint32_t ic = 0,typename CT = char>
    class alias_array_t :public alias_array_i<DT, max_alias_size, ic, CT>
    {
        //描述父类
        typedef alias_array_i<DT, max_alias_size, ic, CT> super_t;
        //描述数组容器实体类
        typedef array_t<typename super_t::array_node_t> array_t;
        //描述哈希容器实体类
        typedef hashtbl_st<uint32_t, max_alias_size, false, CT> hashtbl_t;

        array_t         m_array;                            //定义数组容器实体
        hashtbl_t       m_hashtbl;                          //定义哈希容器实体
        mem_allotter_i  &m_mem;
    public:
        //-------------------------------------------------
        alias_array_t() :m_mem(rx_global_mem_allotter()),super_t(m_array, m_hashtbl) {}
        alias_array_t(mem_allotter_i &ma) :m_array(ma), m_hashtbl(ma), m_mem(ma),super_t(m_array, m_hashtbl) {}
        ~alias_array_t() { clear(); }
        //-------------------------------------------------
        //动态生成别名数组,同时告知哈希表的扩容系数
        bool make(uint32_t max_items,uint32_t factor=30)
        {
            clear();
            if (!m_array.make(max_items))
                return false;
            if (!m_hashtbl.make(max_items, (float)(factor / 100.0)))
                return false;
            return true;
        }
        //动态生成别名数组,同时给元素提供内存分配器告知哈希表的扩容系数
        bool make_ex(uint32_t max_items, uint32_t factor = 30)
        {
            clear();
            if (!m_array.make(max_items,m_mem))             //让用户数据元素使用内存分配器对象进行构造初始化
                return false;
            if (!m_hashtbl.make(max_items, (float)(factor / 100.0)))
                return false;
            return true;
        }
        //-------------------------------------------------
        //释放资源,如果不是复位,则需要再次make后才能使用
        void clear(bool reset_only=false)
        {
            super_t::reset();
            if (reset_only)
                return;
            m_hashtbl.clear();
            m_array.clear();
        }
    };

}

#endif

#ifndef _RX_DTL_ARRAY_EX_H_
#define _RX_DTL_ARRAY_EX_H_

#include "rx_cc_macro.h"
#include "rx_dtl_array.h"
#include "rx_dtl_hashtbl.h"
#include "rx_dtl_hashtbl_ex.h"

namespace rx
{
    //-----------------------------------------------------
    //���й���������DTԪ������,��������ŷ��ʵĻ�����,�����˱����������ʵĹ���
    //DTΪ����Ԫ��;max_alias_sizeΪ��������󳤶�;ic��֪�ַ�����Сд������(0����;1����ΪСд;2����Ϊ��д)
    template<typename DT,uint32_t max_alias_size,uint32_t ic,typename CT=char>
    class alias_array_i
    {
    protected:
        //���������Ľڵ�����
        typedef struct array_node_t
        {
            uint32_t    alias_pos;                          //ָ��maps�еı���λ��
            DT          item;                               //����Ԫ��
            array_node_t():alias_pos(-1){}
            //���⹹��,��Ԫ�ض��󴫵��ڴ������
            array_node_t(mem_allotter_i &ma) :alias_pos(-1),item(ma) {}
        }array_node_t;

        //�ַ���������������
        typedef icstr<max_alias_size, CT, ic> icstr_t;

        //������������������
        typedef array_i<array_node_t> base_cntr_t;
        //��������������
        typedef tiny_string_t<CT, max_alias_size> alias_t;
        //��ϣ�������������
        typedef hashtbl_base_t<alias_t, uint32_t, hashtbl_cmp_t, false> base_map_t;

        base_cntr_t         &m_array;                       //��������������������
        base_map_t          &m_maps;                        //����map������������

        //��������������,���뱻����̳к�ʹ��
        alias_array_i(base_cntr_t &arr, base_map_t &map):m_array(arr),m_maps(map) {}
        virtual ~alias_array_i() {}
    public:
        //-------------------------------------------------
        //�����Ԫ������
        uint32_t capacity() const { return m_array.capacity(); }
        //-------------------------------------------------
        //������idx��Ӧ��Ԫ�ذ󶨱���alias
        bool bind(uint32_t idx, const CT *alias)
        {
            if (idx >= capacity())
                return false;                               //�±�Խ��
            if (m_array.at(idx).alias_pos != (uint32_t)-1)
                return false;                               //Ԫ���Ѿ��󶨹���
            //��Ԫ�ض�Ӧ�ı���
            return NULL != m_maps.insert(icstr_t(alias).c_str(), idx, m_array.at(idx).alias_pos);
        }
        //-------------------------------------------------
        //�󶨹�����������
        uint32_t size() const { return m_maps.size(); }
        //-------------------------------------------------
        //���ݱ�������Ԫ��
        DT* operator[](const CT *alias) const
        {
            uint32_t pos;
            typename base_map_t::node_t *node = m_maps.find(icstr_t(alias).c_str(), pos);
            if (!node) return false;                        //δ�ҵ�������Ӧ������
            rx_assert(node->val < capacity());
            rx_assert(m_array.at(node->val).alias_pos==pos);
            return &m_array.at(node->val).item;
        }
        //-------------------------------------------------
        //������������Ԫ��
        DT& operator[](const uint32_t idx) const
        {
            return m_array.at(idx).item;
        }
        //-------------------------------------------------
        //����������ȡ����
        const CT* alias(const uint32_t idx) const
        {
            if (idx >= capacity())
                return NULL;                                //�±�Խ��
            uint32_t pos = m_array.at(idx).alias_pos;
            if (pos == (uint32_t)-1)
                return NULL;                                //��Ԫ��δ�󶨱���
            return m_maps.at(pos)->value.key.c_str();
        }
        //-------------------------------------------------
        //���ݱ�����ȡ����
        //����ֵ:capacity()����������;
        uint32_t index(const CT *alias) const
        {
            uint32_t pos;
            typename base_map_t::node_t *node = m_maps.find(icstr_t(alias).c_str(), pos);
            return node == NULL ? capacity() : node->value.val;
        }
        //-------------------------------------------------
        //����󶨹�ϵ,֮��������°�
        void reset()
        {
            for (uint32_t i = 0; i < capacity(); ++i)
                m_array.at(i).alias_pos = (uint32_t)-1;
            m_maps.clear();
        }
        //-------------------------------------------------
        //ͳһ������Ԫ�ظ�ֵ
        void set(const DT &dat)
        {
            for (uint32_t i = 0; i < capacity(); ++i)
                m_array.at(i).item = dat;
        }
    };

    //-----------------------------------------------------
    //�̶������ı�������(factorΪ��ϣ�����ݰٷֱ�)
    template<uint32_t max_items,typename DT=uint32_t, uint32_t max_alias_size = 32, uint32_t ic = 0,uint32_t factor=30, typename CT = char>
    class alias_array_ft:public alias_array_i<DT,max_alias_size,ic,CT>
    {
        //��������
        typedef alias_array_i<DT, max_alias_size, ic, CT> super_t;
        //������������ʵ����
        typedef array_ft<typename super_t::array_node_t, max_items> array_t;
        //�����ϣ��������������
        static const uint32_t hashtbl_max_items = (uint32_t)(max_items*(1 + factor / 100.0));
        //������ϣ����ʵ����
        typedef tiny_hashtbl_st<hashtbl_max_items, uint32_t, max_alias_size, false, CT> hashtbl_t;

        array_t     m_array;                                //������������ʵ��
        hashtbl_t   m_hashtbl;                              //�����ϣ����ʵ��
    public:
        alias_array_ft() :super_t(m_array, m_hashtbl) {}
    };

    //-----------------------------------------------------
    //��̬�ڴ����ı�������
    template<typename DT = uint32_t, uint32_t max_alias_size = 32, uint32_t ic = 0,typename CT = char>
    class alias_array_t :public alias_array_i<DT, max_alias_size, ic, CT>
    {
        //��������
        typedef alias_array_i<DT, max_alias_size, ic, CT> super_t;
        //������������ʵ����
        typedef array_t<typename super_t::array_node_t> array_t;
        //������ϣ����ʵ����
        typedef hashtbl_st<uint32_t, max_alias_size, false, CT> hashtbl_t;

        array_t         m_array;                            //������������ʵ��
        hashtbl_t       m_hashtbl;                          //�����ϣ����ʵ��
        mem_allotter_i  &m_mem;
    public:
        //-------------------------------------------------
        alias_array_t() :m_mem(rx_global_mem_allotter()),super_t(m_array, m_hashtbl) {}
        alias_array_t(mem_allotter_i &ma) :m_array(ma), m_hashtbl(ma), m_mem(ma),super_t(m_array, m_hashtbl) {}
        ~alias_array_t() { clear(); }
        //-------------------------------------------------
        //��̬���ɱ�������,ͬʱ��֪��ϣ�������ϵ��
        bool make(uint32_t max_items,uint32_t factor=30)
        {
            clear();
            if (!m_array.make(max_items))
                return false;
            if (!m_hashtbl.make(max_items, (float)(factor / 100.0)))
                return false;
            return true;
        }
        //��̬���ɱ�������,ͬʱ��Ԫ���ṩ�ڴ��������֪��ϣ�������ϵ��
        bool make_ex(uint32_t max_items, uint32_t factor = 30)
        {
            clear();
            if (!m_array.make(max_items,m_mem))             //���û�����Ԫ��ʹ���ڴ������������й����ʼ��
                return false;
            if (!m_hashtbl.make(max_items, (float)(factor / 100.0)))
                return false;
            return true;
        }
        //-------------------------------------------------
        //�ͷ���Դ,������Ǹ�λ,����Ҫ�ٴ�make�����ʹ��
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

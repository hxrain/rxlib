#ifndef _RX_DTL_RAXBIT_SET_H_
#define _RX_DTL_RAXBIT_SET_H_

#include "rx_cc_macro.h"
#include "rx_dtl_raxbit_raw.h"
#include "rx_ct_util.h"

/*
    <����ԭʼbit�ֶ�radix����װ�ļ�������>
    template<class val_t,class op_t= raxbit_op_int<val_t> >
    class raxbit_set_base_t;

    <Ĭ�Ͻڵ�Ϊuint32_t���͵�rax����>
    template<class key_t=uint32_t,class op_t= raxbit_op_int<key_t> >
    class raxbit_set_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //����ԭʼbit�ֶ�rax��װ�ļ�������
    //-----------------------------------------------------
    template<class val_t,class op_t= raxbit_op_int<val_t> >
    class raxbit_set_base_t
    {
        typedef raw_raxbit_t<val_t, op_t> raw_rax_t;
    public:
        //-------------------------------------------------
        //�ڵ�����
        typedef typename raw_rax_t::leaf_t node_t;

        //-------------------------------------------------
        //���������������
        class looper_t :public raw_rax_t::back_path_t
        {
            friend class iterator;
            friend class raxbit_set_base_t;
            raw_rax_t       &m_rax;
            node_t          *m_node;
            looper_t(raw_rax_t &r) :m_rax(r), m_node(NULL) {}
        public:
            looper_t(raxbit_set_base_t &set) :m_rax(set.m_rax), m_node(NULL) {}
        };

        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            friend class raxbit_set_base_t;
            const looper_t   *m_looper;                     //�����и���looper_t�Ĵ���,���Ե������н���¼�������󼴿�
        public:
            //---------------------------------------------
            iterator(const looper_t &s) :m_looper(&s) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_looper->m_node == i.m_looper->m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i)); }
            //---------------------------------------------
            iterator& operator=(const iterator &i) { m_looper = i.m_looper; return *this; }
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_looper!= NULL);
                rx_assert(m_looper->m_node != NULL);
                return m_looper->m_node->key;
            }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                typename raw_rax_t::back_path_t *path=(typename raw_rax_t::back_path_t*)m_looper;
                ((looper_t*)m_looper)->m_node = m_looper->m_rax.next(*path);
                return reinterpret_cast<iterator&>(*this);
            }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator--()
            {
                typename raw_rax_t::back_path_t *path=(typename raw_rax_t::back_path_t*)m_looper;
                ((looper_t*)m_looper)->m_node = m_looper->m_rax.prev(*path);
                return reinterpret_cast<iterator&>(*this);
            }
        };
    protected:
        raw_rax_t   m_rax;                              //�ײ�rax����
        looper_t    m_dummy_end;                        //����end()ʹ��
        looper_t    m_dummy_begin;                      //����begin()ʹ��
    public:
        //-------------------------------------------------
        raxbit_set_base_t():m_dummy_end(m_rax), m_dummy_begin(m_rax){}
        raxbit_set_base_t(mem_allotter_i &m):m_rax(m),m_dummy_end(m_rax), m_dummy_begin(m_rax) {}
        virtual ~raxbit_set_base_t() { clear(); }
        //-------------------------------------------------
        //֦������
        uint32_t limbs() const { return m_rax.limbs(); }
        //�Ѿ�ʹ�õ�Ҷ�ӽڵ�����
        uint32_t size() const { return m_rax.size(); }
        //-------------------------------------------------
        //�ڼ����в���Ԫ�ز���ֵ����
        template<class VT>
        bool insert(const VT &val)
        {
            bool dup;
            return insert(val, dup);
        }
        template<class VT>
        bool insert(const VT &val,bool &dup)
        {
            node_t *node = m_rax.insert(val, dup);
            if (!node)
                return false;
            ct::OC(&node->key, val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ��,�ж��Ƿ����
        template<class VT>
        bool find(const VT &val) const { return m_rax.find(val) != NULL; }
        template<class VT>
        bool operator[](const VT &val) const { return find(val); }
        //-------------------------------------------------
        //����Ԫ��,ͬʱ��ñ���������
        template<class VT>
        iterator find(const VT &val, looper_t &looper) const
        {
            looper.m_node = m_rax.find(val,looper);
            if (looper.m_node == NULL) return end();
            return iterator(looper);
        }
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������,����ָ���Ƿ����ɾ����Ŀն�У��
        template<class VT>
        bool erase(const VT &val)
        {
            typename raw_rax_t::ref_path_t path;
            node_t *node = m_rax.find(val,path);
            if (!node) return false;
            ct::OD(&node->key);
            return m_rax.remove(node, path);
        }
        //-------------------------------------------------
        //��ȡ��Сkey��Ӧ�ĵ�����;��Ҫ������looper������
        iterator begin(looper_t &looper) const
        {
            looper.m_node = m_rax.left(looper);
            return iterator(looper);
        }
        //���̼߳�ʹ�õĵ�������ʼ����,������Ҫ��looperʹ�����ö���
        iterator begin() const
        {
            looper_t *looper = (looper_t*)&m_dummy_begin;
            return begin(*looper);
        }
        //����ֱ�ӻ�ȡ��Сkey��Ӧ�Ľڵ�,�����к�������
        node_t* left() const { return m_rax.left(); }
        //-------------------------------------------------
        //��ȡ���key��Ӧ�ĵ�����;��Ҫ������looper������
        iterator rbegin(looper_t &looper) const
        {
            looper.m_node = m_rax.right(looper);
            return iterator(looper);
        }
        //���̼߳�ʹ�õĵ�������ʼ����,������Ҫ��looperʹ�����ö���
        iterator rbegin() const
        {
            looper_t *looper = (looper_t*)&m_dummy_begin;
            return rbegin(*looper);
        }
        //����ֱ�ӻ�ȡ���key��Ӧ�Ľڵ�,�����к�������
        node_t* right() const { return m_rax.right(); }
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(m_dummy_end); }
        iterator rend() const { return iterator(m_dummy_end); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i != end());
            //�Ȼ�ȡ��ǰ��������Ӧ�ڵ��keyֵ
            const val_t &val = *i;
            //����������
            ++i;

            //���Ҵ�ɾ��key��Ӧ�ڵ�
            typename raw_rax_t::ref_path_t path;
            node_t *node = m_rax.find(val, path);
            rx_assert(node != NULL);
            //��ɾ���ڵ�����
            ct::OD(&node->key);
            //����ɾ���ڵ㲢��������
            return m_rax.remove(node, path);
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear(bool raw=false)
        {
            if (raw) m_rax.clear();
            else
            {
                looper_t looper(*this);
                for (iterator i = begin(looper); i != end();)
                    erase(i);
            }
        }
    };

    //-----------------------------------------------------
    //Ĭ�Ͻڵ�Ϊuint32_t���͵ļ���
    //-----------------------------------------------------
    template<class key_t=uint32_t,class op_t= raxbit_op_int<key_t> >
    class raxbit_set_t :public raxbit_set_base_t<key_t, op_t>
    {
        typedef raxbit_set_base_t<key_t, op_t> super_t;
    public:
        raxbit_set_t(){}
        raxbit_set_t(mem_allotter_i &m) :super_t(m) {}
    };

}

#endif

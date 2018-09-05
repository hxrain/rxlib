#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace rx
{
    #define key_t int8_t
    #define val_t uint32_t
    typedef struct sl_node_t
    {
        key_t key;
        val_t val;
        struct sl_node_t *next[1];
    } sl_node_t;

    #define MAX_LEVEL 4

    //---------------------------------------------------------
    class raw_skiplist_t
    {
        uint32_t        m_levels;                           //��ǰ�Ѿ�ʹ�õ���߲���
        sl_node_t       *m_head;                            //ָ��ͷ���

        //-----------------------------------------------------
        //���ݸ����Ĳ�����̬�����ڵ�,����¼key��val
        static sl_node_t *create_node(uint32_t level, const key_t &key, const val_t& val)
        {
            sl_node_t *node = (sl_node_t *)malloc(sizeof(sl_node_t) + (level-1) * sizeof(sl_node_t *));
            node->val = val;
            node->key = key;

            //�½ڵ����в�ĺ���,��ʼ��ΪNULL
            for (uint32_t lvl = 0; lvl < level; lvl++)
                node->next[lvl] = NULL;

            return node;
        }
        //-----------------------------------------------------
        //�ͷŽڵ�ռ�õĿռ�
        static void free_node(sl_node_t *node)
        {
            free(node);
        }

        //-----------------------------------------------------
        //����һ������Ĳ���:>=1;<=������
        static uint32_t random_level()
        {
            uint32_t level = 1;
            while (rand() %2)
                ++level;
            return (level < MAX_LEVEL) ? level : MAX_LEVEL;
        }

        //-----------------------------------------------------
        //ժ��ָ���Ľڵ�,��������ǰ���ڵ�ĺ���ָ��
        void pick_node(sl_node_t *node, sl_node_t **update)
        {
            for (uint32_t lvl = 0; lvl < m_levels; lvl++)
            {//�ӵ��������ж�
                if (update[lvl]->next[lvl] == node)         //����ǰ�ڵ��ǰ��ָ�������
                    update[lvl]->next[lvl] = node->next[lvl];
            }

            for (uint32_t lvl = m_levels - 1; lvl >= 0; )
            {//�ٴӸ��������ж�
                if (m_head->next[lvl] == NULL)
                    m_levels--;                             //���ͷ���ĵ�ǰ�����ΪNULL,��˵��������������Ӧ�ý�����
                else
                    break;
                if (lvl-- == 0)
                    break;
            }
        }

        //-----------------------------------------------------
        //����ָ��key��ÿ���Ӧ��ǰ��,���ǵ�update��
        //����ֵ:key��Ӧ����ײ��ǰ���ڵ�
        sl_node_t *m_find_node(const key_t &key, sl_node_t **update)
        {
            sl_node_t  *node = m_head;                      //��ͷ�ڵ㿪ʼ������
            for (uint32_t lvl = m_levels - 1; lvl >= 0;)
            {//����߲���㽵������,����skiplist���㷨�������
                while (node->next[lvl] &&                   //�����ǰ�ڵ��к���
                       node->next[lvl]->key < key)          //���ҵ�ǰ�ڵ�ĺ���С��key(˵��key��Ӧ�������)
                    node = node->next[lvl];                 //��ǰ�ڵ����,׼����������
                update[lvl] = node;                         //��ǰ����ҽ�����,��¼��ǰ�ڵ�Ϊָ��keyλ�õ�ǰ��
                if ( lvl-- == 0)                            //����,���ж��Ƿ�Ӧ�ý������ѭ��
                    break;
            }
            return node;
        }
    public:
        //-----------------------------------------------------
        raw_skiplist_t()
        {
            m_levels = 1;                                   //����ĳ�ʼ����ֻ��1��
            m_head = create_node(MAX_LEVEL, 0, 0);
        }

        //-----------------------------------------------------
        void uninit()
        {
            sl_node_t *node = m_head->next[0];              //��ͷ���ĺ�����ʼ����
            while (node)
            {
                sl_node_t *next = node->next[0];            //��¼��ǰ�ڵ�ĺ���ָ��
                free_node(node);                            //�ͷŵ�ǰ�ڵ�
                node = next;                                //��ǰ�ڵ�ָ��ָ�������
            }

            free_node(m_head);                              //�ͷ�ͷ���
        }

        //-----------------------------------------------------
        //����ָ����key��val
        bool insert(const key_t &key, const val_t &val,bool duplication=false)
        {
            sl_node_t *update[MAX_LEVEL];
            sl_node_t *try_node=m_find_node(key,update)->next[0];    //����ָ��key��Ӧ�ĸ���ǰ��

            if (try_node&&try_node->key==key&&!duplication)
                return false;                               //key�Ѵ����Ҳ������ظ�����,�򷵻�

            uint32_t level = random_level();                //����һ������Ĳ���
            sl_node_t *node = create_node(level, key, val); //����������Щ�������½ڵ�
            if (!node)
                return false;

            if (level > m_levels)
            {//����½ڵ�Ĳ�������ԭ�в���,������²��ǰ��Ϊͷ���
                for (uint32_t lvl = m_levels; lvl < level; lvl++)
                    update[lvl] = m_head;
                m_levels = level;                           //���������
            }

            for (uint32_t lvl = 0; lvl < level; lvl++)
            {//��ʼ���ҽ��½ڵ�
                node->next[lvl] = update[lvl]->next[lvl];   //�½ڵ�ĺ���ָ��ǰ���ڵ�ĺ���
                update[lvl]->next[lvl] = node;              //ǰ���ڵ�ĺ���ָ���½ڵ�
            }
            return true;
        }

        //-----------------------------------------------------
        bool remove(const key_t &key)
        {
            sl_node_t *update[MAX_LEVEL];
            sl_node_t *node = m_find_node(key,update)->next[0]; //����ָ��key�ĸ���ǰ��,�����Եõ���Ӧ�Ľڵ�
            if (node && node->key == key)
            {//�����Ӧ�ڵ������key��ͬ,��˵���ҵ��˴˽ڵ�
                pick_node(node, update);                    //ժ��ָ���Ľڵ�(���нڵ����ǰ���ĵ���)
                free_node(node);                            //�ͷŽڵ��ڴ�
                return true;
            }
            return false;
        }

        //-----------------------------------------------------
        //����ָ��key��Ӧ�Ľڵ�
        sl_node_t *find(const key_t &key)
        {
            sl_node_t *node = m_head;                       //������ͷ��ʼ����
            for (uint32_t lvl = m_levels - 1; lvl >= 0;)
            {//�Ӹ߲���ײ�������
                while (node->next[lvl] != NULL)
                {//���нڵ�������ж�
                    if (node->next[lvl]->key < key)
                        node = node->next[lvl];             //�ڵ����С��key,��Ҫ����������
                    else if (node->next[lvl]->key == key)
                        return node->next[lvl];             //key��ͬ,�ҵ���.
                    else
                        break;
                }
                if (lvl-- == 0)
                    break;
            }
            return NULL;
        }
        //-----------------------------------------------------
        void print()
        {
            for (uint32_t lvl = 0; lvl < MAX_LEVEL; lvl++)
            {
                sl_node_t *node = m_head->next[lvl];
                printf("Level[%d]:", lvl);

                while (node)
                {
                    printf("%d(%d) -> ", node->key,node->val);
                    node = node->next[lvl];
                }
                printf("\n");
            }
        }

    };




}

#endif

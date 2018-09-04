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
        val_t obj;
        struct sl_node_t *next[1];
    } sl_node_t;

    #define MAX_LEVEL 2

    //---------------------------------------------------------
    class raw_skiplist_t
    {
        uint32_t        m_levels;
        sl_node_t       *m_head;

        //-----------------------------------------------------
        static sl_node_t *create_node(uint32_t level, const key_t &key, const val_t& obj)
        {
            sl_node_t *node = (sl_node_t *)malloc(sizeof(sl_node_t) + (level-1) * sizeof(sl_node_t *));
            node->obj = obj;
            node->key = key;

            for (uint32_t lvl = 0; lvl < level; lvl++)
                node->next[lvl] = NULL;

            return node;
        }
        //-----------------------------------------------------
        static void free_node(sl_node_t *node)
        {
            free(node);
        }

        //-----------------------------------------------------
        static uint32_t random_level()
        {
            uint32_t level = 1;
            while (rand() %2)
                ++level;
            return (level < MAX_LEVEL) ? level : MAX_LEVEL;
        }

        //-----------------------------------------------------
        bool create_skiplist(void)
        {
            m_levels = 1;
            m_head = create_node(MAX_LEVEL, 0, 0);
            return m_head!=NULL;
        }

        //-----------------------------------------------------
        void delete_node(sl_node_t *node, sl_node_t **update)
        {
            uint32_t lvl;
            for (lvl = 0; lvl < m_levels; lvl++)
            {
                if (update[lvl]->next[lvl] == node)
                    update[lvl]->next[lvl] = node->next[lvl];
            }

            for (lvl = lvl - 1; lvl >= 0; )
            {
                if (m_head->next[lvl] == NULL)
                    m_levels--;
                if (lvl-- == 0)
                    break;
            }
        }

        //-----------------------------------------------------
        sl_node_t *m_find_node(const key_t &key, sl_node_t **update)
        {
            sl_node_t  *node = m_head;
            for (uint32_t lvl = m_levels - 1; lvl >= 0;)
            {
                while (node->next[lvl] && node->next[lvl]->key < key)
                    node = node->next[lvl];
                update[lvl] = node;
                if ( lvl-- == 0)
                    break;
            }
            return node;
        }
    public:
        raw_skiplist_t(){create_skiplist();}

        //-----------------------------------------------------
        void uninit()
        {
            sl_node_t *node = m_head->next[0];
            free_node(m_head);

            while (node)
            {
                sl_node_t *next = node->next[0];
                free_node(node);
                node = next;
            }
        }

        //-----------------------------------------------------
        bool insert(const key_t &key, const val_t &obj)
        {
            uint32_t level = random_level();
            sl_node_t *node = create_node(level, key, obj);
            if (!node)
                return false;

            sl_node_t  *update[MAX_LEVEL];
            m_find_node(key,update);

            if (level > m_levels)
            {
                for (uint32_t lvl = m_levels; lvl < level; lvl++)
                    update[lvl] = m_head;
                m_levels = level;
            }


            for (uint32_t lvl = 0; lvl < level; lvl++)
            {
                node->next[lvl] = update[lvl]->next[lvl];
                update[lvl]->next[lvl] = node;
            }
            return true;
        }

        //-----------------------------------------------------
        bool remove(const key_t &key)
        {
            sl_node_t *update[MAX_LEVEL];
            sl_node_t *node = m_find_node(key,update);
            if (node==m_head)
                return false;

            node = node->next[0];
            if (node && node->key == key)
            {
                delete_node(node, update);
                free_node(node);
            }
            return true;
        }

        //-----------------------------------------------------
        sl_node_t *find(const key_t &key)
        {
            sl_node_t *node = m_head;
            for (uint32_t lvl = m_levels - 1; lvl >= 0;)
            {
                while (node->next[lvl] != NULL)
                {
                    if (node->next[lvl]->key < key)
                        node = node->next[lvl];
                    else if (node->next[lvl]->key == key)
                        return node->next[lvl];
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
                    printf("%d -> ", node->key);
                    node = node->next[lvl];
                }
                printf("\n");
            }
        }

    };




}

#endif

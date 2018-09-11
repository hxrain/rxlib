#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define object int

#define MAX_LEVEL 8

typedef struct _node {
	int key;
	object *obj;
	struct _node *forward[1];
} node;

typedef struct _skiplist {
	int level;
	struct _node *head;
} skiplist;

skiplist *create_skiplist(void);
void free_skiplist(skiplist *sl);
void insert(skiplist *sl, int key, object *obj);
void earse(skiplist *sl, int key);
node *find(skiplist *sl, int key);
void print(skiplist *sl);


#endif

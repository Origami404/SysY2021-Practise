#include "util.h"

typedef struct Node {
    string key; 
    string val;
    struct Node *next;
} *Node;

// 不会太慢, 应该
#define HT_SIZE (1u << 8)

typedef struct str_map {
    Node bucket[HT_SIZE];
} *str_map;

#define DEF_STR_MAP
#include "cstl.h"

Node node_create(string key, string val, Node next) {
    Node p = checked_malloc(sizeof(*p));
    *p = (struct Node) { key, val, next };
    return p;
}

// Using djb2 algorithm. See: https://stackoverflow.com/a/7666577
u32 hash(string s) {
    u32 h = 5381;
    u32 c;

    while (c = *s++) 
        h = (h << 5) + h + c;
    
    return (h & 0x00FF0000) >> 16; 
}

str_map map_create(void) {
    str_map p = checked_malloc(sizeof(*p));
    memset(p, 0, sizeof(*p));

    return p;
}

void map_set(str_map m, string key, string val) {
    u32 const h = hash(key);
    Node n = m->bucket[h];
    
    // 当当前 hash 值对应的链表长为 0 时, 直接添加
    if (!n) {
        m->bucket[h] = node_create(key, val, 0);
        return;
    }

    // 否则, 找到匹配键值的修改
    for ( ; n->next; n = n->next) {
        if (!strcmp(n->key, key)) {
            n->val = val;
            return;
        }
    }
    // 或者走到最后一个元素加入新节点
    n->next = node_create(key, val, 0);
}

string map_get(str_map m, string key) {
    Node n = m->bucket[hash(key)];

    // 找到第一个非空的并且键就是 key 的节点
    while (n) {
        if (!strcmp(n->key, key))
            return n->val;
        n = n->next;
    }

    panic("Unbound key: %s", key);
}

// TODO: 尝试循环链表? 
void map_del(str_map m, string key) {
    u32 const h = hash(key);
    Node n = m->bucket[h];

    // 若链表长度为 0
    if (!n) panic("Unbound key: %s", key);

    // 若链表长度为 1
    if (!(n->next)) {
        if (!strcmp(n->key, key)) {
            free(n);
            m->bucket[h] = 0;
            return;
        }
        panic("Unbound key: %s", key);
    }

    // 若链表长度大于 1, 每次检查 n->next
    while (n->next) {
        if (!strcmp(n->next->key, key)) {
            Node p = n->next->next;
            free(n->next);
            n->next = p;
            return;
        }
    }

    panic("Unbound key: %s", key);
}

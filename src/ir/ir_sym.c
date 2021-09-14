#include "util.h"
#include "ir.h"
#include "cstl.h"

typedef str_map IR_SymTab;

inline IR_SymTab ir_tab_create(void) { return map_create(); }
inline void ir_tab_add(IR_SymTab tab, string name, int *shape) { 
    map_set(tab, name, shape); 
}
inline int* ir_tab_get(IR_SymTab tab, string name) {
    return map_get(tab, name);
}

typedef struct IR_Scope {
    IR_SymTab sym_tab;
    struct IR_Scope *upper;
} *IR_Scope;

IR_Scope ir_scope_creates(IR_Scope upper) {
    IR_Scope p = checked_malloc(sizeof(*p));
    p->sym_tab = ir_tab_create();
    p->upper = upper;
    return p;
}

IR_Scope scope_list[IR_SCOPE_MAX_SIZE];
size_t scope_cnt = 0;
IR_Scope scope_now;

void ir_sym_push_scope(void) {
    if (scope_now == IR_SCOPE_MAX_SIZE - 1) panic("Too many scope");

    scope_now = ir_scope_creates(scope_now);
    scope_list[scope_cnt++] = scope_now;
}

void ir_sym_pop_scope(void) {
    scope_now = scope_now->upper;
}

void ir_sym_add_variable(string name, int* shape) {
    ir_tab_add(scope_now->sym_tab, name, shape);
}

int* ir_sym_get_shape(string name) {
    ir_tab_get(scope_now->sym_tab, name);
}
 

#include "util.h"
#include "ir.h"
#include "cstl.h"

typedef struct IR_VarInfo *IR_VarInfo;
static IR_VarInfo info_dup(struct IR_VarInfo info) {
    IR_VarInfo p = checked_malloc(sizeof(*p));
    memcpy(p, &info, sizeof(*p));
    return p;
}

typedef str_map IR_SymTab;

static inline IR_SymTab ir_tab_create(void) { return map_create(); }
static inline void ir_tab_add(IR_SymTab tab, string name, struct IR_VarInfo info) { 
    map_set(tab, name, info_dup(info)); 
}
static inline struct IR_VarInfo ir_tab_get(IR_SymTab tab, string name) {
    return *((IR_VarInfo)map_get(tab, name));
}

typedef struct IR_Scope {
    IR_SymTab sym_tab;
    struct IR_Scope *upper;
} *IR_Scope;

static IR_Scope ir_scope_creates(IR_Scope upper) {
    IR_Scope p = checked_malloc(sizeof(*p));
    p->sym_tab = ir_tab_create();
    p->upper = upper;
    return p;
}

IR_Scope scope_list[IR_SCOPE_MAX_SIZE];
size_t scope_cnt = 0;
IR_Scope scope_now;

void ir_info_scope_push(void) {
    if (scope_cnt == IR_SCOPE_MAX_SIZE - 1) 
        panic("Too many scope");

    scope_now = ir_scope_creates(scope_now);
    scope_list[scope_cnt++] = scope_now;
}

void ir_info_scope_pop(void) {
    scope_now = scope_now->upper;
}

void ir_info_var_set(string name, struct IR_VarInfo info) {
    ir_tab_add(scope_now->sym_tab, name, info);
}

struct IR_VarInfo ir_info_var_get(string name) {
    return ir_tab_get(scope_now->sym_tab, name);
}
 

// IR
struct IR_Code ir_list[IR_IR_MAX_SIZE];
IR_Code ir_now = ir_list;
size_t ir_now_offset = 0;

void ir_code_add(IR_Type type, string dest, string arg1, string arg2) {
    *ir_now = (struct IR_Code) { type, dest, arg1, arg2 };

    ir_now += 1;
    ir_now_offset += 1;
}

string* ir_code_add_with_undetermined_label(IR_Type type, string arg1, string arg2) {
    ir_code_add(type, 0, arg1, arg2);
    return &(ir_now->dest);
}

string ir_temporary(void) {
    static int cnt = 1;

    char buf[16];
    sprintf(buf, "%%%d", cnt++);
    return String(buf);
}


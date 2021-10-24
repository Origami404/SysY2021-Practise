#ifndef HEADER_IR_H__
#define HEADER_IR_H__

#include "util.h"
#include "cstl.h"
#include "ast.h"

typedef enum {
    IRT_MOV,             // dest = op1
    IRT_ADD,             // dest = op1 + op2
    IRT_SUB,             // dest = op1 - op2
    IRT_MUL,             // dest = op1 * op2
    IRT_DIV,             // dest = op1 / op2
    IRT_MOD,             // dest = op1 % op2
    IRT_ARG,             // if dest < 4: R(dest) = op1; else: push_stack(op1); 
    IRT_CALL,            // call label
    // IRT_CMP,             // cmp op1, op2
    IRT_JMP,             // jmp label
    IRT_JEQ,             // if op1 == op2: jmp dest
    IRT_JNE,             // if op1 != op2: jmp dest
    IRT_JLE,             // if op1 <= op2: jmp dest
    IRT_JLT,             // if op1 <  op2: jmp dest
    IRT_JGE,             // if op1 >= op2: jmp dest
    IRT_JGT,             // if op1 >  op2: jmp dest
    IRT_AND,             // dest = op1 && op2
    IRT_OR,              // dest = op1 || op2
    IRT_SAL,             // dest = op1 << op2 算数左移
    IRT_SAR,             // dest = op1 >> op2 算数右移
    IRT_STORE,           // dest[op1] = op2
    IRT_LOAD,            // dest = op1[op2]
    IRT_RET,             // return / return op1
    IRT_DATA_BEGIN,      //.data
    IRT_DATA_WORD,       //.word
    IRT_DATA_SPACE,      //.space
    IRT_DATA_END,        // nothing
    IRT_FUNCTION_BEGIN,  // FUNCTION_BEGIN
    IRT_FUNCTION_END,    // FUNCTION_END
    IRT_PHI,             // PHI
    IRT_NOOP,            // no operation
} IR_Type;

// 参数约定
// 以 . 开头的是局部 label, 以 _ 开头的是函数 label, 以 字母 开头的是暂时的原来的 label
// 以 @ 开头的是全局变量, 以 # 开头的是局部变量
// 以 数字 开头的是立即数, 以 字母 开头的是暂时的原来的名字
typedef struct IR_Code {
    IR_Type type;
    string dest, op1, op2;
} *IR_Code;
IR_Code ir_code_create(IR_Type type, string dest, string op1, string op2);

#define IR_IR_MAX_SIZE 65536
// extern IR_Code ir_list[IR_IR_MAX_SIZE];
// 当前的 IR 指令 
extern IR_Code ir_now;
extern size_t ir_now_offset;

// 生成一个新的局部变量名字, 始终以 # 开头
string ir_temporary();

// 新增一行 IR
void ir_code_add(IR_Type type, string dest, string op1, string op2);

// 增加一行 IR 并且暴露出 dest 的位置给第二轮 pass 修改
string* ir_code_add_with_undetermined_label(IR_Type type, string arg1, string arg2);

// ======================== IR Generation Context ==========================

#define IR_SCOPE_MAX_SIZE 512
// 新建作用域
void ir_info_scope_push(void);
void ir_info_scope_pop(void);

typedef vector_t(string *) HoleVec;

// 标记最近的 while
void ir_info_push_while(HoleVec start, HoleVec end);
void ir_info_pop_while(void);
HoleVec ir_info_get_nearest_while_start(void);
HoleVec ir_info_get_nearest_while_end(void);

// 变量信息
// 不是指针类型, 不用 typedef
struct IR_VarInfo {
    // shape 是一个非空的 int 数组, 表示这个变量的数组维度
    // shape[0] 表示有多少维, 接下来的shape[1], shape[2],..., shape[n]依次为该维长度
    // 做出此安排是为了方便后面计算拍平的索引
    int *shape;
    bool is_const;
    bool is_global;
    // 放在最后, 不给的话默认为 0
    int init;
};
void ir_info_var_set(string name, struct IR_VarInfo info);
struct IR_VarInfo ir_info_var_get(string name);

// 函数信息
struct IR_FuncInfo {
    Ast_FuncRetType ret_type;
    struct IR_VarInfo *arg_types;
};

void ir_info_func_set(string name, struct IR_FuncInfo);
struct IR_FuncInfo ir_info_func_get(string name);

#endif // HEADER_IR_H__
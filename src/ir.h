#ifndef HEADER_IR_H__
#define HEADER_IR_H__

#include "util.h"
#include "cstl.h"

typedef enum {
    IRT_MOV,             // dest = op1
    IRT_ADD,             // dest = op1 + op2
    IRT_SUB,             // dest = op1 - op2
    IRT_MUL,             // dest = op1 * op2
    IRT_DIV,             // dest = op1 / op2
    IRT_MOD,             // dest = op1 % op2
    IRT_CALL,            // call label
    IRT_CMP,             // cmp op1, op2
    IRT_JMP,             // jmp label
    IRT_JEQ,             // if EQ: jmp label
    IRT_JNE,             // if NE: jmp label
    IRT_JLE,             // if LE: jmp label
    IRT_JLT,             // if LT: jmp label
    IRT_JGE,             // if GE: jmp label
    IRT_JGT,             // if GT: jmp label
    IRT_AND,             // dest = op1 && op2
    IRT_OR,              // dest = op1 || op2
    IRT_SAL,             // dest = op1 << op2 算数左移
    IRT_SAR,             // dest = op1 >> op2 算数右移
    IRT_STORE,           // op1[op2] = op3
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

// 参数中以 . 开头的是 label
// 以 @ 开头的是全局变量, 以 # 开头的是局部变量
// 以 数字 开头的是立即数
typedef struct IR_Code {
    IR_Type type;
    string dest, op1, op2;
} *IR_Code;
IR_Code ir_code_create(IR_Type type, string dest, string op1, string op2);

#define IR_IR_MAX_SIZE 65536
extern IR_Code ir_list[IR_IR_MAX_SIZE];
extern IR_Code ir_now;

string ir_temporary(char prefix);
void ir_code_add(IR_Type type, string dest, string op1, string op2);

// 不是指针类型, 不用 typedef
struct IR_VarInfo {
    // shape 是一个非空的 int 数组, 表示这个变量的数组维度
    // shape[0] 表示有多少维, 接下来的shape[1], shape[2],..., shape[n]依次为该维长度
    // 做出此安排是为了方便后面计算拍平的索引
    int *shape;
    bool is_const;
    // 放在最后, 不给的话默认为 0
    int init;
};
void ir_sym_add_variable(string name, struct IR_VarInfo info);
struct IR_VarInfo ir_sym_get_info(string name);

#define IR_SCOPE_MAX_SIZE 512
void ir_sym_push_scope(void);
void ir_sym_pop_scope(void);


#endif // HEADER_IR_H__
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

typedef struct OpArg {
    enum {
        OV_Var, OV_Imm, OV_Labal
    } type;

    union {
        string var;
        string label;
        int imm;
    };
} *OpArg;
OpArg op_arg_var_create(string var);
OpArg op_arg_imm_create(int imm);
OpArg op_arg_lab_create(string label);

typedef struct IR_Code {
    IR_Type type;

    string dest;
    OpArg op1, op2;
} *IR_Code;
IR_Code ir_code_create(IR_Type type, string dest, OpArg op1, OpArg op2);

typedef struct IR_List {
    IR_Code ir;
    struct IR_List *next;
    struct IR_List *prev;
} *IR_List;
IR_List ir_list_create(IR_Code ir, IR_List next, IR_List prev);

typedef struct IR_SymTable {
    str_map table;
    struct IR_SymTable *upper;
} *IR_SymTable;
IR_SymTable ir_sym_table_create(IR_SymTable upper);
string ir_sym_lookup(IR_SymTable sym_tab, string name);

typedef struct IR_Block {
    IR_List ir_beg;
    IR_List ir_end;

    vector_t(IR_Block) succ;
    vector_t(IR_Block) pred;

    IR_SymTable sym_tab;
} *IR_Block;
IR_Block ir_block_create();



#endif // HEADER_IR_H__
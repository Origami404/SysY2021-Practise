#ifndef HEADER_AST_UNOOP_H__
#define HEADER_AST_UNOOP_H__

#include "list.h"
#include "util.h"

typedef enum Ast_OpType {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, 
    OP_EQ,  OP_NOT_EQ, OP_LESS, OP_LESS_EQ, OP_GREATER, OP_GREATER_EQ, 
    OP_LOG_NOT, OP_LOG_AND, OP_LOG_OR
} Ast_OpType;

typedef enum Ast_NodeType {
    AT_Lval = 1,

    // ---- expr ---
    AT_ExpOp, AT_ExpLval,
    AT_ExpNum, AT_ExpPutf, AT_ExpCall,

    // --- stmt ---
    AT_Block, AT_StmtExp, AT_StmtIf, AT_StmtWhile,
    AT_StmtEmpty, AT_StmtBreak, AT_StmtContinue, 
    AT_StmtReturn, AT_StmtAssign,

    // --- init ---
    AT_InitExp, AT_InitArr,

    // --- other ---
    AT_VarDef, AT_FuncDef, AT_Decl
} Ast_NodeType;

typedef enum Ast_FuncRetType {
    FRT_VOID, FRT_INT
} Ast_FuncRetType;

struct Ast_Node;
typedef struct Ast_Node *Ast_Node;

typedef struct ListAst {
    Ast_Node car;
    struct ListAst *cdr;
} *ListAst;
ListAst cons_Ast(Ast_Node car, ListAst cdr);

struct Ast_Node {
    enum Ast_NodeType type;
    union {
        struct {
            string name;
            ListAst index;
        } lval;


        // --- expr ---
        struct { 
            Ast_OpType op;
            Ast_Node arg1;
            Ast_Node arg2;
        } exp_op;

        struct {
            Ast_Node lval;
        } exp_lval;

        struct {
            int val;
        } exp_num;

        struct {
            string format;
            ListAst args;
        } exp_putf;

        struct {
            Ast_Node func;
            ListAst args;
        } exp_call;


        // --- stmt --- 
        struct {
            ListAst items;
        } block;

        struct {
            Ast_Node exp;
        } stmt_exp;

        struct {
            Ast_Node cond;
            Ast_Node if_clause;
            Ast_Node else_clause;
        } stmt_if;

        struct {
            Ast_Node cond;
            Ast_Node body;
        } stmt_while;

        struct {
            Ast_Node exp;
        } stmt_return;

        struct {
            Ast_Node lval;
            Ast_Node exp;
        } stmt_assign;


        // --- init ---
        struct {
            Ast_Node exp;
        } init_exp;

        struct {
            ListAst subs;
        } init_arr;


        // --- other ---
        struct {
            Ast_Node lval;
            Ast_Node init;
        } var_def;

        struct {
            Ast_FuncRetType ret_type;
            string func_name;
            ListAst params;
            Ast_Node body;
        } func_def;

        struct {
            bool is_const; 
            ListAst def; 
        } decl;
    } u;
};

//@ cons_header
#include <ast/cons.gen.h>


#endif // HEADER_AST_UNOOP_H__
#ifndef HEADER_AST_UNOOP_H__
#define HEADER_AST_UNOOP_H__

#include "list.h"
#include "util.h"

typedef enum Ast_ExpAddType {
    OP_ADD=1, OP_SUB, OP_MUL, OP_DIV, OP_MOD, 
} Ast_ExpAddType;

typedef enum Ast_ExpRelType {
    OP_EQ=1, OP_NOT_EQ, OP_LESS, OP_LESS_EQ, OP_GREATER, OP_GREATER_EQ, 
} Ast_ExpRelType;

typedef enum Ast_ExpLogType {
    OP_LOG_NOT=1, OP_LOG_AND, OP_LOG_OR
} Ast_ExpLogType;

typedef enum Ast_NodeType {
    AT_Lval = 1,

    // ---- expr ---
    AT_ExpAdd, AT_ExpRel, AT_ExpLog,
    AT_ExpLval, AT_ExpNum, AT_ExpPutf, AT_ExpCall,

    // --- stmt ---
    AT_Block, AT_StmtExp, AT_StmtIf, AT_StmtWhile,
    AT_StmtEmpty, AT_StmtBreak, AT_StmtContinue, 
    AT_StmtReturn, AT_StmtAssign,

    // --- init ---
    AT_InitExp, AT_InitArr,

    // --- other ---
    AT_VarDef, AT_FuncDef, AT_Decl,
} Ast_NodeType;

typedef enum Ast_FuncRetType {
    FRT_VOID=1, FRT_INT
} Ast_FuncRetType;

struct Ast_Node;
typedef struct Ast_Node *Ast_Node;

typedef struct Ast_List {
    Ast_Node car;
    struct Ast_List *cdr;
} *Ast_List;
Ast_List cons_Ast(Ast_Node car, Ast_List cdr);

struct Ast_Node {
    enum Ast_NodeType type;
    union {
        struct {
            string name;
            Ast_List index;
        } lval;


        // --- expr ---
        struct { 
            Ast_ExpAddType op;
            Ast_Node arg1; // Primary Exp
            Ast_Node arg2;
        } exp_add;

        struct {
            Ast_ExpRelType op;
            Ast_Node arg1; // Add Exp
            Ast_Node arg2;
        } exp_rel;

        struct {
            Ast_ExpLogType op;
            Ast_Node arg1; // Rel Exp
            Ast_Node arg2;
        } exp_log;

        struct {
            Ast_Node lval;
        } exp_lval;

        struct {
            string val;
        } exp_num;

        struct {
            string format;
            Ast_List args;
        } exp_putf;

        struct {
            string func;
            Ast_List args;
        } exp_call;


        // --- stmt --- 
        struct {
            Ast_List items;
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
            Ast_List subs;
        } init_arr;


        // --- other ---
        struct {
            Ast_Node lval;
            Ast_Node init;
        } var_def;

        struct {
            Ast_FuncRetType ret_type;
            string func_name;
            Ast_List params;
            Ast_Node body;
        } func_def;

        struct {
            bool is_const; 
            Ast_List def; 
        } decl;
    };
};

//@ cons_header
#include <ast/cons.gen.h.inl>

void ast_dump(FILE *out, Ast_Node node);
void ast_dump_list(FILE *out, Ast_List list);

#endif // HEADER_AST_UNOOP_H__
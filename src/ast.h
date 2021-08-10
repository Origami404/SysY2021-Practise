#ifndef HEADER_AST_H__
#define HEADER_AST_H__

#include "util.h"
#include "list.h"

typedef enum Ast_OpType {
    OP_LOG_NOT,
} *Ast_OpType;

typedef struct Ast_Node {
    
} *Ast_Node;

//--------------- Basic -----------------------------

DEF_LIST(ArrayIndexList, int)
typedef struct Ast_LVal {
    Ast_Node super;

    string name;
    ArrayInitList *index;
} *Ast_LVal;

//--------------- Expression ------------------------

typedef struct Ast_ExpNode {
    Ast_Node super;
} *Ast_ExpNode;

typedef struct Ast_UnaryExp {
    Ast_ExpNode super;

    Ast_OpType op;
    Ast_ExpNode arg1;
} *Ast_UnaryExpNode;

typedef struct Ast_BinaryExp {
    Ast_ExpNode super;

    Ast_OpType op;
    Ast_ExpNode arg1;
    Ast_ExpNode arg2;
} *Ast_BinaryExp;

typedef struct Ast_LValExp {
    Ast_ExpNode super;

    Ast_LVal lval;
} *Ast_LValExp;

typedef struct Ast_Number {
    Ast_ExpNode super;

    int num;
} *Ast_Number;

DEF_LIST(ExpList, Ast_ExpNode*)
typedef struct Ast_SpecFormPut {
    Ast_ExpNode super;

    string format;
    ExpList *args;
} *Ast_SpecFormPutf;

typedef struct Ast_FuncCallExp {
    Ast_ExpNode super;

    Ast_LVal func;
    ExpList *args;
} *Ast_FuncCallExp;



//------------------ BlockItem --------------------------

typedef struct Ast_Init {
    Ast_Node super;
} *Ast_Init;

typedef struct Ast_InitExp {
    Ast_Init super;

    Ast_ExpNode expr;
} *Ast_InitExp;

DEF_LIST(ArrayInitList, Ast_Init*)
typedef struct Ast_InitArr {
    Ast_Init super;

    ArrayInitList *subs;
} *Ast_InitArr;

// typedef enum BType {
//     BT_INT = 1
// } *BType;

typedef struct Ast_VarDef {
    Ast_Node super;

    Ast_LVal lval;
    Ast_Init init;
} *Ast_VarDef;

DEF_LIST(VarDefList, Ast_VarDef*)
typedef struct Ast_Decl {
    Ast_Node super;

    // BType btype;
    bool is_const;
    VarDefList *defs;
} *Ast_Decl;


typedef struct Ast_FuncParam {
    Ast_Node super;

    Ast_LVal param;
} *Ast_FuncParam;

struct Ast_Block;
typedef enum FuncRetType {
    FRT_INT=1, FRT_VOID
} *FuncRetType;
DEF_LIST(FuncParamList, Ast_FuncParam*)
typedef struct Ast_FuncDef {
    Ast_Node super;

    FuncRetType ret_type;
    Ast_LVal func;
    FuncParamList *params;
    struct Ast_Block body;
} *Ast_FuncDef;


//----------------------- Block & Stmt --------------------

typedef struct Ast_Stmt {
    Ast_Node super;
} *Ast_Stmt;

DEF_LIST(StmtList, Ast_Stmt*)
typedef struct Ast_Block {
    Ast_Stmt super;

    StmtList *stmts;
} *Ast_Block;

typedef struct Ast_ExpStmt {
    Ast_Stmt super;

    Ast_ExpNode exp;
} *Ast_ExpStmt;

typedef struct Ast_IfStmt {
    Ast_Stmt super;

    Ast_ExpNode cond;
    Ast_Stmt if_clause;
    Ast_Stmt else_clause;
} *Ast_IfStmt;

typedef struct Ast_WhileStmt {
    Ast_Stmt super;

    Ast_ExpNode cond;
    Ast_Stmt body;
} *Ast_WhileStmt;

typedef struct Ast_BreakStmt {
    Ast_Stmt super;
} *Ast_BreakStmt;

typedef struct Ast_ContinueStmt {
    Ast_Stmt super;
} *Ast_ContinueStmt;

typedef struct Ast_ReturnStmt {
    Ast_Stmt super;

    Ast_ExpNode *ret;
} *Ast_ReturnStmt;

#endif // HEADER_AST_H__ 
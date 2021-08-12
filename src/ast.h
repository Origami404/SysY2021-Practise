#ifndef HEADER_AST_H__
#define HEADER_AST_H__

#include "util.h"
#include "list.h"

typedef enum Ast_OpType {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, 
    OP_EQ,  OP_NOT_EQ, OP_LESS, OP_LESS_EQ, OP_GREATER, OP_GREATER_EQ, 
    OP_LOG_NOT, OP_LOG_AND, OP_LOG_OR
} *Ast_OpType;

typedef struct Ast_Node {
    
} *Ast_Node;

DEF_LIST(NodeList, Ast_Node)

//--------------- Basic -----------------------------
DEF_LIST(ExpList, Ast_ExpNode)

typedef struct Ast_LVal {
    Ast_Node super;

    string name;
    ExpList index;
} *Ast_LVal;
Ast_LVal ast_LVal(string name, ExpList indx);

//--------------- Expression ------------------------

typedef struct Ast_ExpNode {
    Ast_Node super;
} *Ast_ExpNode;

typedef struct Ast_UnaryExp {
    Ast_ExpNode super;

    Ast_OpType op;
    Ast_ExpNode arg1;
} *Ast_UnaryExp;
Ast_UnaryExp ast_UnaryExp(Ast_OpType op, Ast_ExpNode arg1);

typedef struct Ast_BinaryExp {
    Ast_ExpNode super;

    Ast_OpType op;
    Ast_ExpNode arg1;
    Ast_ExpNode arg2;
} *Ast_BinaryExp;
Ast_BinaryExp ast_BinaryExp(Ast_OpType op, Ast_ExpNode arg1, Ast_ExpNode arg2);

typedef struct Ast_LValExp {
    Ast_ExpNode super;

    Ast_LVal lval;
} *Ast_LValExp;
Ast_LValExp ast_LValExp(Ast_LVal lval); 

typedef struct Ast_Number {
    Ast_ExpNode super;

    int num;
} *Ast_Number;
Ast_Number ast_Number(int num);

typedef struct Ast_SpecFormPutf {
    Ast_ExpNode super;

    string format;
    ExpList args;
} *Ast_SpecFormPutf;
Ast_SpecFormPutf ast_SpecFormPutf(string format, ExpList args);

typedef struct Ast_FuncCallExp {
    Ast_ExpNode super;

    Ast_LVal func;
    ExpList args;
} *Ast_FuncCallExp;
Ast_FuncCallExp ast_FuncCallExp(Ast_LVal func, ExpList args);


//------------------ BlockItem --------------------------

typedef struct Ast_BlockItem {
    Ast_Node super;
} *Ast_BlockItem;

typedef struct Ast_Init {
    Ast_Node super;
} *Ast_Init;

typedef struct Ast_InitExp {
    Ast_Init super;

    Ast_ExpNode expr;
} *Ast_InitExp;
Ast_InitExp ast_InitExp(Ast_ExpNode *expr);

DEF_LIST(ArrayInitList, Ast_Init)
typedef struct Ast_InitArr {
    Ast_Init super;

    ArrayInitList subs;
} *Ast_InitArr;
Ast_InitArr ast_InitArr(ArrayInitList subs);

// typedef enum BType {
//     BT_INT = 1
// } *BType;

typedef struct Ast_VarDef {
    Ast_Node super;

    Ast_LVal lval;
    Ast_Init init;
} *Ast_VarDef;
Ast_VarDef ast_VarDef(Ast_LVal lval, Ast_Init init);

DEF_LIST(VarDefList, Ast_VarDef)
typedef struct Ast_Decl {
    Ast_BlockItem super;

    // BType btype;
    bool is_const;
    VarDefList defs;
} *Ast_Decl;
Ast_Decl ast_Decl(bool is_const, VarDefList defs);


typedef struct Ast_FuncParam {
    Ast_Node super;

    Ast_LVal param;
} *Ast_FuncParam;
Ast_FuncParam ast_FuncParam(Ast_LVal param);

struct Ast_Block;
typedef enum FuncRetType {
    FRT_INT=1, FRT_VOID
} *FuncRetType;
DEF_LIST(FuncParamList, Ast_FuncParam)
typedef struct Ast_FuncDef {
    Ast_Node super;

    FuncRetType ret_type;
    string func;
    FuncParamList params;
    struct Ast_Block *body;
} *Ast_FuncDef;
Ast_FuncDef ast_FuncDef(FuncRetType ret_type, 
                        string func, 
                        FuncParamList params, 
                        struct Ast_Block *body);

//----------------------- Block & Stmt --------------------

typedef struct Ast_Stmt {
    Ast_BlockItem super;
} *Ast_Stmt;

DEF_LIST(StmtList, Ast_Stmt)

DEF_LIST(BlockItemList, Ast_BlockItem)
typedef struct Ast_Block {
    Ast_Stmt super;

    BlockItemList stmts;
} *Ast_Block;
Ast_Block ast_Block(BlockItemList stmts);

typedef struct Ast_ExpStmt {
    Ast_Stmt super;

    Ast_ExpNode exp;
} *Ast_ExpStmt;
Ast_ExpStmt ast_ExpStmt(Ast_ExpNode exp);

typedef struct Ast_IfStmt {
    Ast_Stmt super;

    Ast_ExpNode cond;
    Ast_Stmt if_clause;
    Ast_Stmt else_clause;
} *Ast_IfStmt;
Ast_IfStmt ast_IfStmt(Ast_ExpNode cond, Ast_Stmt if_clause, Ast_Stmt else_clause);

typedef struct Ast_WhileStmt {
    Ast_Stmt super;

    Ast_ExpNode cond;
    Ast_Stmt body;
} *Ast_WhileStmt;
Ast_WhileStmt ast_WhileStmt(Ast_ExpNode cond, Ast_Stmt body);

typedef struct Ast_EmptyStmt {
    Ast_Stmt super;
} *Ast_EmptyStmt;
Ast_EmptyStmt ast_EmptyStmt(void);

typedef struct Ast_BreakStmt {
    Ast_Stmt super;
} *Ast_BreakStmt;
Ast_BreakStmt ast_BreakStmt(void);

typedef struct Ast_ContinueStmt {
    Ast_Stmt super;
} *Ast_ContinueStmt;
Ast_ContinueStmt ast_ContinueStmt(void);

typedef struct Ast_ReturnStmt {
    Ast_Stmt super;

    Ast_ExpNode *ret;
} *Ast_ReturnStmt;
Ast_ReturnStmt ast_ReturnStmt(Ast_ExpNode *ret);

#endif // HEADER_AST_H__ 
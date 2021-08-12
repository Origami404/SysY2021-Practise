%{

#include "util.h"
#include "parser.tab.h"
#include "ast.h"

extern int yylex();

%}

%union {
    // int token;
    int ival;
    string sval;

    Ast_ExpNode exp;
    Ast_Init init;
    Ast_LVal lval;
    Ast_Block block;
    Ast_Stmt stmt;

    ArrayInitList  *arr_init_list;
    ExpList        *exp_list;
    VarDefList     *var_def_list;
    FuncParamList  *func_param_list;
    StmtList       *stmt_list;
}

// %token <FieldNameInUnion> TerminalName "Comment"

%token <sval> T_IDENT "identifier" T_STR "string literal" T_PUTF "putf"
%token <ival> T_NUM   "number literal" 

%token T_CONST "const"  T_INT    "int"     T_VOID     "void"
%token T_IF    "if"     T_ELSE   "else"    T_WHILE    "while"
%token T_BREAK "break"  T_RETURN "return"  T_CONTINUE "continue"

%token T_COMMA      ","   T_SEMICOCLON  ";"
%token T_PAREN_LEFT "("   T_PAREN_RIGHT ")"
%token T_SQU_LEFT   "["   T_SQU_RIGHT   "]"
%token T_CURLY_LEFT "{"   T_CURLY_RIGHT "}"

%token T_ASSIGN "="
%token T_ADD "+" T_SUB "-" T_MUL "*" T_DIV "/" T_MOD "%"

%token T_EQ      "=="  T_NOT_EQ "!="  T_LESS       "<"
%token T_LESS_EQ "<="  T_GREATER ">"  T_GREATER_EQ ">=" 

%token T_LOG_NOT "!" T_LOG_AND "&&" T_LOG_OR "||"


%type <exp> PutfForm PrimaryExp UnaryExp MulExp AddExp 
%type <exp> RelExp EqExp LAndExp LOrExp Cond Exp
%type <lval> LVal
%type <exp_list> ArrIdx FuncArgs

%start CompUnit

%%

CompUnit: Decl CompUnit
        | FuncDef CompUnit
        | /* empty */
        ; 

//---------------- Declaration & Definition ----------------------------
FuncDef: "int"  T_IDENT "(" FuncParamList ")" Block
       | "void" T_IDENT "(" FuncParamList ")" Block;
FuncParamList: FuncParam
             | FuncParam "," FuncParamList;
FuncParam: "int" LVal;

Decl: "const" "int" Def
    | "int" Def;

Def: LVal
   | LVal "=" InitVal
   | LVal "," Def
   | LVal "=" InitVal "," Def;

InitVal: Exp
       | "{" ArrInit "}";
ArrInit: InitVal 
       | InitVal "," ArrInit;

//---------------------------- Stmt ---------------------------

Stmt: LVal "=" Exp ";"
    | Exp ";"
    | ";"
    | Block
    | IfStmt
    | WhileStmt
    | "break"      ";"
    | "continue"   ";"
    | "return"     ";"
    | "return" Exp ";"
    | PutfForm
    ;

Block: "{" BlockItemList "}" ;
BlockItemList: BlockItem BlockItemList | /* Empty */ ;
BlockItem: Decl | Stmt ;

IfStmt: "if" "(" Cond ")" Stmt
      | "if" "(" Cond ")" Stmt "else" Stmt;

WhileStmt: "while" "(" Cond ")" Stmt;



//----------------- Expression ---------------------------- 

Exp : AddExp;
Cond: LOrExp;

LOrExp: LAndExp
      |  LOrExp "||" LAndExp { $$ = ast_BinaryExp(OP_LOG_OR, $1, $3); }
      ;

LAndExp: EqExp
       | LAndExp "&&" EqExp { $$ = ast_BinaryExp(OP_LOG_AND, $1, $3); }
       ;

EqExp: RelExp
     | EqExp "==" RelExp { $$ = ast_BinaryExp(OP_EQ,     $1, $3); }
     | EqExp "!=" RelExp { $$ = ast_BinaryExp(OP_NOT_EQ, $1, $3); }
     ; 

RelExp: AddExp
      | RelExp "<"  AddExp { $$ = ast_BinaryExp(OP_LESS,       $1, $3); }
      | RelExp "<=" AddExp { $$ = ast_BinaryExp(OP_LESS_EQ,    $1, $3); }
      | RelExp ">"  AddExp { $$ = ast_BinaryExp(OP_GREATER,    $1, $3); }
      | RelExp ">=" AddExp { $$ = ast_BinaryExp(OP_GREATER_EQ, $1, $3); }
      ;

AddExp: MulExp
      | AddExp "+" MulExp  { $$ = ast_BinaryExp(OP_ADD, $1, $3); }
      | AddExp "-" MulExp  { $$ = ast_BinaryExp(OP_SUB, $1, $3); }
      ; 

MulExp: UnaryExp
      | MulExp "*" UnaryExp   { $$ = ast_BinaryExp(OP_MUL, $1, $3); }
      | MulExp "/" UnaryExp   { $$ = ast_BinaryExp(OP_DIV, $1, $3); }  
      | MulExp "%" UnaryExp   { $$ = ast_BinaryExp(OP_MOD, $1, $3); }
      ;

UnaryExp: PrimaryExp               
        | T_IDENT "(" FuncArgs ")" { $$ = ast_FuncCallExp($1, $3);      }
        | "+" UnaryExp             { $$ = ast_UnaryExp(OP_SUB, $2);     }
        | "-" UnaryExp             { $$ = ast_UnaryExp(OP_ADD, $2);     }
        | "!" UnaryExp             { $$ = ast_UnaryExp(OP_LOG_NOT, $2); }
        ;       

PrimaryExp: "(" Exp ")"   { $$ = $2               }
          | LVal          { $$ = ast_LValExp($1); }
          | T_NUM         { $$ = ast_Number($1);  }
          ;

LVal: T_IDENT ArrIdx { $$ = ast_LVal($1, $2); }
    ;

ArrIdx: ArrIdx "[" Exp "]"   { $$ = cons(ExpList, $3, $1); }
      | /* empty */          { $$ = 0;                     }
      ;

FuncArgs: LVal               { $$ = cons(ExpList, $1, 0);  }
        | LVal "," FuncArgs  { $$ = cons(ExpList, $1, $3); }
        ;

PutfForm: "putf" "(" T_STR "," FuncArgs ")" ";" { $$ = ast_SpecFormPutf($3, $5); }
        ; 
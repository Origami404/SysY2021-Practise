%{

#include "util.h"
#include "parser.y.tab.h"

extern int yylex();

%}

%union {
    // int token;
    int ival;
    string sval;
}

// %token <FieldNameInUnion> TerminalName "Comment"

%token <sval> T_IDENT "identifier" T_STR "string literal" T_PUTF "Special form: putf"
%token <ival> T_NUM   "number literal" 

%token T_CONST "'const'"  T_INT  "'int'"       T_VOID     "'void'"
%token T_IF    "'if'"     T_ELSE "'else'"      T_WHILE    "'while'"
%token T_BREAK "'break'"  T_RETURN "'return'"  T_CONTINUE "'continue'"

%token T_COMMA ":"        T_SEMICOCLON ";"
%token T_PAREN_LEFT "("   T_PAREN_RIGHT ")"
%token T_SQU_LEFT "[" T_SQU_RIGHT "]"
%token T_CURLY_LEFT "{"   T_CURLY_RIGHT "}"

%token T_ASSIGN "="
%token T_ADD "+" T_MINUS "-" T_MUL "*" T_DIVIDE "/" T_MOD "%"

%token T_EQ      "=="  T_NOT_EQ "!="  T_LESS "<"
%token T_LESS_EQ "<="  T_GREATER ">"  T_GREATER_EQ ">=" 

%token T_LOG_NOT "!" T_LOG_AND "&&" T_LOG_OR "||"

%start CompUnit

%%

CompUnit: Decl CompUnit
        | FuncDef CompUnit
        | /* empty */
        ;

//---------------- Declaration & Definition ----------------------------
FuncDef: FuncRetType T_IDENT T_PAREN_LEFT FuncParamList T_PAREN_RIGHT Block
FuncRetType: T_VOID | T_INT ;
FuncParamList: FuncParam
             | FuncParam T_COMMA FuncParamList;
FuncParam: T_INT LVal;

Decl: T_CONST T_INT Def
    | T_INT Def;

Def: LVal
   | LVal T_ASSIGN InitVal
   | LVal T_COMMA Def
   | LVal T_ASSIGN InitVal T_COMMA Def;

InitVal: Exp
       | T_CURLY_LEFT ArrInit T_CURLY_RIGHT;
ArrInit: InitVal 
       | InitVal T_COMMA ArrInit;

//---------------------------- Stmt ---------------------------

Stmt: LVal T_ASSIGN Exp T_SEMICOCLON
    | Exp T_SEMICOCLON
    | T_SEMICOCLON
    | Block
    | IfStmt
    | WhileStmt
    | T_BREAK      T_SEMICOCLON
    | T_CONTINUE   T_SEMICOCLON
    | T_RETURN     T_SEMICOCLON
    | T_RETURN Exp T_SEMICOCLON;

Block: T_CURLY_LEFT BlockItemList T_CURLY_RIGHT ;
BlockItemList: BlockItem BlockItemList | /* Empty */ ;
BlockItem: Decl | Stmt ;

IfStmt: T_IF T_PAREN_LEFT Cond T_PAREN_RIGHT Stmt
      | T_IF T_PAREN_LEFT Cond T_PAREN_RIGHT Stmt T_ELSE Stmt;

WhileStmt: T_WHILE T_PAREN_LEFT Cond T_PAREN_RIGHT Stmt;



//----------------- Expression ---------------------------- 

Exp : AddExp;
Cond: LOrExp;

LOrExp: LAndExp
      |  LOrExp T_LOG_OR LAndExp;

LAndExp: EqExp
       | LAndExp T_LOG_AND EqExp;

EqExp: RelExp
     | EqExp T_EQ RelExp
     | EqExp T_NOT_EQ RelExp;

RelExp: AddExp
      | RelExp T_LESS       AddExp
      | RelExp T_LESS_EQ    AddExp
      | RelExp T_GREATER    AddExp
      | RelExp T_GREATER_EQ AddExp;

AddExp: MulExp
      | AddExp T_ADD MulExp
      | AddExp T_MUL MulExp;

MulExp: UnaryExp
      | MulExp T_MUL    UnaryExp
      | MulExp T_DIVIDE UnaryExp
      | MulExp T_MOD    UnaryExp;

UnaryExp: PrimaryExp
        | T_IDENT T_PAREN_LEFT FuncArgs T_PAREN_RIGHT
        | T_ADD     UnaryExp 
        | T_MINUS   UnaryExp
        | T_LOG_NOT UnaryExp;

PrimaryExp: T_PAREN_LEFT Exp T_PAREN_RIGHT
          | LVal
          | T_NUM;

LVal: T_IDENT ArrIdx;
ArrIdx: T_SQU_LEFT Exp T_SQU_RIGHT ArrIdx
      | /* empty */;

FuncArgs: LVal
          | LVal T_COMMA FuncArgs;


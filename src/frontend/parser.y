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
}

// %token <FieldNameInUnion> TerminalName "Comment"

%token <sval> T_IDENT "identifier" T_STR "string literal" T_PUTF "putf"
%token <ival> T_NUM   "number literal" 

%token T_CONST "const"  T_INT  "int"       T_VOID     "void"
%token T_IF    "if"     T_ELSE "else"      T_WHILE    "while"
%token T_BREAK "break"  T_RETURN "return"  T_CONTINUE "continue"

%token T_COMMA ","        T_SEMICOCLON ";"
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
FuncDef: FuncRetType T_IDENT "(" FuncParamList ")" Block
FuncRetType: "void" | "int" ;
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
      |  LOrExp "||" LAndExp;

LAndExp: EqExp
       | LAndExp "&&" EqExp;

EqExp: RelExp
     | EqExp "==" RelExp
     | EqExp "!=" RelExp;

RelExp: AddExp
      | RelExp "<"  AddExp
      | RelExp "<=" AddExp
      | RelExp ">"  AddExp
      | RelExp ">="  AddExp;

AddExp: MulExp
      | AddExp "+" MulExp
      | AddExp "-" MulExp;

MulExp: UnaryExp
      | MulExp "*" UnaryExp
      | MulExp "/" UnaryExp
      | MulExp "%" UnaryExp;

UnaryExp: PrimaryExp
        | T_IDENT "(" FuncArgs ")"
        | "+" UnaryExp 
        | "-" UnaryExp
        | "!" UnaryExp;

PrimaryExp: "(" Exp ")"
          | LVal
          | T_NUM;

LVal: T_IDENT ArrIdx;
ArrIdx: "[" Exp "]" ArrIdx
      | /* empty */;

FuncArgs: LVal
        | LVal "," FuncArgs;

PutfForm: "putf" "(" T_STR "," FuncArgs ")" ";"
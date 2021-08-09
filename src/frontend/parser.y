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
%token T_BRACKET_LEFT "[" T_BRACKET_RIGHT "]"
%token T_BRACE_LEFT "{"   T_BRACE_RIGHT "}"

%token T_ASSIGN "="
%token T_ADD "+" T_MINUS "-" T_MUL "*" T_DIVIDE "/" T_MOD "%"

%token T_EQ      "=="  T_NOT_EQ "!="  T_LESS "<"
%token T_LESS_EQ "<="  T_GREATER ">"  T_GREATER_EQ ">=" 

%token T_LOG_NOT "!" T_LOG_AND "&&" T_LOG_OR "||"

%start CompUnit

%%

// CompUnit : Decl CompUnit
//          | FuncDef CompUnit
//          | /* Empty */
//          ;

// Decl : ConstDecl
//      | VarDecl
//      ;

// ConstDecl : 

Number : T_NUM {  }
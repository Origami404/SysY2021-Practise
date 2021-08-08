%{

#include "util.h"
#include "parser.y.tab.h"

extern int yylex();

%}

%union {
    int token;
    int ival;
    string sval;
}

// %token <FieldNameInUnion> TerminalName "Comment"

%token <sval> T_IDENT "identifier" T_STR "string literal"
%token <ival> T_NUM   "number literal" 

%token <token> T_CONST "'const'"  T_INT  "'int'"       T_VOID     "'void'"
%token <token> T_IF    "'if'"     T_ELSE "'else'"      T_WHILE    "'while'"
%token <token> T_BREAK "'break'"  T_RETURN "'return'"  T_CONTINUE "'continue'"

%token <token> T_COMMA ":"        T_SEMICOCLON ";"
%token <token> T_PAREN_LEFT "("   T_PAREN_RIGHT ")"
%token <token> T_BRACKET_LEFT "[" T_BRACKET_RIGHT "]"
%token <token> T_BRACE_LEFT "{"   T_BRACE_RIGHT "}"

%token <token> T_ASSIGN "="
%token <token> T_ADD "+" T_MINUS "-" T_MUL "*" T_DIVIDE "/" T_MOD "%"

%token <token> T_EQ      "=="  T_NOT_EQ "!="  T_LESS "<"
%token <token> T_LESS_EQ "<="  T_GREATER ">"  T_GREATER_EQ ">=" 

%token <token> T_LOG_NOT "!" T_LOG_AND "&&" T_LOG_OR "||"

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
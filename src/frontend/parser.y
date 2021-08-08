%{

#include "util.h"

extern int yylex();

%}

%union {
    int token;
    string literal;
}

%%

// CompUnit : Decl CompUnit
//          | FuncDef CompUnit
//          | /* Empty */
//          ;

// Decl : ConstDecl
//      | VarDecl
//      ;

// ConstDecl : 
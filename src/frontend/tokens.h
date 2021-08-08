#ifndef HEADER_FRONTEND_TOKEN_H__
#define HEADER_FRONTEND_TOKEN_H__

#include "util.h"

enum Token {
    // Literals
    T_IDENT = 257, T_NUM, T_STRING,

    // Keywords 
    T_CONST, T_INT, T_VOID, 
    T_IF, T_ELSE, T_WHILE, 
    T_BREAK, T_CONTINUE, T_RETURN,
    
    // Symbols
    T_COMMA, T_SEMICOCLON,            // , ;
    T_PAREN_LEFT, T_PAREN_RIGHT,      // ( )
    T_BRACKET_LEFT, T_BRACKET_RIGHT,  // [ ]
    T_BRACE_LEFT, T_BRACE_RIGHT,      // { }

    T_ASSIGN,  // = 
    
    T_ADD, T_MINUS,          // + - 
    T_MUL, T_DIVIDE, T_MOD,  // * / %

    T_EQ, T_NOT_EQ,          // == != 
    T_LESS, T_LESS_EQ,       // < <=
    T_GREATER, T_GREATER_EQ, // > >=

    T_LOG_NOT, T_LOG_AND, T_LOG_OR,     // ! && ||
};

typedef union {
    int ival;
    string sval;
} YYSTYPE;
extern YYSTYPE yylval;

#endif // HEADER_FRONTEND_TOKEN_H__
%{
#include "util.h"
#include "parser.y.tab.h"

// 维护行号地跳过 C-Style 注释
void skip_c_style_comment(void) {
    int c = 0;
    while ( (c = yyinput()) != 0 ) {
        if (c == '\n') {
            yylineno += 1;
        } else if (c == '*') {
            if ( (c = yyinput()) == '/' ) {
                return;
            } else {
                unput(c);
            }
        }
    }
}

void deal_with_putf(void) {
    const char *str_start = strchr(yytext, '"');
    const char *str_end   = strchr(str_start, '"') + 1;
    const size_t count    = str_end - str_start;

    yylval.sval = checked_malloc(count + 1);
    memcpy(yylval.sval, str_start, count);
    yylval.sval[count] = '\0';
}

%}

%option noyywrap

%%
" " { continue; }
\t  { continue; }
\n  { continue; }

"\\".*\n     { continue; }
"/*"         { skip_c_style_comment(); }

const { return T_CONST; }
int   { return T_INT;   }
void  { return T_VOID;  }

if    { return T_IF;    }
else  { return T_ELSE;  }
while { return T_WHILE; }

break     { return T_BREAK;    }
continue  { return T_CONTINUE; }
return    { return T_RETURN;   }

","  { return T_COMMA;         }
";"  { return T_SEMICOCLON;    }
"("  { return T_PAREN_LEFT;    }
")"  { return T_PAREN_RIGHT;   }
"["  { return T_BRACKET_LEFT;  }
"]"  { return T_BRACKET_RIGHT; }
"{"  { return T_BRACE_LEFT;    }
"}"  { return T_BRACE_RIGHT;   }

"="  { return T_ASSIGN; }

"+"  { return T_ADD;     }
"-"  { return T_MINUS;   }
"*"  { return T_MUL;     }
"/"  { return T_DIVIDE;  }
"%"  { return T_MOD;     }

"=="  { return T_EQ;         }
"!="  { return T_NOT_EQ;     }
"<"   { return T_LESS;       }
"<="  { return T_LESS_EQ;    }
">"   { return T_GREATER;    }
">="  { return T_GREATER_EQ; }

"!"   { return T_LOG_NOT; }
"&&"  { return T_LOG_AND; }
"||"  { return T_LOG_OR;  }

putf[\t\n]*"("\".*\",  { deal_with_putf(); return T_PUTF; }

[a-zA-Z_][0-9a-zA-Z_]* { yylval.sval = String(yytext); return T_IDENT; }

[1-9][0-9]*            { yylval.ival = (int)strtol(yytext, 0, 0); return T_NUM; }
0[0-7]*                { yylval.ival = (int)strtol(yytext, 0, 0); return T_NUM; }
0(x|X)[0-9a-fA-F]*     { yylval.ival = (int)strtol(yytext, 0, 0); return T_NUM; }
 
\".*\"                 { yylval.sval = String(yytext); return T_STRING; }

.                      { panic("Unknown token: %s", yytexxt); }
%%
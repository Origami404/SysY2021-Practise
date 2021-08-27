#include "util.h"
#include "ast.h"
#include "frontend/parser.tab.h"

Ast_List result;

int main(int argc, char **argv) {
    yyparse();
    ast_dump_list(stdout, result);
}
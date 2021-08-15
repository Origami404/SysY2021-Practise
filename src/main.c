#include "util.h"
#include "ast.h"
#include "frontend/parser.tab.h"

ListAst result;

int main(int argc, char **argv) {
    yyparse();
    ast_dump_list(stderr, result);
}
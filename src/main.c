#include "util.h"
#include "ast.h"
#include "frontend/parser.tab.h"

int main(int argc, char **argv) {
    yyparse();
    ast_dump_list(stderr, yylval.node_list);
}
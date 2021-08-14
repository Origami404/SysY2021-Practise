#include "util.h"
#include "ast.h"

void ast_dump_list(FILE *out, ListAst list) {
    fprintf(out, "[");
    for (; list; list = list->cdr) {
        ast_dump(out, list->car);
        fprintf(out, ",");
    }
    fprintf(out, "]");
}

static char const* is_const_name[] = {
    "false", "true"
};

static char const* ret_type_name[] = {
    "**error**",
    "void", "int",
};

static char const* op_name[] = {
    "**error**",
    "ADD", "SUB", "MUL", "DIV", "MOD", 
    "EQ",  "NOT_EQ", "LESS", "LESS_EQ", "GREATER", "GREATER_EQ", 
    "LOG_NOT", "LOG_AND", "LOG_OR", 
};

static char const* type_name[] = {
    "**error**", 
    
    "Lval",

    // ---- expr ---
    "ExpOp", "ExpLval",
    "ExpNum", "ExpPutf", "ExpCall",

    // --- stmt ---
    "Block", "StmtExp", "StmtIf", "StmtWhile",
    "StmtEmpty", "StmtBreak", "StmtContinue", 
    "StmtReturn", "StmtAssign",

    // --- init ---
    "InitExp", "InitArr",

    // --- other ---
    "VarDef", "FuncDef", "Decl",
};

ListAst cons_Ast(Ast_Node car, ListAst cdr) {
    ListAst p = checked_malloc(sizeof(*p));
    p->car = car;
    p->cdr = cdr;
    return p;
}

#define print_str(prompt, val) \
    fprintf(out, "'" prompt "': %s", val)
#define print_int(prompt, val) \
    fprintf(out, "'" prompt "': %d", val)
#define print_ast(prompt, val) \
    fprintf(out, "'" prompt "': "); ast_dump(out, val); fprintf(out, ", ")
#define print_list(prompt, val) \
    fprintf(out, "'" prompt "': "); ast_dump_list(out, val); fprintf(out, ",")

void ast_dump(FILE *out, Ast_Node node) {
    if (!node) {
        fprintf(out, "{}");
        return;
    }

    fprintf(out, "{");
    fprintf(out, "'type': %s,", type_name[node->type]);

    switch (node->type) {
        #include "ast/ast_print.gen.c.inl"      
    }

    fprintf(out, "}");
}

#include "ast/cons.gen.c.inl"
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

    Ast_Node node;
    ListAst node_list;
}

// %token <FieldNameInUnion> TerminalName "Comment"

%token <sval> T_IDENT "identifier" T_STR "string literal" T_PUTF "putf"
%token <ival> T_NUM   "number literal" 

%token T_CONST "const"  T_INT    "int"     T_VOID     "void"
%token T_IF    "if"     T_ELSE   "else"    T_WHILE    "while"
%token T_BREAK "break"  T_RETURN "return"  T_CONTINUE "continue"

%token T_COMMA      ","   T_SEMICOCLON  ";"
%token T_PAREN_LEFT "("   T_PAREN_RIGHT ")"
%token T_SQU_LEFT   "["   T_SQU_RIGHT   "]"
%token T_CURLY_LEFT "{"   T_CURLY_RIGHT "}"

%token T_ASSIGN "="
%token T_ADD "+" T_SUB "-" T_MUL "*" T_DIV "/" T_MOD "%"

%token T_EQ      "=="  T_NOT_EQ "!="  T_LESS       "<"
%token T_LESS_EQ "<="  T_GREATER ">"  T_GREATER_EQ ">=" 

%token T_LOG_NOT "!" T_LOG_AND "&&" T_LOG_OR "||"


%type <node_list> CompUnit

%type <node> FuncDef FuncParam
%type <node_list> FuncParamList

%type <node> Decl

%type <node> DefOne
%type <node_list> DefAny

%type <node> InitVal
%type <node_list> ArrInit

%type <node> Stmt Block IfStmt WhileStmt PutfForm
%type <node_list> BlockItemList

%type <node> PrimaryExp UnaryExp MulExp AddExp 
%type <node> RelExp EqExp LAndExp LOrExp Cond Exp
%type <node> LVal
%type <node_list> ArrIdx FuncArgs

%start CompUnit

%%

CompUnit: Decl CompUnit     { $$ = cons_Ast($1, $2);  }
        | FuncDef CompUnit  { $$ = cons_Ast($1, $2);  } 
        | /* empty */       { $$ = 0;                      }
        ; 

//---------------- Declaration & Definition ----------------------------
FuncDef: "int"  T_IDENT "(" FuncParamList ")" Block { $$ = ast_FuncDef(FRT_INT, $2, $4, $Block);  }
       | "void" T_IDENT "(" FuncParamList ")" Block { $$ = ast_FuncDef(FRT_VOID, $2, $4, $Block); }
       ;
FuncParamList: FuncParam                   { $$ = cons_Ast($1, 0);   }
             | FuncParam "," FuncParamList { $$ = cons_Ast($1, $3);  }
             ;
FuncParam: "int" LVal { $$ = $LVal; }
         ;

Decl: "const" "int" DefAny  { $$ = ast_Decl(true, $DefAny);  }
    | "int" DefAny          { $$ = ast_Decl(false, $DefAny); }
    ; 

DefOne: LVal                       { $$ = ast_VarDef($1, 0);  }
      | LVal "=" InitVal           { $$ = ast_VarDef($1, $3); }
      ;

DefAny: DefOne                     { $$ = cons_Ast($1, 0);   }
      | DefOne "," DefAny          { $$ = cons_Ast($1, $3);  }
      ;

InitVal: Exp                    { $$ = ast_InitExp($Exp); }
       | "{" ArrInit "}"        { $$ = ast_InitArr($2);   }
       ;
ArrInit: InitVal                { $$ = cons_Ast($1, 0);   }
       | InitVal "," ArrInit    { $$ = cons_Ast($1, $3);  }
       ;   

//---------------------------- Stmt ---------------------------

Stmt: Block
    | IfStmt
    | WhileStmt
    | PutfForm   
    | Exp ";"             { $$ = ast_StmtExp($Exp);       }
    | LVal "=" Exp ";"    { $$ = ast_StmtAssign($1, $3);  }
    | ";"                 { $$ = ast_StmtEmpty();         }
    | "break"      ";"    { $$ = ast_StmtBreak();         }
    | "continue"   ";"    { $$ = ast_StmtContinue();      }
    | "return"     ";"    { $$ = ast_StmtReturn(0);       }
    | "return" Exp ";"    { $$ = ast_StmtReturn($Exp);    }
    ;

Block: "{" BlockItemList "}"        { $$ = ast_Block($2); }
     ;
BlockItemList: Decl BlockItemList   { $$ = cons_Ast($1, $2);  }
             | Stmt BlockItemList   { $$ = cons_Ast($1, $2);  }
             | /* Empty */          { $$ = 0;                           }
             ;

IfStmt: "if" "(" Cond ")" Stmt             { $$ = ast_StmtIf($Cond, $5, 0); }
      | "if" "(" Cond ")" Stmt "else" Stmt { $$ = ast_StmtIf($Cond, $5, $7); }
      ;

WhileStmt: "while" "(" Cond ")" Stmt { $$ = ast_StmtWhile($Cond, $Stmt); }
         ;

PutfForm: "putf" "(" T_STR "," FuncArgs ")" ";" { $$ = ast_ExpPutf($3, $5); }
        ; 

//----------------- Expression ---------------------------- 

Exp : AddExp;
Cond: LOrExp;

LOrExp: LAndExp
      |  LOrExp "||" LAndExp { $$ = ast_ExpOp(OP_LOG_OR, $1, $3); }
      ;

LAndExp: EqExp
       | LAndExp "&&" EqExp { $$ = ast_ExpOp(OP_LOG_AND, $1, $3); }
       ;

EqExp: RelExp
     | EqExp "==" RelExp { $$ = ast_ExpOp(OP_EQ,     $1, $3); }
     | EqExp "!=" RelExp { $$ = ast_ExpOp(OP_NOT_EQ, $1, $3); }
     ; 

RelExp: AddExp
      | RelExp "<"  AddExp { $$ = ast_ExpOp(OP_LESS,       $1, $3); }
      | RelExp "<=" AddExp { $$ = ast_ExpOp(OP_LESS_EQ,    $1, $3); }
      | RelExp ">"  AddExp { $$ = ast_ExpOp(OP_GREATER,    $1, $3); }
      | RelExp ">=" AddExp { $$ = ast_ExpOp(OP_GREATER_EQ, $1, $3); }
      ;

AddExp: MulExp
      | AddExp "+" MulExp  { $$ = ast_ExpOp(OP_ADD, $1, $3); }
      | AddExp "-" MulExp  { $$ = ast_ExpOp(OP_SUB, $1, $3); }
      ; 

MulExp: UnaryExp
      | MulExp "*" UnaryExp   { $$ = ast_ExpOp(OP_MUL, $1, $3); }
      | MulExp "/" UnaryExp   { $$ = ast_ExpOp(OP_DIV, $1, $3); }  
      | MulExp "%" UnaryExp   { $$ = ast_ExpOp(OP_MOD, $1, $3); }
      ;

UnaryExp: PrimaryExp               
        | T_IDENT "(" FuncArgs ")" { $$ = ast_ExpFuncCall($1, $3);      }
        | "+" UnaryExp             { $$ = ast_ExpOp(OP_SUB, $2, 0);     }
        | "-" UnaryExp             { $$ = ast_ExpOp(OP_ADD, $2, 0);     }
        | "!" UnaryExp             { $$ = ast_ExpOp(OP_LOG_NOT, $2, 0); }
        ;       

PrimaryExp: "(" Exp ")"   { $$ = $2;              }
          | LVal          { $$ = ast_ExpLval($1); }
          | T_NUM         { $$ = ast_ExpNum($1);  }
          ;

LVal: T_IDENT ArrIdx { $$ = ast_Lval($1, $2); }
    ;

ArrIdx: ArrIdx "[" Exp "]"   { $$ = cons_Ast($3, $1);  }
      | /* empty */          { $$ = 0;                     }
      ;

FuncArgs: LVal               { $$ = cons_Ast($1, 0);   }
        | LVal "," FuncArgs  { $$ = cons_Ast($1, $3);  }
        ;


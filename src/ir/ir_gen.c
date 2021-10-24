#include "util.h"
#include "ast.h"
#include "ir.h"

// ======================= gen TAC ====================
// 此过程不对 变量/函数 原有的名字做修饰处理

// 一些判断 Ast_Node 类型的 assert
#define assert_type_exp(n) \
    assert_s(2 <= n->type && n->type <= 6, "not target type: Exp")
#define assert_type_stmt(n) \
    assert_s(7 <= n->type && n->type <= 15, "not target type: Stmt")
#define assert_type_init(n) \
    assert_s(16 <= n->type && n->type <= 17, "not target type: Init")
#define assert_type(n, t) \
    assert_s(n->type == t, "not target type: " #t)


// ===== 常量表达式求值 =====
int eval_const_op(Ast_Node n) {
    assert_type(n, AT_ExpAdd);

    const int a = eval_const_exp(n->exp_add.arg1);
    const int b = eval_const_exp(n->exp_add.arg2);

    switch (n->exp_add.op) {
        case OP_ADD: return a + b;
        case OP_SUB: return a - b;
        case OP_MUL: return a * b;
        case OP_DIV: return a / b;
        case OP_MOD: return a % b;
        default: panic("Unsupport op: %d", n->exp_add.op);
    }
}

int eval_const_exp(Ast_Node n) {
    assert_type_exp(n);

    switch (n->type) {
        case AT_ExpAdd:  return eval_const_op(n);
        case AT_ExpNum:  return n->exp_num.val;
        case AT_ExpLval: return ir_info_var_get(n->exp_lval.lval->lval.name).init;
        default: panic("Unsupport type: %d", n->type);
    }
}

IR_Type op2ir_type(Ast_ExpAddType t) {
    switch (t) {
        case OP_ADD: return IRT_ADD;
        case OP_SUB: return IRT_SUB;
        case OP_MUL: return IRT_MUL;
        case OP_DIV: return IRT_DIV;
        case OP_MOD: return IRT_MOD;
        default: panic("Unsupport OpType: %d", t);
    }
}

bool lval_is_array(Ast_Node n) {
    assert_type(n, AT_Lval);

    return n->lval.index != 0;
}

// 获取数组型 LVal 的拍扁的地址偏移量
// 只支持完全符合数组形状的 LVal Exp
// ref: "方括号个数必须和数组变量的维数相同"
// 计算出来的地址保存在生成的最后一条 IR 的 dest 里
void tac_gen_lval_addr(Ast_Node n) {
    assert_type(n, AT_Lval);
    
    // 必须得是数组型, 不能是一个裸的变量
    assert_s(!lval_is_array(n), "LVal is not an array");

    Ast_List const indexs = n->lval.index;
    string const name = n->lval.name;
    
    // 对每一个下标的表达式都计算其值, 并且把保存着其值的变量的名字存起来
    vector_t(string) sub_vars = vec_create((data_spec) {0});
    for (Ast_List i = indexs; i; i = i->cdr) {
        Ast_Node const now = i->car;
        
        // 生成计算其值的 IR 并且保存
        tac_gen_exp_int(now);
        vec_add(sub_vars, ir_now->dest);
    }

    // 判断下标跟声明是否匹配
    size_t const len = vec_len(sub_vars);
    int * const shape = ir_info_var_get(name).shape;
    assert_s(len == shape[0], "Unmatch index len");

    // 把嵌套下标拍扁
    // 如果声明是 int Array[s0][s1][s2][s3];
    // LVal 是 Array[a0][a1][a2][a3] 的话, 
    // 索引应该是: a0*s1*s2*s3 + a1*s2*s3 + a2*s3 + a3
    // 即: ((a0*s1 + a1)*s2 + a2)*s3 + a3
    // 初始值为 a0
    ir_code_add(IRT_NOOP, vec_get(sub_vars, 0), 0, 0);
    for (size_t i = 1; i < len; i++) {
        // 每一步把 上一步的值*si + ai
        ir_code_add(IRT_MUL, ir_temporary(), ir_now->dest, num2str(shape[i]));
        ir_code_add(IRT_ADD, ir_temporary(), ir_now->dest, vec_get(sub_vars, i));
    }       
}

// ===== 表达式 TAC 生成 =====

void tac_gen_exp_int(Ast_Node n) {
    assert_type_exp(n);

    // 这个函数只会在非常量表达式上被调用, which means 它不可能在全局生成 IR, 只能生成局部变量

    switch (n->type) {
        case AT_ExpAdd: {
            // 正常的表达式生成不可能生成非 int 值的表达式
            // Rel/Log 表达式只能在 if/while 的条件内出现
            tac_gen_exp_int(n->exp_add.arg1);
            string const op1 = ir_now->dest;

            tac_gen_exp_int(n->exp_add.arg2);
            string const op2 = ir_now->dest;

            string const dest = ir_temporary();
            ir_code_add(op2ir_type(n->exp_add.op), dest, op1, op2);
        } break;
        
        case AT_ExpLval: {
            Ast_Node const lval = n->exp_lval.lval;

            // 如果 LVal 不是数组, 就直接把它返回
            if (!lval_is_array(lval)) {
                // TODO: 也许可以在这里 MOV 一个?
                ir_code_add(IRT_NOOP, lval->lval.name, 0, 0);
            }

            // 生成总偏移量 并 Load 进一个临时变量里
            tac_gen_lval_addr(lval);
            ir_code_add(IRT_LOAD, ir_temporary(), lval->lval.name, ir_now->dest);
        } break;

        case AT_ExpNum: {
            string const dest = ir_temporary();
            ir_code_add(IRT_MOV, dest, n->exp_num.val, 0);
        } break;

        case AT_ExpCall: {
            vector_t(string) arg_vars = vec_create((data_spec) {0});

            // 求值所有的参数
            for (Ast_List l = n->exp_call.args; l; l = l->cdr) {
                tac_gen_exp_int(l->car);
                vec_add(arg_vars, ir_now->dest);
            }

            // 把所有的参数都加进参数列表里
            size_t const arg_len = vec_len(arg_vars);
            for (size_t i = 0; i < arg_len; i++) {
                ir_code_add(IRT_ARG, num2str(i), vec_get(arg_vars, i), 0);
            }

            // 调用函数
            ir_code_add(IRT_CALL, n->exp_call.func, 0, 0);

            vec_destory(arg_vars);
        } break;

        case AT_ExpPutf: {
            // TODO: 思考字符串字面量的储存方式
        } break;

        default: panic("Unsupport type: %d", n->type);
    }
}

void tac_gen_block(Ast_Node n) {
    assert_type(n, AT_Block);

    ir_info_scope_push();
    for (Ast_List p = n->block.items; p; p = p->cdr) {
        tac_gen_stmt(p->car);
    }
    ir_info_scope_pop();
}

void tac_gen_stmt(Ast_Node n) {
    assert_type_stmt(n);
    // TODO
    
    switch (n->type) {
        case AT_Block: tac_gen_block(n); break;
        case AT_StmtIf: tac_gen_if(n); break;
        case AT_StmtWhile: 
        case AT_StmtAssign:
        case AT_StmtBreak:
        case AT_StmtContinue:
        case AT_StmtReturn: 
        case AT_StmtExp: tac_gen_exp_int(n->stmt_exp.exp); break;
        case AT_StmtEmpty: break;
        default: panic("Unkonwn stmt type: %d", n->type);
    }
}

typedef vector_t(string *) HoleVec;

IR_Type adjust_op(Ast_ExpRelType op, bool rev) {
    if (rev) {
        switch (op) {
            case OP_EQ:          return IRT_JNE;
            case OP_NOT_EQ:      return IRT_JEQ;
            case OP_GREATER:     return IRT_JLE;
            case OP_GREATER_EQ:  return IRT_JLT;
            case OP_LESS:        return IRT_JGE;
            case OP_LESS_EQ:     return IRT_JGT;
            default: panic("Not a rel op: %d", op);
        }
    } else {
        switch (op) {
            case OP_EQ:          return IRT_JEQ;
            case OP_NOT_EQ:      return IRT_JNE;
            case OP_GREATER:     return IRT_JGT;
            case OP_GREATER_EQ:  return IRT_JGE;
            case OP_LESS:        return IRT_JLT;
            case OP_LESS_EQ:     return IRT_JLE;
            default: panic("Not a rel op: %d", op);
        }
    }
}

void cond_gen_rel(Ast_Node n, bool rev, vector_t(string *) holes) {
    // 处理条件表达式

    // 如果这个条件表达式就是一个单独的 ExpAdd, 那么就直接求值
    // 其等价于 <Add> != 0
    if (n->type != AT_ExpRel) {
        tac_gen_exp_int(n->exp_rel.arg1);
        vec_add(holes, ir_code_add_with_undetermined_label(
            adjust_op(OP_NOT_EQ, rev), ir_now->dest, num2str(0)));
        return;
    } 

    assert_type(n, AT_ExpRel);

    // 否则, 正常地按条件表达式判断
    tac_gen_exp_int(n->exp_rel.arg1);
    string const a = ir_now->dest;

    tac_gen_exp_int(n->exp_rel.arg2);
    string const b = ir_now->dest;

    vec_add(holes, 
        ir_code_add_with_undetermined_label(
            rel2jmp(adjust_op(n->exp_rel.op, rev)), a, b));
    
}


HoleVec cond_gen(Ast_Node n, bool has_not, HoleVec A, HoleVec B) {
    // 此函数是条件生成系列的第一层, 它负责拆开所有的逻辑表达式, 并调用下一层关系表达式的生成函数
    /* 经过分析, 发现所有可能的情况只有下面八种: 
     * (其中 <L> 和 <R> 为逻辑表达式/关系表达式, A/B 代表 if/else 分支的地址)
     * (比如 if <L> && <R> : goto <A> (B) 意为 "如果 <L> 且 <R> 成立, 那么去 A; (否则去 B)")
     * (之所以 "else分支" 以括号表示, 是因为去向 "else分支" 的 JMP 指令应该只在条件的 IR 的最后生成而非每一次判断子表达式都生成)
     * (但是作为递归这个信息又必须保留, 所以以括号表示. 在代码中 我并没有让函数"确定自己是最后的表达式", 而是让函数返回 "else分支")
     * (通过只返回后面的语句的 "else分支" 来实现 "确定自己是最后的" 这种功能)
     * 
     * (1) if <L> && <R> : goto <A>   (B)
     *          ==> ifnot <L> : goto <B>   (A)
     *              ifnot <R> : goto <B>   (A)
     * 
     * (2) if <L> || <R> : goto <A>   (B)
     *          ==> if <L> : goto <A>   (B)
     *              if <R> : goto <A>   (B)
     * 
     * (3) if !<L> : goto <A>   (B)
     *          ==> ifnot <L> : goto <A>   (B)
     * 
     * (4) if <Rel> : goto <A>   (B)
     *          ==> 调用 cond_gen_rel, rev = false
     * 
     * 
     * (5) ifnot <L> && <R> : goto <A>   (B)
     *          ==> ifnot <L> : goto <A>   (B)
     *              ifnot <R> : goto <A>   (B)
     * 
     * (6) ifnot <L> || <R> : goto <A>   (B)
     *          ==> if <L> : goto <B>   (A)
     *              if <R> : goto <B>   (A)
     * 
     * (7) ifnot !<L> : goto <A>   (B)
     *          ==> if <L> : goto <A>   (B)
     * 
     * (8) ifnot <Rel> : goto <A>   (B)
     *          ==> 调用 cond_gen_rel, rev = true
     * 
     * 总结上表可知, 只有当 (if &&) 或者 (ifnot ||) 的时候, 才需要切换 if/ifnot 并且交换 A/B
     */

    // 首先处理情况 4/8, 即表达式就是 <Rel> 的情况
    if (n->type != AT_ExpLog) {
        cond_gen_rel(n, has_not, A);
        return B;
    }

    assert_type(n, AT_ExpLog);
    Ast_ExpLogType const op = n->exp_log.op;

    // 接着处理情况 3/7, 即 if/ifnot !<L> 的情况
    if (op == OP_LOG_NOT) {
        return cond_gen(n->exp_log.arg1, !has_not, A, B);
    }

    // 然后处理情况 1/2/5/6
    if (has_not ^ (op == OP_LOG_AND)) {
        cond_gen(n->exp_log.arg1, !has_not, B, A);
        return cond_gen(n->exp_log.arg2, !has_not, B, A);
    } else {
        cond_gen(n->exp_log.arg1, has_not, A, B);
        return cond_gen(n->exp_log.arg2, has_not, A, B);
    }
}

vector create_empty_label_hole(void) {
    return vec_create((data_spec){ 0, 0, sizeof(string *), 0 });
}

void stmt_gen_and_fill_holes(Ast_Node n, vector_t(string *) holes) {
    assert_type_stmt(n);

    size_t const label = ir_now_offset + 1;
    tac_gen_stmt(n);

    size_t const len = vec_len(holes);
    for (size_t i = 0; i < len; i++) {
        *(string *)vec_get(holes, i) = num2str(label);
    }
}

void tac_gen_if(Ast_Node n) {
    assert_type(n, AT_StmtIf);

    // 保存生成if/else分支 IR 后需要修改的跳转语句的 "洞"
    HoleVec if_holes   = create_empty_label_hole();
    HoleVec else_holes = create_empty_label_hole();

    // 生成条件部分
    // 这部分难点在于, 要做到短路. 
    // 主要思想是分三层, 分别为 cond_gen/cond_gen_rel/tac_gen_exp_int
    // 详见对应函数的注释
    HoleVec last = cond_gen(n->stmt_if.cond, false, if_holes, else_holes);
    vec_add(last, ir_code_add_with_undetermined_label(IRT_JMP, 0, 0));
    
    // 生成 if 部分的 IR, 并且在 if 后面加一个跳转到最后的语句
    stmt_gen_and_fill_holes(n->stmt_if.if_clause, if_holes);
    string *end = ir_code_add_with_undetermined_label(IRT_JMP, 0, 0);

    // 生成 else 部分的 IR
    stmt_gen_and_fill_holes(n->stmt_if.else_clause, else_holes);

    // 在最后补上跳转到 if语句 结束的洞 
    *end = num2str(ir_now_offset + 1);

    vec_destory(if_holes);
    vec_destory(else_holes);
}

// void tac_gen_stmt(Ast_Node n) {
//     assert_type_stmt(n);

//     switch (n->type) {

//         case AT_StmtIf: {

//         } break;

//         case AT_StmtWhile: {

//         } break;


//         case AT_StmtBreak: {

//         } break;

//         case AT_StmtContinue: {

//         } break;

//         case AT_StmtEmpty: {
//             // Do nothing;
//         } break;

//         case AT_Block: { 
//             tac_gen_block(n); 
//         } break; 

//         case AT_StmtExp: {
//             tac_gen_exp_int(n->stmt_exp.exp);
//         } break;

//         case AT_StmtReturn: {
//             if (n->stmt_return.exp) {
//                 tac_gen_exp_int(n->stmt_return.exp);
//                 ir_code_add(IRT_RET, 0, ir_now->dest, 0);
//             } else {
//                 ir_code_add(IRT_RET, 0, 0, 0);
//             }
//         } break;

//         case AT_StmtAssign: {
//             Ast_Node const lval = n->stmt_assign.lval;
            
//             tac_gen_exp_int(n->stmt_assign.exp);
//             string const exp_name = ir_now->dest;

//             if (lval_is_array(lval)) {
//                 tac_gen_lval_addr(lval);
//                 ir_code_add(IRT_STORE, lval->lval.name, ir_now->dest, exp_name);
//             } else {
//                 ir_code_add(IRT_MOV, lval->lval.name, exp_name, 0);
//             }

//         } break;

//         default: panic("Unsupport type: %d", n->type);
//     }
// }

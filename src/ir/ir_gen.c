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
    assert_type(n, AT_ExpOp);

    const int a = eval_const_exp(n->exp_op.arg1);
    const int b = eval_const_exp(n->exp_op.arg2);

    switch (n->exp_op.op) {
        case OP_ADD: return a + b;
        case OP_SUB: return a - b;
        case OP_MUL: return a * b;
        case OP_DIV: return a / b;
        case OP_MOD: return a % b;
        default: panic("Unsupport op: %d", n->exp_op.op);
    }
}

int eval_const_exp(Ast_Node n) {
    assert_type_exp(n);
    assert_type(n, AT_ExpOp);
    assert_type(n, AT_ExpNum);

    switch (n->type) {
        case AT_ExpOp:   return eval_const_op(n);
        case AT_ExpNum:  return n->exp_num.val;
        case AT_ExpLval: return ir_info_var_get(n->exp_lval.lval->lval.name).init;
        default: panic("Unsupport type: %d", n->type);
    }
}

IR_Type op2ir_type(Ast_OpType t) {
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
        case AT_ExpOp: {
            tac_gen_exp_int(n->exp_op.arg1);
            string const op1 = ir_now->dest;

            tac_gen_exp_int(n->exp_op.arg2);
            string const op2 = ir_now->dest;

            string const dest = ir_temporary();
            ir_code_add(op2ir_type(n->exp_op.op), dest, op1, op2);
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

    // TODO:
}

void gen_stmt_and_fill_holes(Ast_Node n, vector_t(string *) *holes) {
    assert_type_stmt(n);

    size_t const label = ir_now_offset + 1;
    tac_gen_stmt(n);

    size_t const len = vec_len(*holes);
    for (size_t i = 0; i < len; i++) {
        *(string *)vec_get(*holes, i) = num2str(label);
    }
}

IR_Type rel_op_to_negative_ir(Ast_OpType t) {
    switch (t) {
        case OP_EQ:          return IRT_JNE;
        case OP_NOT_EQ:      return IRT_JEQ;
        case OP_LESS:        return IRT_JGE;
        case OP_LESS_EQ:     return IRT_JGT;
        case OP_GREATER:     return IRT_JLE;
        case OP_GREATER_EQ:  return IRT_JLT;
        default: panic("Not an relation op: %d", t);
    }
}

void gen_exp_cond_with_jmp(Ast_Node exp, vector_t(string *) *holes) {
    // 生成两个参数的值
    tac_gen_exp_int(exp->exp_op.arg1);
    string const a1 = ir_now->dest;

    tac_gen_exp_int(exp->exp_op.arg2);
    string const a2 = ir_now->dest;

    // 根据关系符的类型来求出什么时候应该跳转到别的部分
    ir_code_add(IRT_CMP, 0, a1, a2);
    IR_Type const neg_ir_type = rel_op_to_negative_ir(exp->exp_op.op);
    vec_add(*holes, ir_code_add_with_undetermined_label(neg_ir_type));
}

void tac_gen_stmt_if(Ast_Node n) {
    assert_type(n, AT_StmtIf);

    Ast_Node const cond = n->stmt_if.cond;
    
    // TODO: init
    vector_t(string *) if_holes;
    vector_t(string *) else_holes;
    
    switch (cond->type) {
        case AT_ExpOp: {
            Ast_OpType const op = cond->exp_op.op;
            if (is_int(op)) {
                // 按正常表达式求值并判 0
                goto gen_exp;
            } else if (is_relation(op)) {
                gen_exp_cond_with_jmp(cond, &else_holes);
            } else if (is_logic(op)) {
                switch (op) {
                    case OP_LOG_NOT: 
                        
                    case OP_LOG_AND:
                    case OP_LOG_OR: 
                    default: panic("Not a logic op type: %d", op);
                }
            } else panic("Unknown OP type: %d", op);

        } break;

        case AT_ExpNum: {
            int const num = cond->exp_num.val;

            // 直接做死代码优化 跳过正常的两段代码生成
            tac_gen_stmt(num ? n->stmt_if.if_clause : n->stmt_if.else_clause);
            goto clear_up;   
        } break;

        case AT_ExpCall: // fall down
        case AT_ExpLval: {
        gen_exp: 
            // 计算出表达式的值
            tac_gen_exp_int(n);

            // 比较并生成跳转指令
            ir_code_add(IRT_CMP, 0, ir_now->dest, num2str(0));

            // 如果等于 0 就直接跳转到 else
            vec_add(else_holes, ir_code_add_with_undetermined_label(IRT_JEQ));
        } break;

        
    }

   gen_stmt_and_fill_holes(n->stmt_if.if_clause, &if_holes);
   string * const end_hole = ir_code_add_with_undetermined_label(IRT_JMP);
   gen_stmt_and_fill_holes(n->stmt_if.else_clause, &else_holes);
   *end_hole = num2str(ir_now_offset + 1);

clear_up:
    vec_destory(if_holes);
    vec_destory(else_holes);
}

void tac_gen_stmt(Ast_Node n) {
    assert_type_stmt(n);

    switch (n->type) {

        case AT_StmtIf: {

        } break;

        case AT_StmtWhile: {

        } break;


        case AT_StmtBreak: {

        } break;

        case AT_StmtContinue: {

        } break;

        case AT_StmtEmpty: {
            // Do nothing;
        } break;

        case AT_Block: { 
            tac_gen_block(n); 
        } break; 

        case AT_StmtExp: {
            tac_gen_exp_int(n->stmt_exp.exp);
        } break;

        case AT_StmtReturn: {
            if (n->stmt_return.exp) {
                tac_gen_exp_int(n->stmt_return.exp);
                ir_code_add(IRT_RET, 0, ir_now->dest, 0);
            } else {
                ir_code_add(IRT_RET, 0, 0, 0);
            }
        } break;

        case AT_StmtAssign: {
            Ast_Node const lval = n->stmt_assign.lval;
            
            tac_gen_exp_int(n->stmt_assign.exp);
            string const exp_name = ir_now->dest;

            if (lval_is_array(lval)) {
                tac_gen_lval_addr(lval);
                ir_code_add(IRT_STORE, lval->lval.name, ir_now->dest, exp_name);
            } else {
                ir_code_add(IRT_MOV, lval->lval.name, exp_name, 0);
            }

        } break;

        default: panic("Unsupport type: %d", n->type);
    }
}

#include "llvmgen.h"

/***
 * suppose all int are i32 formmated 
 * 
 ***/

FILE* fp;
int vreg_cnt = 1;
int cur_section_num = 0;
int ifScopeLevel = 0;
int elseScopeLevel = 0;
long if_refine_pos[MAX_SECTIONS / 3]; //if and while use together
long else_refine_pos[MAX_SECTIONS / 3];
int loop_sections[MAX_SECTIONS / 2];
NonTerminal nont_stack[STACK_SIZE][MAX_NONT_NUM];
int nont_pos[STACK_SIZE] = {0};
int row = 0;

void llvmgen_init() {
    fp = fopen("output.ll", "w");
}

void llvmTraverse(struct ASTnode* node) {
    switch (node->name)
    {
    case T_RSB:
        printf("pass\n");
        break;

    case T_WHILE:
        enter_while();
    case T_IF: {
        struct ASTnode* i;
        i = node->sibling->sibling;
        struct ASTnode* op_stack[50];
        union exp_value cond_val, init_sym;
        int parent[50];
        int preced = 0;
        expandExpNode(i, op_stack, parent, &preced);
        //now calculate every operators one by one 
        if(preced != 0) {
            cond_val = calculateExp(op_stack, parent, preced);
            //TODO
        }
        else {
            for(; i->name != T_exp_atomic; i = i->child);
            if(i->child->value[0] <= '9') {
                cond_val.constant = atoi(i->child->value);
            }
            else {
                strcpy(cond_val.symbol, "%");
                strcat(cond_val.symbol, i->child->value);
                rw_var(&cond_val, Load, NULL, 1);

            }
        }
        if(cond_val.symbol[0] == '%') {
            start_scope(cond_val, 1);
        }
        else {
            start_scope(cond_val, 0);
        }
               //IF->LSB->exp->RSB->else_stmt
        node = node->sibling->sibling->sibling->sibling;
        break;
    }

    case T_RLB:
        if(nont_stack[row - 2][nont_pos[row - 2] - 1] == T_func_stmt) {
            FuncEnd();
        }
        break;
    
    case T_func_stmt:
        FuncBegin(symTable->table[hash(node->child->sibling->value)]);
        break;

    case T_local_var_stmt: {
        NonTerminal cur_type;
        char* cur_sym;
        //处理最简单实现 不考虑const 指针等类型
        //      local_var_stmt->type->type_const->type_primary->"type"
        cur_type = node->child->child->child->child->name;
        //暂时不处理int a, b, c这种情况（symbol table有bug 待修复）
        //local_var_stmt->type->local_var_list->var_declar->"NAME"
        cur_sym = node->child->sibling->child->child->value;
        VarDeclar(cur_type, cur_sym);
        //暂不处理数组定义
        //T_local_var_stmt->NAME
        node = node->child->sibling->child->child;
        if(node->sibling != NULL) {
            struct ASTnode* exp_node = node->sibling->sibling, *j;
            union exp_value init_val, init_sym;
            int mode = 0;
            struct ASTnode* op_stack[50];
            int parent[50];
            int preced = 0;
            expandExpNode(exp_node, op_stack, parent, &preced);
            strcpy(init_sym.symbol, "%");
            strcat(init_sym.symbol, node->value);
            if(preced != 0) {
                init_val = calculateExp(op_stack, parent, preced);
                if(init_val.symbol[0] == '%') {
                    rw_var(&init_sym, Store, &init_val, 1);
                }
                else {
                    rw_var(&init_sym, Store, &init_val, 0);
                }
            }
            else {
                for(j = exp_node; j->name != T_exp_atomic; j = j->child);
                if(j->child->value[0] <= '9') {
                    init_val.constant = atoi(j->child->value);
                    rw_var(&init_sym, Store, &init_val, 0);
                }
                else {
                    strcpy(init_val.symbol, "%");
                    strcat(init_val.symbol, j->child->value);
                    rw_var(&init_val, Load, NULL, 1);
                    rw_var(&init_sym, Store, &init_val, 1);
                }
            }
        }
        fflush(fp);
        nont_pos[row] = 0;
        row--;
        return;
    }

    //处理四则运算
    case T_expr: {
        //stacking operators
        //如何展开???
        struct ASTnode* op_stack[50];
        int parent[50];
        int preced = 0;
        expandExpNode(node, op_stack, parent, &preced);
        //now calculate every operators one by one 
        calculateExp(op_stack, parent, preced);
        nont_pos[row] = 0;
        row--;
        return;
    }

    case T_RETURN: {
        struct ASTnode* i;
        i = node->sibling->child;
        if(i->sibling != NULL) {
            struct ASTnode* op_stack[50];
            union exp_value return_val, init_sym;
            int parent[50];
            int preced = 0;
            expandExpNode(i, op_stack, parent, &preced);
            //now calculate every operators one by one 
            if(preced != 0) {
                return_val = calculateExp(op_stack, parent, preced);
                if(return_val.symbol[0] == '%') {
                    write_return(&return_val, 2);
                }
                else {
                    write_return(&return_val, 1);
                }
            }
            else {
                for(; i->name != T_exp_atomic; i = i->child);
                if(i->child->value[0] <= '9') {
                    return_val.constant = atoi(i->child->value);
                    write_return(&return_val, 1);
                }
                else {
                    strcpy(return_val.symbol, "%");
                    strcat(return_val.symbol, i->child->value);
                    rw_var(&return_val, Load, NULL, 1);
                    write_return(&return_val, 2);
                }
            }
        }
        else {
            write_return(NULL, 3);
        }
        nont_pos[row] = 0;
        row--;
        return;
    }

    default:
        printf("pass\n");
        break;
    }
    fflush(fp);

    unsigned char isPushed = 0;
	if(node->child!=0) {
        nont_stack[row][nont_pos[row]] = node->name;
        nont_pos[row]++;
        row++;
        isPushed = 1;
		llvmTraverse(node->child);
    }

    //handle condition and iteration statement
    if(node->name == T_stmt_block) {
        if(nont_stack[row][nont_pos[row] - 2] == T_ELSE) {
            refine_else_label();
            end_scope(1, 0);
        }
        else if(nont_stack[row - 1][nont_pos[row - 1] - 1] == T_else_stmt) {
            refine_label();
            if(node->sibling == 0) {
                end_scope(0, 0);
            }
            else {
                else_scope();
            }
        }
        else if(nont_stack[row - 1][0] == T_iter_stmt) {
            refine_label();
            end_scope(0, 1);
        }
        fflush(fp);
    }
    
	if(node->sibling!=0) {
        if(!isPushed) {
            nont_stack[row][nont_pos[row]] = node->name;
            nont_pos[row]++;
        }
        llvmTraverse(node->sibling);
    }
    else {
        nont_pos[row] = 0;
        row--;
    }
}

void expandExpNode(struct ASTnode* node, struct ASTnode** op_stack,
                    int* parent, int* preced)
{
    struct ASTnode* i;
    unsigned char isExpanded[50];
    memset(isExpanded, -1, sizeof(isExpanded));
    memset(parent, -1, sizeof(int) * 50);
    int last_pos = *preced;
    doExpand(node, op_stack, preced);
    memset(isExpanded, 0, *preced);
    for(int k = last_pos + 1; k <= *preced - 1; k++) {
        parent[k] = k - 1;
    }
    last_pos = *preced;
    while(1) {
        int j;
        for (j = *preced - 1; isExpanded[j] != 1 && j >= 0; j--) {
            if(op_stack[j]->name == T_unary_op) {
                doExpand(op_stack[j]->sibling, op_stack, preced);
            }
            else if(op_stack[j]->name == T_exp_assign) {
                doExpand(op_stack[j], op_stack, preced);
            }
            else {
                if(op_stack[j]->name != T_exp_postfix
                    || op_stack[j]->sibling->sibling->name == T_expr) {
                    doExpand(op_stack[j]->sibling->sibling, op_stack, preced);
                }
            }
            if(*preced - last_pos != 0) {
                memset(isExpanded + last_pos, 0, *preced - last_pos);
                if(op_stack[j]->name == T_exp_postfix
                    && op_stack[j]->sibling->name == T_LSB) {
                    for(int k = last_pos; k <= *preced - 1; k++) {
                        parent[k] = j;
                    }
                }
                else {
                    int k = last_pos;
                    parent[k] = j;
                    for(k = last_pos + 1; k <= *preced - 1; k++) {
                        parent[k] = k - 1;
                    }
                }
                last_pos = *preced;
            }
            isExpanded[j] = 1;
        }
        if(j == *preced - 1) {
            break;
        }
    }
}

union exp_value calculateExp(struct ASTnode** op_stack, int* parent, int preced) {
    union exp_value res[50];
    for(int i = 0; i < 50; i++)
        strcpy(res[i].symbol, "%");
    for (int i = preced - 1; i >= 0; i--) {
        int cnt = 0;
        int index[MAX_FUNC_ARGS];
        for(int j = i + 1; j <= preced - 1; j++) {
            if(parent[j] == i) {
                index[cnt++] = j;
            }
        }
        struct ASTnode* j;
        union exp_value a, b;
        unsigned char mode = 0;
        switch (cnt)
        {
            case 0: {
                //x = ...
                if(op_stack[i]->name == T_exp_unary) {
                    for(j = op_stack[i]->child; j->name != T_exp_atomic; j = j->child);
                    strcpy(a.symbol, "%");
                    strcat(a.symbol, j->child->value);
                    for(j = op_stack[i]->sibling->sibling; j->name != T_exp_atomic; j = j->child);
                    if(j->child->value[0] <= '9') {
                        res[i].constant = b.constant = atoi(j->child->value);
                        rw_var(&a, Store, &b, 0);
                    }
                    else {
                        strcpy(b.symbol, "%");
                        strcat(b.symbol, j->child->value);
                        rw_var(&b, Load, NULL, 1);
                        strcpy(res[i].symbol, b.symbol);
                        rw_var(&a, Store, &b, 1);
                    }
                }
                else if(op_stack[i]->name == T_unary_op) {
                    for(j = op_stack[i]->sibling->child; j->name != T_exp_atomic; j = j->child);
                    if(j->child->value[0] <= '9') {
                        a.constant = atoi(j->child->value);
                        examine_unary_exp(&res[i], op_stack[i]->child->name, a, 0);
                    }
                    else {
                        strcpy(a.symbol, "%");
                        strcat(a.symbol, j->child->value);
                        rw_var(&a, Load, NULL, 1);
                        examine_unary_exp(&res[i], op_stack[i]->child->name, a, 1);
                    }
                }
                else if(op_stack[i]->name == T_exp_postfix) {
                    if(op_stack[i]->sibling->name == T_LSB) {
                        struct symbol* func;
                        func = symTable->table[hash(op_stack[i]->child->child->value)];
                        write_call(NULL, NULL, func, i, 0);
                    }
                    else {
                        //TODO INC/DEC
                    }
                }
                else if(op_stack[i]->name == T_expr || op_stack[i]->name == T_exp_assign) {
                    for(j = op_stack[i]->child; j->name != T_exp_atomic; j = j->child);
                    if(j->child->value[0] <= '9') {
                        res[i].constant = atoi(j->child->value);
                    }
                    else {
                        strcpy(res[i].symbol, "%");
                        strcat(res[i].symbol, j->child->value);
                        rw_var(&res[i], Load, NULL, 1);
                    }
                }
                else {
                    for(j = op_stack[i]->child; j->name != T_exp_atomic; j = j->child);
                    if(j->child->value[0] <= '9') {
                        a.constant = atoi(j->child->value);
                    }
                    else {
                        strcpy(a.symbol, "%");
                        strcat(a.symbol, j->child->value);
                        mode |= 0b10;
                        rw_var(&a, Load, NULL, 1);
                    }
                    for(j = op_stack[i]->sibling->sibling->child; j->name != T_exp_atomic; j = j->child);
                    if(j->child->value[0] <= '9') {
                        b.constant = atoi(j->child->value);
                    }
                    else {
                        strcpy(b.symbol, "%");
                        strcat(b.symbol, j->child->value);
                        mode |= 0b1;
                        rw_var(&b, Load, NULL, 1);
                    }
                    examine_binary_exp(&res[i], op_stack[i]->sibling->name, a, b, mode);
                }
                break;
            }
            
            case 1: {
                if(op_stack[i]->name == T_exp_unary) {
                    for(j = op_stack[i]->child; j->name != T_exp_atomic; j = j->child);
                    strcpy(a.symbol, "%");
                    strcat(a.symbol, j->child->value);
                    if(res[index[0]].symbol[0] == '%') {
                        strcpy(res[i].symbol, res[index[0]].symbol);
                    }
                    else {
                        res[i].constant = res[index[0]].constant;
                    }
                    rw_var(&a, Store, &res[index[0]], 1);
                }
                else if(op_stack[i]->name == T_unary_op) {
                    if(res[index[0]].symbol[0] == '%') {
                        examine_unary_exp(&res[i], op_stack[i]->child->name, res[index[0]], 1);
                    }
                    else {
                        examine_unary_exp(&res[i], op_stack[i]->child->name, res[index[0]], 0);
                    }
                }
                else if(op_stack[i]->name == T_exp_postfix) {
                    if(op_stack[i]->sibling->name == T_LSB) {
                        struct symbol* func;
                        func = symTable->table[hash(op_stack[i]->child->child->value)];
                        write_call(res, index, func, i, cnt);
                    }
                    else {
                        //TODO INC/DEC
                    }
                }
                else if(op_stack[i]->name == T_expr || op_stack[i]->name == T_exp_assign) {
                    res[i] = res[index[0]];
                }
                else {
                //检查左边是否存在sibling 如果存在 说明左边就是res的结果
                    for(j = op_stack[i]->child; j->sibling == NULL
                        && j->name != T_exp_atomic; j = j->child);
                    if(j->name != T_exp_atomic) {//左边是更深的exp
                        for(j = op_stack[i]->sibling->sibling;
                            j->name != T_exp_atomic; j = j->child);
                        if(j->child->value[0] <= '9') {
                            b.constant = atoi(j->child->value);
                        }
                        else {
                            strcpy(b.symbol, "%");
                            strcat(b.symbol, j->child->value);
                            mode |= 0b1;
                            rw_var(&b, Load, NULL, 1);
                        }
                        if(res[index[0]].symbol[0] == '%') {
                            mode |= 0b10;
                        }
                        examine_binary_exp(&res[i], op_stack[i]->sibling->name, res[index[0]], b, mode);
                    }
                    else {
                        for(; j->name != T_exp_atomic; j = j->child);
                        if(j->child->value[0] <= '9') {
                            a.constant = atoi(j->child->value);
                        }
                        else {
                            strcpy(a.symbol, "%");
                            strcat(a.symbol, j->child->value);
                            mode |= 0b10;
                            rw_var(&a, Load, NULL, 1);
                        }
                        if(res[index[0]].symbol[0] == '%') {
                            mode |= 0b1;
                        }
                        examine_binary_exp(&res[i], op_stack[i]->sibling->name, a, res[index[0]], mode);
                    }
                }
                break;
            }

            case 2:
                if(op_stack[i]->name == T_exp_postfix) {
                    struct symbol* func;
                    func = symTable->table[hash(op_stack[i]->child->child->value)];
                    write_call(res, index, func, i, cnt);
                }
                else {
                    examine_binary_exp(&res[i], op_stack[i]->sibling->name, res[index[0]], res[index[1]], 3);
                }
                break;

            default://cnt > 2
                if(op_stack[i]->name == T_exp_postfix) {
                    struct symbol* func;
                    func = symTable->table[hash(op_stack[i]->child->child->value)];
                    write_call(res, index, func, i, cnt);
                }
                break;
        }
        fflush(fp);
    }
    return res[0];
}


void doExpand(struct ASTnode* node, struct ASTnode** op_stack, int* preced) {
    struct ASTnode *i, *last_exp_node;
    unsigned char flag = 0, flag2 = 0;
    for(i = node->child; i->name != T_exp_atomic; i = i->child) {
        if(i->sibling != NULL) {
            flag2 = 0;
            op_stack[(*preced)++] = i;
            if(i->child->name == T_exp_assign) {
                last_exp_node = i->child;
                i = i->child;
                flag2 = 1;
            }
        }
        if(i->name == T_unary_op) {
            flag = 1;
            break;
        }
    }
    if(flag != 1) {
        while(i->child->sibling != NULL) {//if exp_atomic->LSB exp RSB
            for(i = i->child->sibling->child; i->name != T_exp_atomic; i = i->child) {
                if(i->sibling != NULL) {
                    op_stack[(*preced)++] = i;
                }
                if(i->name == T_unary_op) {
                    flag = 1;
                    break;
                }
            }
            if(flag == 1) {
                break;
            }
        }
    }
    if(flag2 == 1) {
        op_stack[(*preced)++] = last_exp_node;
    }
}

void FuncBegin(struct symbol* func) {
    struct type* para_list[MAX_FUNC_ARGS];
    struct funcParaList* para_node;
    int i;
    switch(func->vartype.varType) {// int
        case TYPE_INT:
        fprintf(fp, "define dso_local i32 @%s(", func->vartype.name);
            break;
        case TYPE_VOID:
            fprintf(fp, "define dso_local void @%s(", func->vartype.name);
            break;
        default:
            printf("pass\n");
            break;
    }
    for(para_node = func->paraList, i = 0; para_node != NULL; para_node = para_node->next, i++) {
        para_list[i] = para_node->para;
    }
    if(i >= 1) {//at least looped once
        union exp_value s, t;

        int j = --i;
        switch (para_list[j]->varType)
        {
        case TYPE_INT:
            fprintf(fp, "i32 %%%d", vreg_cnt++);
            break;
        
        //wait to expand for other parameter types

        default:
            break;
        }
        for(j = i - 1; j >= 0; j--) {
            switch (para_list[j]->varType)
            {
            case TYPE_INT:
                fprintf(fp, ", i32 %%%d", vreg_cnt++);
                break;
        
            //wait to expand for other parameter types

            default:
                break;
            }
        }
        fprintf(fp, ") #0 {\n");
        int k = 1;
        for(j = i; j >= 0; j--, k++) {
            strcpy(s.symbol, "%");
            strcat(s.symbol, para_list[j]->name);
            switch (para_list[j]->varType)
            {
            case TYPE_INT:
                fprintf(fp, "  %s = alloca i32, align 4\n", s.symbol);
                strcpy(t.symbol, "%");
                sprintf(t.symbol + 1, "%d", k);
                rw_var(&s, Store, &t, 1);
                break;
            
            //wait to expand for other parameter types

            default:
                break;
            }
            fflush(fp);
        }
    }
    else
    {
        fprintf(fp, ") #0 {\n");
    }
    
}

void FuncEnd() {
    //reset virtual registers and sections
    vreg_cnt = 1;
    cur_section_num = 0;
    fprintf(fp, "}\n\n");
}

void VarDeclar(NonTerminal cur_type, char* cur_sym) {
    //future work:
    //do array declar
    //do global var declar
    switch (cur_type)
    {
    case T_INT:
        fprintf(fp, "  %%%s = alloca i32, align 4\n", cur_sym);
        break;
    
    default:
        break;
    }
}

void start_scope(union exp_value res, unsigned char mode) {
    if(mode == 0) {
        fprintf(fp, "  br i1 %d, label %%L%d, label %%L", res.constant, cur_section_num);
    }
    else {
        fprintf(fp, "  br i1 %s, label %%L%d, label %%L", res.symbol, cur_section_num);
    }
    if_refine_pos[ifScopeLevel] = ftell(fp);
    fprintf(fp, "    \n\nL%d:\n", cur_section_num);
    cur_section_num++;
    ifScopeLevel++;
}

void enter_while() {
    fprintf(fp, "  br label %%L%d\n\n", cur_section_num);
    loop_sections[ifScopeLevel] = cur_section_num;
    fprintf(fp, "L%d:\n", cur_section_num++);
}

void else_scope() {
    //fprintf(fp, "br label %%L%d\n\n", cur_section_num + 1);
    //fprintf(fp, "%d:\n", cur_section_num++);
    //similar as write_branch
    fprintf(fp, "  br label %%L");
    else_refine_pos[elseScopeLevel] = ftell(fp);
    fprintf(fp, "    \n\nL%d:\n", cur_section_num++);
    elseScopeLevel++;
}

int end_scope(int is_else, int is_iter) {
    if(is_iter) {
        fprintf(fp, "  br label %%L%d\n\n", loop_sections[ifScopeLevel]);
    }
    else {
        fprintf(fp, "  br label %%L%d\n\n", cur_section_num);
    }
    fprintf(fp, "L%d:\n", cur_section_num++);
    ifScopeLevel--;
    if(is_else)
        elseScopeLevel--;
}

void refine_label() {
    long cur_pos = ftell(fp);
    fseek(fp, if_refine_pos[ifScopeLevel - 1], SEEK_SET);
    fprintf(fp, "%d", cur_section_num);
    fseek(fp, cur_pos, SEEK_SET);
}

void refine_else_label() {
    long cur_pos = ftell(fp);
    fseek(fp, else_refine_pos[elseScopeLevel - 1], SEEK_SET);
    fprintf(fp, "%d", cur_section_num);
    fseek(fp, cur_pos, SEEK_SET);
}


void rw_var(union exp_value* target, enum action a,
            union exp_value* res_to_store, int mode) {
    switch (a)
    {
        case Load:
            /*
                if(sym->attr.array) {
                    do load array action(getelemenptr)
                }
                else if(CurrentScope == &globalSymTab) {
                    load global var
                }
                else {
                    original place
                }
            */
            //default loading i32(int) type
            fprintf(fp, "  %%%d = load i32, i32* %s, align 4\n", vreg_cnt, target->symbol);
            sprintf(target->symbol + 1, "%d", vreg_cnt++);
            break;
        
        case Store:
            /*
            if(sym->attr.array) {
                do load array action(getelemenptr)
            }
            else if(CurrentScope == &globalSymTab) {
                load global var
            }
            else {
                original place    
            }
            */
            //default loading i32(int) type
            if(mode == 0) {
                fprintf(fp, "  store i32 %d, i32* %s, align 4\n",
                        res_to_store->constant, target->symbol);
            }
            else {
                fprintf(fp, "  store i32 %s, i32* %s, align 4\n",
                        res_to_store->symbol, target->symbol);
            }
            break;
    }
}

void write_return(union exp_value* res, int mode) {
    switch (mode)
    {
    case 1:
        fprintf(fp, "  ret i32 %d\n", res->constant);
        break;
    case 2:
        fprintf(fp, "  ret i32 %s\n", res->symbol);
        break;
    case 3:
        fprintf(fp, "  ret void\n");
        break;
    }
}

void write_call(union exp_value* res, int* index, struct symbol* func,
                int cur_op, int cnt) {
    switch(func->vartype.varType) {
        case TYPE_INT:
            fprintf(fp, "  %%%d = call i32 @%s(", vreg_cnt, func->vartype.name);
            sprintf(res[cur_op].symbol + 1, "%d", vreg_cnt++);
            break;
        
        case TYPE_VOID:
            fprintf(fp, "  call void @%s(", func->vartype.name);
            break;
        
        default:
            printf("pass\n");
            break;
    }
    int i;
    for(i = cnt - 1; i > 0; i--) {
        if(res[index[i]].symbol[0] == '%') {
            fprintf(fp, "i32 %s, ", res[index[i]].symbol);
        }
        else {
            fprintf(fp, "i32 %d, ", res[index[i]].constant);
        }
    }
    if(cnt != 0) {
        if(res[index[i]].symbol[0] == '%') {
            fprintf(fp, "i32 %s)\n", res[index[i]].symbol);
        }
        else {
            fprintf(fp, "i32 %d)\n", res[index[i]].constant);
        }
    }
    else {
        fprintf(fp, ")\n");
    }
}

void examine_unary_exp(union exp_value* res, NonTerminal operators,
                        union exp_value opa, unsigned char mode) {
    switch (operators)
    {
    case T_PLUS:
        switch (mode)
        {
        case 0: res->constant = opa.constant; break;
        case 1: strcpy(res->symbol, opa.symbol); break;
        }
        break;

    case T_MINUS:
        switch (mode)
        {
        case 0: res->constant = -opa.constant; break;
        case 1:
            fprintf(fp, "  %%%d = sub nsw i32 0, %s\n", vreg_cnt, opa.symbol);
            sprintf(res->symbol + 1, "%d", vreg_cnt++);
            break;
        }
        break;
    
    case T_B_AND:
        printf("pass\n");
        break;
    
    case T_L_NOT:
        switch (mode)
        {
        case 0: res->constant = !opa.constant; break;
        case 1:
            fprintf(fp, "  %%%d = icmp ne i32 %s, 0\n", vreg_cnt, opa.symbol);
            fprintf(fp, "  %%%d = xor i1 %%%d, true\n", vreg_cnt + 1, vreg_cnt);
            vreg_cnt += 1;
            fprintf(fp, "  %%%d = zext i1 %%%d to i32\n", vreg_cnt + 1, vreg_cnt);
            vreg_cnt += 1;
            sprintf(res->symbol + 1, "%d", vreg_cnt++);
            break;
        }
        break;
    
    case T_B_NOT:
        switch (mode)
        {
        case 0: res->constant = ~opa.constant; break;
        case 1:
            fprintf(fp, "  %%%d = xor i32 %s, -1\n", vreg_cnt, opa.symbol);
            sprintf(res->symbol + 1, "%d", vreg_cnt++);
            break;
        }
        break;
        
    default:
        printf("pass\n");
        break;
    }
}

void examine_binary_exp(union exp_value* res, NonTerminal operators,
                    union exp_value opa, union exp_value opb,
                    unsigned char mode) {
    //all operators are signed calculations
    switch (operators)
    {
    case T_PLUS:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant + opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = add nsw i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = add nsw i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = add nsw i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;
    
    case T_MINUS:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant - opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = sub nsw i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = sub nsw i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = sub nsw i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;

    case T_STAR:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant * opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = mul nsw i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = mul nsw i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = mul nsw i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;
    
    case T_DIV:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant / opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = sdiv i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = sdiv i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = sdiv i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;
    
    case T_LT:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant < opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = icmp slt i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = icmp slt i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = icmp slt i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;

    case T_LE:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant <= opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = icmp sle i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = icmp sle i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = icmp sle i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;

    case T_GT:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant > opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = icmp sgt i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = icmp sgt i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = icmp sgt i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;
    
    case T_GE:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant >= opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = icmp sge i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = icmp sge i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = icmp sge i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;

    case T_EQ:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant == opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = icmp eq i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = icmp eq i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = icmp eq i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;
    
    case T_NEQ:
        switch (mode)
        {
            //case 1 should be optimzed
            case 0:
                res->constant = opa.constant != opb.constant;
                break;
            case 1:
                fprintf(fp, "  %%%d = icmp ne i32 %d, %s\n", vreg_cnt, opa.constant, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 2:
                fprintf(fp, "  %%%d = icmp ne i32 %s, %d\n", vreg_cnt, opa.symbol, opb.constant);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
            case 3:
                fprintf(fp, "  %%%d = icmp ne i32 %s, %s\n", vreg_cnt, opa.symbol, opb.symbol);
                sprintf(res->symbol + 1, "%d", vreg_cnt++);
                break;
        }
        break;
    
    default:
        printf("pass\n");
    }
}

void llvmgen_finalize() {
    fprintf(fp, "\nattributes #0 = { noinline nounwind optnone uwtable"
            "\"correctly-rounded-divide-sqrt-fp-math\"=\"false\""
            "\"disable-tail-calls\"=\"false\" \"frame-pointer\"=\"all\""
            "\"less-precise-fpmad\"=\"false\" \"min-legal-vector-width\"=\"0\""
            "\"no-infs-fp-math\"=\"false\" \"no-jump-tables\"=\"false\""
            "\"no-nans-fp-math\"=\"false\" \"no-signed-zeros-fp-math\"=\"false\""
            "\"no-trapping-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\""
            "\"target-cpu\"=\"x86-64\""
            "\"target-features\"=\"+cx8,+fxsr,+mmx,+sse,+sse2,+x87\""
            "\"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n");

    fprintf(fp, "attributes #1 = {"
            "\"correctly-rounded-divide-sqrt-fp-math\"=\"false\""
            "\"disable-tail-calls\"=\"false\" \"frame-pointer\"=\"all\""
            "\"less-precise-fpmad\"=\"false\" \"no-infs-fp-math\"=\"false\""
            "\"no-nans-fp-math\"=\"false\" \"no-signed-zeros-fp-math\"=\"false\""
            "\"no-trapping-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\""
            "\"target-cpu\"=\"x86-64\""
            "\"target-features\"=\"+cx8,+fxsr,+mmx,+sse,+sse2,+x87\""
            "\"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n");

    fclose(fp);
}
#include "ncc.h"

void generateForVariable(Node *node);

uint16_t labelIndex;
uint16_t breakLabelIndex;
uint16_t continueLabelIndex;

/**
 * アセンブリを標準出力する
 */
void generate(Node *node) {
    if (node->type == NUM) {
        printf("    push    %d\n", node->value);
        return;
    } else if (node->type == VAR) {
        generateForVariable(node);
        printf("    pop     rax\n");
        printf("    mov     rax, [rax]\n");
        printf("    push    rax\n");
        return;
    } else if (node->type == ASG) {
        generateForVariable(node->left);
        generate(node->right);
        printf("    pop     rdi\n");
        printf("    pop     rax\n");
        printf("    mov     [rax], rdi\n");
        printf("    push    rdi\n");
        return;
    } else if (node->type == FNK) {
        printf("%s:\n", node->name);
        printf("    push    rbp\n");
        printf("    mov     rbp, rsp\n");
        printf("    sub     rsp, %u\n", variableVector->length * 8);
        uint16_t blockIndex = 0;
        while(blockIndex < node->block->length) {
            Node *code = node->block->data[blockIndex];
            generate(code);
            blockIndex++;
        }
        printf("    mov     rsp, rbp\n");
        printf("    pop     rbp\n");
        printf("    ret\n");
        return;
    } else if (node->type == CAL) {
        printf("    call    %s\n", node->name);
        printf("    push    rax\n");
        return;
    } else if (node->type == PREINC) {
        Variable *variable = getVariable(node->name, node->length);
        printf("    mov     rax, rbp\n");
        printf("    sub     rax, %d\n", variable->offset * variable->byte);
        printf("    mov     rdi, [rax]\n");
        printf("    add     rdi, 1\n");
        printf("    mov     [rax], rdi\n");
        printf("    push    rdi\n");
        return;
    } else if (node->type == PREDEC) {
        Variable *variable = getVariable(node->name, node->length);
        printf("    mov     rax, rbp\n");
        printf("    sub     rax, %d\n", variable->offset * variable->byte);
        printf("    mov     rdi, [rax]\n");
        printf("    sub     rdi, 1\n");
        printf("    mov     [rax], rdi\n");
        printf("    push    rdi\n");
        return;
    } else if (node->type == POSINC) {
        Variable *variable = getVariable(node->name, node->length);
        printf("    mov     rax, rbp\n");
        printf("    sub     rax, %d\n", variable->offset * variable->byte);
        printf("    mov     rdi, [rax]\n");
        printf("    push    rdi\n");
        printf("    add     rdi, 1\n");
        printf("    mov     [rax], rdi\n");
        return;
    } else if (node->type == POSDEC) {
        Variable *variable = getVariable(node->name, node->length);
        printf("    mov     rax, rbp\n");
        printf("    sub     rax, %d\n", variable->offset * variable->byte);
        printf("    mov     rdi, [rax]\n");
        printf("    push    rdi\n");
        printf("    sub     rdi, 1\n");
        printf("    mov     [rax], rdi\n");
        return;
    } else if (node->type == BLK) {
        int blockIndex = 0;
        while(blockIndex < node->block->length) {
            Node *code = node->block->data[blockIndex];
            generate(code);
            blockIndex++;
        }
        return;
    } else if (node->type == LOR) {
        uint16_t trueLabelIndex = labelIndex++;
        uint16_t exitLabelIndex = labelIndex++;
        generate(node->left);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    jne     .Ltrue%u\n", trueLabelIndex);
        generate(node->right);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    jne     .Ltrue%u\n", trueLabelIndex);
        printf("    mov     rax, 0\n");
        printf("    jmp     .Lexit%u\n", exitLabelIndex);
        printf(".Ltrue%u:\n", trueLabelIndex);
        printf("    mov     rax, 1\n");
        printf(".Lexit%u:\n", exitLabelIndex);
        printf("    push    rax\n");
        return;
    } else if (node->type == LAND) {
        uint16_t falseLabelIndex = labelIndex;
        uint16_t exitLabelIndex = labelIndex++;
        generate(node->left);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lfalse%u\n", falseLabelIndex);
        generate(node->right);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lfalse%u\n", falseLabelIndex);
        printf("    mov     rax, 1\n");
        printf("    jmp     .Lexit%u\n", exitLabelIndex);
        printf(".Lfalse%u:\n", falseLabelIndex);
        printf("    mov     rax, 0\n");
        printf(".Lexit%u:\n", exitLabelIndex);
        printf("    push    rax\n");
        return;
    } else if (node->type == IF) {
        uint16_t ifLabelIndex = labelIndex++;
        generate(node->cond);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lend%u\n", ifLabelIndex);
        generate(node->then);
        printf(".Lend%u:\n", ifLabelIndex);
        return;
    } else if (node->type == FOR) {
        uint16_t beginLabelIndex = labelIndex;
        uint16_t endLabelIndex = labelIndex;
        breakLabelIndex = labelIndex;
        continueLabelIndex = labelIndex++;
        generate(node->init);
        printf(".Lbegin%u:\n", beginLabelIndex);
        generate(node->cond);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lend%u\n", endLabelIndex);
        generate(node->then);
        generate(node->after);
        printf("    jmp     .Lbegin%u\n", beginLabelIndex);
        printf(".Lend%u:\n", endLabelIndex);
        return;
    } else if (node->type == WHL) {
        uint16_t beginLabelIndex = labelIndex;
        uint16_t endLabelIndex = labelIndex;
        breakLabelIndex = labelIndex;
        continueLabelIndex = labelIndex++;
        printf(".Lbegin%u:\n", beginLabelIndex);
        generate(node->cond);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lend%u\n", endLabelIndex);
        generate(node->then);
        printf("    jmp     .Lbegin%u\n", beginLabelIndex);
        printf(".Lend%u:\n", endLabelIndex);
        return;
    } else if (node->type == RET) {
        generate(node->left);
        printf("    pop     rax\n");
        printf("    mov     rsp, rbp\n");
        printf("    pop     rbp\n");
        printf("    ret\n");
        return;
    } else if (node->type == BRK) {
        printf("    jmp     .Lend%u\n", breakLabelIndex);
        return;
    } else if (node->type == CNT) {
        printf("    jmp     .Lbegin%u\n", continueLabelIndex);
        return;
    }

    generate(node->left);
    generate(node->right);

    printf("    pop     rdi\n");
    printf("    pop     rax\n");
    switch (node->type) {
        case ADD:
            printf("    add     rax, rdi\n");
            break;
        case SUB:
            printf("    sub     rax, rdi\n");
            break;
        case MUL:
            printf("    mul     rdi\n");
            break;
        case DIV:
            printf("    mov     rdx, 0\n");
            printf("    div     rdi\n");
            break;
        case REM:
            printf("    mov     rdx, 0\n");
            printf("    div     rdi\n");
            printf("    push    rdx\n");
            return;
        case BAND:
            printf("    and     rax, rdi\n");
            break;
        case BOR:
            printf("    or      rax, rdi\n");
            break;
        case XOR:
            printf("    xor     rax, rdi\n");
            break;
        case EQ:
            printf("    cmp     rax, rdi\n");
            printf("    sete    al\n");
            printf("    movzb   rax, al\n");
            break;
        case NEQ:
            printf("    cmp     rax, rdi\n");
            printf("    setne   al\n");
            printf("    movzb   rax, al\n");
            break;
        case LT:
            printf("    cmp     rax, rdi\n");
            printf("    setl    al\n");
            printf("    movzb   rax, al\n");
            break;
        case LTE:
            printf("    cmp     rax, rdi\n");
            printf("    setle   al\n");
            printf("    movzb   rax, al\n");
            break;
    }

    printf("    push    rax\n");
}

void generateForVariable(Node *node) {
    if (node->type == VAR) {
        Variable *variable = getVariable(node->name, node->length);
        printf("    mov     rax, rbp\n");
        printf("    sub     rax, %d\n", variable->offset * variable->byte);
        printf("    push    rax\n");
        return;
    } else {
        fprintf(stderr, "=の左に来るノードではありません。\n");
        exit(1);
    }
}
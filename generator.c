#include "ncc.h"

void generateForVariable(Node *node);

uint16_t endLabelIndex;
uint16_t beginLabelIndex;
uint16_t breakLabelIndex;

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
    } else if (node->type == BLK) {
        int blockIndex = 0;
        while(blockIndex < node->block->length) {
            Node *code = node->block->data[blockIndex];
            generate(code);
            blockIndex++;
        }
        return;
    } else if (node->type == LOR) {
        int trueLabelIndex = endLabelIndex++;
        int exitLabelIndex = endLabelIndex++;
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
        int falseLabelIndex = endLabelIndex++;
        int exitLabelIndex = endLabelIndex++;
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
        generate(node->cond);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lend%u\n", endLabelIndex);
        generate(node->then);
        printf(".Lend%u:\n", endLabelIndex);
        endLabelIndex++;
        return;
    } else if (node->type == FOR) {
        breakLabelIndex = endLabelIndex + 1;
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
        beginLabelIndex++;
        endLabelIndex++;
        return;
    } else if (node->type == WHL) {
        breakLabelIndex = endLabelIndex + 1;
        printf(".Lbegin%u:\n", beginLabelIndex);
        generate(node->cond);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lend%u\n", endLabelIndex);
        generate(node->then);
        printf("    jmp     .Lbegin%u\n", beginLabelIndex);
        printf(".Lend%u:\n", endLabelIndex);
        beginLabelIndex++;
        endLabelIndex++;
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
        printf("    jmp     .Lbegin%u\n", beginLabelIndex);
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
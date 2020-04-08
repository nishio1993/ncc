#include "ncc.h"

void generateForVariable(Node *node);

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
    } else if (node->type == IF) {
        generate(node->condition);
        printf("    pop     rax\n");
        printf("    cmp     rax, 0\n");
        printf("    je      .Lend%d\n", endLabelIndex);
        generate(node->then);
        printf(".Lend%d:\n", endLabelIndex);
        endLabelIndex++;
        return;
    } else if (node->type == RET) {
        generate(node->left);
        printf("    pop     rax\n");
        printf("    mov     rsp, rbp\n");
        printf("    pop     rbp\n");
        printf("    ret\n");
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
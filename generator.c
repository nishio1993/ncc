#include "ncc.h"

/**
 * アセンブリを標準出力する
 */
void generate(Node *node) {
    if (node->type == NUM) {
        printf("    push    %d\n", node->value);
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
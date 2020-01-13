#include "ncc.h"

/**
 * アセンブリを標準出力する
 */
void codeGenerate(Node *node) {
    if (node->type == NUM) {
        printf("    push    %d\n", node->value);
        return;
    }

    codeGenerate(node->left);
    codeGenerate(node->right);

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
    }

    printf("    push    rax\n");
}
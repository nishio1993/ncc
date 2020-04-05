#include "ncc.h"

void outputError(int tokenIndex) {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    fprintf(stderr, "予期せぬ「%s」トークンがありました。\n", token->operator);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。\n");
        return 1;
    }

    tokenize(argv[1]);
    parse();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    push    rbp\n");
    printf("    mov     rbp, rsp\n");
    printf("    sub     rsp, %d\n", variableVector->length * 8);

    int i;
    for (i = 0; codeVector->length > i; i++) {
        Node *node = (Node *)codeVector->data[i];
        generate(node);
        printf("    pop     rax\n");
    }

    printf("    mov     rsp, rbp\n");
    printf("    pop     rbp\n");
    printf("    ret\n");
    return 0;
}
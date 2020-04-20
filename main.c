#include "ncc.h"

void outputError(int index) {
    Token *token = (Token*)tokenVector->data[index];
    fprintf(stderr, "%d行目%d文字目：予期せぬ「%s」トークンがありました。\n", token->row, token->col, token->ident);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。\n");
        return 1;
    }

    tokenize(argv[1]);
    int j;
    for (j = 0; j < tokenVector->length; j++) {
        Token *token = (Token*)tokenVector->data[j];
    }
    parse();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    int i;
    for (i = 0; codeVector->length > i; i++) {
        Node *node = (Node*)codeVector->data[i];
        generate(node);
        printf("    pop     rax\n");
    }

    printf("    mov     rsp, rbp\n");
    printf("    pop     rbp\n");
    printf("    ret\n");
    return 0;
}
#include "ncc.h"

void outputError(int tokenIndex) {
    fprintf(stderr, "予期せぬ「%s」トークンがありました。", tokenList[tokenIndex].error);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。\n");
        return 1;
    }

    tokenizer(argv[1]);
    position = 0;
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    generate(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
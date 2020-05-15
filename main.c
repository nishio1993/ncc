#include "ncc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。\n");
        return 1;
    }

    tokenize(argv[1]);
    parse();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    for (uint8_t i = 0; codeVector->length > i; i++) {
        Node *node = (Node*)codeVector->data[i];
        generate(node);
        printf("    pop     rax\n");
    }
    return 0;
}
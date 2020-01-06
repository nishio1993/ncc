#include "ncc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。\n");
        return 1;
    }

    char* input = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  mov rax, %ld\n", strtol(input, &input, 10));

    while (*input) {
        if (*input == '+') {
            input++;
            printf("  add rax, %ld\n", strtol(input, &input, 10));
        } else if (*input == '-') {
            input++;
            printf("  sub rax, %ld\n", strtol(input, &input, 10));
        } else {
            fprintf(stderr, "予期せぬトークンが見つかりました。\n");
            return 1;
        }
    }

    printf("  ret\n");
    return 0;
}
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

    bool tokenizeResult = tokenizer(argv[1]);
    if (!tokenizeResult) {
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    if (tokenList[0].type != NUMBER) {
        fprintf(stderr, "数式の最初は数値でなければなりません。\n");
        return 1;
    }
    printf("  mov rax, %d\n", tokenList[0].value);

    int index = 1;
    while (tokenList[index].type != END_OF_FILE) {
        if (tokenList[index].type != NUMBER) {
            int type = tokenList[index].type;
            index++;
            if (tokenList[index].type != NUMBER) {
                fprintf(stderr, "演算子の次は数値でなければなりません。\n");
                return 1;
            }

            if (type == '+') {
                printf("  add rax, %d\n", tokenList[index].value);
            } else if (type == '-') {
                printf("  sub rax, %d\n", tokenList[index].value);
            } else {
                outputError(index);
            }
            index++;
        } else {
            fprintf(stderr, "予期せぬトークンが見つかりました。\n");
            return 1;
        }
    }

    printf("  ret\n");
    return 0;
}
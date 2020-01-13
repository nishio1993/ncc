#include "ncc.h"

/**
 * 与えられた式をトークンに分解する
 */
void tokenizer(char *input) {
    int index = 0;
    while (*input) {
        if (isspace(*input)) {
            input++;
        } else if (*input == '+'
                || *input == '-'
                || *input == '*'
                || *input == '/'
                || *input == '('
                || *input == ')') {
            tokenList[index].type = *input;
            tokenList[index].error = input;
            input++;
            index++;
        } else if (isdigit(*input)) {
            tokenList[index].type = NUMBER;
            tokenList[index].value = strtol(input, &input, 10);
            tokenList[index].error = input;
            index++;
        } else {
            fprintf(stderr, "%sはトークナイズできません。", input);
            exit(1);
        }
    }

    tokenList[index].type = END_OF_FILE;
    tokenList[index].error = input;
    return;
}
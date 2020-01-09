#include "ncc.h"

bool tokenizer(char *input) {
    int index = 0;
    while (*input) {
        if (isspace(*input)) {
            input++;
        } else if (*input == '+' || *input == '-') {
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
            return false;
        }
    }

    tokenList[index].type = END_OF_FILE;
    tokenList[index].error = input;
    return true;
}
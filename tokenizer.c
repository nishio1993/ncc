#include "ncc.h"

/**
 * 与えられた式をトークンに分解する
 */
void tokenizer(char *input) {
    int index = 0;
    while (*input) {
        if (isspace(*input)) {
            input++;
        } else if (strncmp("==", input, 2) == 0
                || strncmp("!=", input, 2) == 0
                || strncmp("<=", input, 2) == 0
                || strncmp(">=", input, 2) == 0) {
            tokenList[index].type = OPERATOR;
            char *operator = (char*)malloc(sizeof(char) * 3);
            operator[0] = *input;
            operator[1] = *(input + 1);
            operator[2] = '\0';
            tokenList[index].operator = operator;
            tokenList[index].error = operator;
            input += 2;
            index++;
        } else if (strchr("+-*/()<>;=", *input) != NULL) {
            tokenList[index].type = OPERATOR;
            char *operator = (char*)malloc(sizeof(char) * 2);
            operator[0] = *input;
            operator[1] = '\0';
            tokenList[index].operator = operator;
            tokenList[index].error = input;
            input++;
            index++;
        } else if ('a' <= *input && *input <= 'z') {
            tokenList[index].type = VARIABLE;
            tokenList[index].name = *input;
            tokenList[index].operator = "";
            input++;
            index++;
        } else if (isdigit(*input)) {
            tokenList[index].type = NUMBER;
            tokenList[index].value = strtol(input, &input, 10);
            tokenList[index].operator = "";
            tokenList[index].error = input;
            index++;
        } else {
            fprintf(stderr, "%cはトークナイズできません。", *input);
            exit(1);
        }
    }

    tokenList[index].type = END_OF_FILE;
    tokenList[index].operator = "";
    tokenList[index].error = input;
    return;
}
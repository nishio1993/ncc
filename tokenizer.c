#include "ncc.h"

/**
 * 与えられた式をトークンに分解する
 */
void tokenizer(char *input) {
    int index = 0;
    while (*input) {
        if (isspace(*input)) {
            input++;
        } else if ((*input == '=' && *(input + 1) == '=')
                || (*input == '!' && *(input + 1) == '=')
                || (*input == '<' && *(input + 1) == '=')
                || (*input == '>' && *(input + 1) == '=')) {
            tokenList[index].type = OPERATOR;
            char *operator = (char*)malloc(sizeof(char) * 3);
            operator[0] = *input;
            operator[1] = *(input + 1);
            operator[2] = '\0';
            tokenList[index].operator = operator;
            tokenList[index].error = operator;
            input += 2;
            index++;
        } else if (*input == '+'
                || *input == '-'
                || *input == '*'
                || *input == '/'
                || *input == '('
                || *input == ')'
                || *input == '<'
                || *input == '>') {
            tokenList[index].type = OPERATOR;
            char *operator = (char*)malloc(sizeof(char) * 2);
            operator[0] = *input;
            operator[1] = '\0';
            tokenList[index].operator = operator;
            tokenList[index].error = input;
            input++;
            index++;
        } else if (isdigit(*input)) {
            tokenList[index].type = NUMBER;
            tokenList[index].value = strtol(input, &input, 10);
            tokenList[index].operator = "";
            tokenList[index].error = input;
            index++;
        } else {
            fprintf(stderr, "%sはトークナイズできません。", input);
            exit(1);
        }
    }

    tokenList[index].type = END_OF_FILE;
    tokenList[index].operator = "";
    tokenList[index].error = input;
    return;
}
#include "ncc.h"

void addToken(int type, char *operator, int value, char *name, int length);
void addVariable(char *name, int length, int byte, int offset);
char *duplicateString(char *start, int length);

/**
 * 与えられた式をトークンに分解する
 */
void tokenize(char *input) {
    tokenVector = newVector();
    variableVector = newVector();
    int offset = 8;

    while (*input) {
        if (isspace(*input)) {
            input++;
        } else if (strncmp("==", input, 2) == 0
                || strncmp("!=", input, 2) == 0
                || strncmp("<=", input, 2) == 0
                || strncmp(">=", input, 2) == 0) {
            char *operator = duplicateString(input, 2);
            addToken(OPERATOR, operator, 0, "", 0);
            input +=2;
        } else if (strchr("+-*/()<>;=", *input) != NULL) {
            char *operator = duplicateString(input, 1);
            addToken(OPERATOR, operator, 0, "", 0);
            input++;
        } else if (isalpha(*input) || *input == '_') {
            int length = 0;
            while(isalpha(input[length]) || isdigit(input[length]) || input[length] == '_') {
                length++;
            }
            char *name = duplicateString(input, length);
            addToken(VARIABLE, "", 0, name, length);
            if (getVariable(name, length) == NULL) {
                addVariable(name, length, 1, offset);
                offset += 8;
            }
            input += length;
        } else if (isdigit(*input)) {
            int value = strtol(input, &input, 10);
            addToken(NUMBER, "", value, "", 0);
        } else {
            fprintf(stderr, "%cはトークナイズできません。", *input);
            exit(1);
        }
    }
    return;
}

void addToken(int type, char *operator, int value, char *name, int length) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->operator = operator;
    token->value = value;
    token->name = name;
    token->length = length;
    vectorPush(tokenVector, token);
    return;
}

void addVariable(char *name, int length, int byte, int offset) {
    Variable *variable = malloc(sizeof(Variable));
    variable->name = name;
    variable->length = length;
    variable->byte = byte;
    variable->offset = offset;
    vectorPush(variableVector, variable);
    return;
}

char *duplicateString(char *start, int length) {
    char *string = malloc((sizeof(char) * length) + 1);
    int i;
    for (i = 0; i < length; i++) {
        string[i] = start[i];
    }
    string[i] = '\0';
    return string;
}
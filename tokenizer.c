#include "ncc.h"

void addToken(int type, char *identifier, int length, int value);
void addVariable(char *name, int length, int byte, int offset);
char *duplicateString(char *start, int length);
bool isIdentifier(char c);

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
        } else if (strncmp("if", input, 2) == 0
                && isIdentifier(input[2]) == false) {
            addToken(SYNTAX, "if", 2, 0);
            input += 2;
        } else if (strncmp("for", input, 3) == 0
                && isIdentifier(input[3]) == false) {
            addToken(SYNTAX, "for", 3, 0);
            input += 3;
        } else if (strncmp("return", input, 6) == 0
                && isIdentifier(input[6]) == false) {
            addToken(SYNTAX, "return", 6, 0);
            input += 6;
        } else if (strncmp("==", input, 2) == 0
                || strncmp("!=", input, 2) == 0
                || strncmp("<=", input, 2) == 0
                || strncmp(">=", input, 2) == 0) {
            char *identifier = duplicateString(input, 2);
            addToken(OPERATOR, identifier, 2, 0);
            input +=2;
        } else if (strchr("+-*/()<>;=", *input) != NULL) {
            char *identifier = duplicateString(input, 1);
            addToken(OPERATOR, identifier, 1, 0);
            input++;
        } else if (isalpha(*input) || *input == '_') {
            int length = 0;
            while(isIdentifier(input[length])) {
                length++;
            }
            char *name = duplicateString(input, length);
            addToken(VARIABLE, name, length, 0);
            if (getVariable(name, length) == NULL) {
                addVariable(name, length, 1, offset);
                offset += 8;
            }
            input += length;
        } else if (isdigit(*input)) {
            int value = strtol(input, &input, 10);
            addToken(NUMBER, "", 0, value);
        } else {
            fprintf(stderr, "%cはトークナイズできません。", *input);
            exit(1);
        }
    }
    return;
}

void addToken(int type, char *identifier, int length, int value) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->identifier = identifier;
    token->length = length;
    token->value = value;
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

bool isIdentifier(char c) {
    return isalpha(c) || isdigit(c) || c == '_';
}
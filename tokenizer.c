#include "ncc.h"

void addToken(int8_t type, char *ident, uint8_t length, int32_t value, uint16_t row, uint16_t col);
void addVariable(char *name, uint8_t length, uint8_t byte, uint8_t offset);
bool isIdentifier(char c);
uint8_t getDigits(uint8_t num);

/**
 * 与えられた式をトークンに分解する
 */
void tokenize(char *input) {
    tokenVector = newVector();
    variableVector = newVector();
    uint8_t offset = 8;
    uint16_t row = 1;
    uint16_t col = 1;

    while (*input) {
        if (isspace(*input) ||*input == '\t') {
            input++;
            col++;
        } else if (*input == '\n' || *input == '\r' ) {
            input++;
            row++;
            col = 1;
        } else if (strchr("{}", *input) != NULL) {
            char *ident = duplicateString(input, 1);
            addToken(SYNTAX, ident, 1, 0, row, col);
            input++;
            col++;
        } else if (strncmp("if", input, 2) == 0
                && isIdentifier(input[2]) == false) {
            addToken(SYNTAX, "if", 2, 0, row, col);
            input += 2;
            col += 2;
        } else if (strncmp("for", input, 3) == 0
                && isIdentifier(input[3]) == false) {
            addToken(SYNTAX, "for", 3, 0, row, col);
            input += 3;
            col += 3;
        } else if (strncmp("while", input, 5) == 0
                && isIdentifier(input[5]) == false) {
            addToken(SYNTAX, "while", 5, 0, row, col);
            input += 5;
            col += 5;
        } else if (strncmp("return", input, 6) == 0
                && isIdentifier(input[6]) == false) {
            addToken(SYNTAX, "return", 6, 0, row, col);
            input += 6;
            col += 6;
        } else if (strncmp("==", input, 2) == 0
                || strncmp("!=", input, 2) == 0
                || strncmp("<=", input, 2) == 0
                || strncmp(">=", input, 2) == 0) {
            char *ident = duplicateString(input, 2);
            addToken(OPERATOR, ident, 2, 0, row, col);
            input +=2;
            col += 2;
        } else if (strchr("+-*/()<>;=", *input) != NULL) {
            char *ident = duplicateString(input, 1);
            addToken(OPERATOR, ident, 1, 0, row, col);
            input++;
            col++;
        } else if (isalpha(*input) || *input == '_') {
            int length = 0;
            while(isIdentifier(input[length])) {
                length++;
            }
            char *name = duplicateString(input, length);
            if (input[length] == '(' && input[length + 1] == ')') {
                addToken(FUNCTION, name, length, 0, row, col);
                input += length;
                col += length;
            } else {
                addToken(VARIABLE, name, length, 0, row, col);
                if (getVariable(name, length) == NULL) {
                    addVariable(name, length, 1, offset);
                    offset += 8;
                }
                input += length;
                col += length;
            }
        } else if (isdigit(*input)) {
            int value = strtol(input, &input, 10);
            int length = getDigits(value);
            addToken(NUMBER, "", 0, value, row, col);
            col += length;
        } else {
            fprintf(stderr, "%d行目%d文字目：%cはトークナイズできません。", row, col, *input);
            exit(1);
        }
    }
    addToken(END_OF_FILE, "", 0, 0, row, col);
    return;
}

void addToken(int8_t type, char *ident, uint8_t length, int32_t value, uint16_t row, uint16_t col) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->ident = ident;
    token->length = length;
    token->value = value;
    token->row = row;
    token->col = col;
    vectorPush(tokenVector, token);
    return;
}

void addVariable(char *name, uint8_t length, uint8_t byte, uint8_t offset) {
    Variable *variable = malloc(sizeof(Variable));
    variable->name = name;
    variable->length = length;
    variable->byte = byte;
    variable->offset = offset;
    vectorPush(variableVector, variable);
    return;
}

bool isIdentifier(char c) {
    return isalpha(c) || isdigit(c) || c == '_';
}

uint8_t getDigits(uint8_t num) {
    uint8_t digits = 0;
    if (num == 0) {
        return 1;
    }

    while(num != 0) {
        num /= 10;
        digits++;
    }
    return digits;
}
#include "ncc.h"

void codeProceed(uint8_t count);
void codeNewLine(void);
void addToken(int8_t type, char *ident, uint8_t length, int32_t value, uint16_t row, uint16_t col);
void addVariable(char *name, uint8_t length, uint8_t byte, uint8_t offset);
bool isIdentifier(char c);
uint8_t getDigits(uint8_t num);

char *sourceCode;
uint8_t offset = 8;
uint16_t row = 1;
uint16_t col = 1;

typedef struct Symbol {
    char* ident;
    uint8_t length;
} Symbol;

Symbol syntaxList[] = {
    {"if", 2}, {"for", 3}, {"while", 5}, {"return", 6}, {"{", 1}, {"}", 1},
    {"break", 5}, {"continue", 8},
    {NULL, 0}
};
Symbol multiWordOperatorList[] = {
    {"==", 2}, {"!=", 2}, {"<=", 2}, {">=", 2}, {"&&", 2},
    {NULL, 0}
};

/**
 * 与えられた式をトークンに分解する
 */
void tokenize(char *input) {
    sourceCode = input;
    tokenVector = newVector();
    variableVector = newVector();
loop:
    while (*sourceCode != '\0') {
        //空白・改行
        if (*sourceCode == ' ' ||*sourceCode == '\t') {
            codeProceed(1);
            continue;
        } else if (*sourceCode == '\n') {
            codeNewLine();
            continue;
        }

        //コメント
        if (strncmp("//", sourceCode, 2) == 0) {
            codeProceed(2);
            while (*sourceCode != '\n') {
                codeProceed(1);
            }
            codeNewLine();
            continue;
        } else if (strncmp("/*", sourceCode, 2) == 0) {
            codeProceed(2);
            while (strncmp("*/", sourceCode, 2) != 0) {
                if (*sourceCode == '\n') {
                    codeNewLine();
                } else {
                    codeProceed(1);
                }
            }
            codeProceed(2);
            continue;
        }

        //構文
        for (int i = 0; multiWordOperatorList[i].ident; i++) {
            if (strncmp(multiWordOperatorList[i].ident, sourceCode, multiWordOperatorList[i].length) == 0
                && isIdentifier(sourceCode[multiWordOperatorList[i].length]) == false) {
                addToken(SYNTAX, multiWordOperatorList[i].ident, multiWordOperatorList[i].length, 0, row, col);
                codeProceed(multiWordOperatorList[i].length);
                goto loop;
            }
        }

        //複数文字演算子
        for (int i = 0; syntaxList[i].ident; i++) {
            if (strncmp(syntaxList[i].ident, sourceCode, syntaxList[i].length) == 0
                && isIdentifier(sourceCode[syntaxList[i].length]) == false) {
                addToken(SYNTAX, syntaxList[i].ident, syntaxList[i].length, 0, row, col);
                codeProceed(syntaxList[i].length);
                goto loop;
            }
        }
        
        //単一文字演算子
        if (strchr("+-*/%&|^()<>;=", *sourceCode) != NULL) {
            char *ident = duplicateString(sourceCode, 1);
            addToken(OPERATOR, ident, 1, 0, row, col);
            codeProceed(1);
            continue;
        }
        
        //変数・関数
        if (isalpha(*sourceCode) || *sourceCode == '_') {
            int length = 0;
            while (isIdentifier(sourceCode[length])) {
                length++;
            }
            char *name = duplicateString(sourceCode, length);
            if (sourceCode[length] == '(') {
                addToken(FUNCTION, name, length, 0, row, col);
            } else {
                addToken(VARIABLE, name, length, 0, row, col);
                if (getVariable(name, length) == NULL) {
                    addVariable(name, length, 1, offset);
                    offset += 8;
                }
            }
            codeProceed(length);
            continue;
        }
        
        //数値
        if (isdigit(*sourceCode)) {
            int value = strtol(sourceCode, &sourceCode, 10);
            int length = getDigits(value);
            addToken(NUMBER, "", 0, value, row, col);
            col += length;
            continue;
        }

        //上記のいずれでもない
        fprintf(stderr, "%d行目%d文字目：%cはトークナイズできません。", row, col, *sourceCode);
        exit(1);
    }
    addToken(END_OF_FILE, "", 0, 0, row, col);
    return;
}

void codeProceed(uint8_t count) {
    sourceCode += count;
    col += count;
    return;
}

void codeNewLine(void) {
    sourceCode++;
    row++;
    col = 1;
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

    while (num != 0) {
        num /= 10;
        digits++;
    }
    return digits;
}
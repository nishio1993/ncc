#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    RESERVED,
    NUMBER,
    END_OF_FILE
} TokenType;

typedef struct {
    int type;
    int value;
    char *error;
} Token;

Token tokenList[255];

bool tokenizer(char *input);
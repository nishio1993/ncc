#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum TokenType {
    RESERVED,   //予約語
    NUMBER,     //整数
    END_OF_FILE //終端
} TokenType;

typedef struct Token {
    int type;  //TokenType
    int value;      //TokenType=NUMBERの時の値
    char *error;    //エラー文表示用
} Token;

Token tokenList[255];
int position;

void tokenizer(char *input);

typedef enum NodeType{
    ADD,    //+
    SUB,    //-
    MUL,    //*
    DIV,    //÷
    NUM     //整数
} NodeType;

typedef struct Node {
    int type;           //NodeType
    struct Node *left;  //左辺
    struct Node *right; //右辺
    int value;          //NodeType=NUMの時の値
} Node;

Node *newSymbolNode(int type, Node *left, Node *right);
Node *newNumberNode(int value);

Node *expr();
Node *mul();
Node *primary();

void codeGenerate(Node *node);

void outputError(int position);
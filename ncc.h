#ifndef _NCC
#define _NCC
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum TokenType {
    OPERATOR,   //演算子
    SYNTAX,     //構文
    NUMBER,     //整数
    VARIABLE,   //変数
    END_OF_FILE //終端
} TokenType;

typedef enum NodeType{
    ADD,    //+
    SUB,    //-
    MUL,    //*
    DIV,    //÷
    EQ,     //==
    NEQ,    //!=
    LT,     //<
    LTE,    //<=
    VAR,    //変数
    ASG,    //=
    NUM,    //整数
    IF,     //IF
    FOR,    //FOR
    RET     //return
} NodeType;

typedef struct Token {
    int type;           //TokenType
    char *identifier;   //TokenType=NUMBER以外の識別子
    int length;         //identifierの長さ
    int value;          //TokenType=NUMBERの値
} Token;

typedef struct Variable {
    char *name; //変数名
    int length; //変数名の長さ
    int byte;   //バイトサイズ
    int offset; //RBPからのビット距離
} Variable;

typedef struct Node {
    int type;           //NodeType
    struct Node *left;  //左辺
    struct Node *right; //右辺
    int value;          //NodeType=NUMの時の値
    char *name;         //NodeType=VARの時の値
    int length;         //変数名の長さ
    struct Node *init;  //初期化式
    struct Node *cond;  //条件式
    struct Node *after; //変化式
    struct Node *then;  //実行処理
} Node;

typedef struct Vector {
    void **data;
    int capacity;
    int length;
} Vector;

typedef struct Map {
    Vector *key;
    Vector *value;
} Map;

Vector *tokenVector;
Vector *variableVector;
Vector *nodeVector;
Vector *codeVector;
int tokenIndex;

void tokenize(char *input);

void parse(void);

void generate(Node *node);

void outputError(int tokenIndex);

Vector *newVector(void);
void vectorPush(Vector *vector, void *element);
Map *newMap(void);
void mapSet(Map *map, char *key, void *val);
void *mapGet(Map *map, const char *key);
bool mapExists(Map *map, char *key);
Variable *getVariable(char *name, int length);
#endif
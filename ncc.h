#ifndef _NCC
#define _NCC
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef enum TokenType {
    OPERATOR,   //演算子
    SYNTAX,     //構文
    NUMBER,     //整数
    VARIABLE,   //変数
    FUNCTION,   //関数
    END_OF_FILE //終端
} TokenType;

typedef enum NodeType{
    ADD,    //+
    SUB,    //-
    MUL,    //*
    DIV,    //÷
    REM,    //%
    EQ,     //==
    NEQ,    //!=
    LT,     //<
    LTE,    //<=
    VAR,    //変数
    ASG,    //=
    NUM,    //整数
    IF,     //if
    FOR,    //for
    WHL,    //while
    BLK,    //{}
    BRK,    //break
    CNT,    //continue
    FNK,    //関数定義
    CAL,    //関数呼出
    RET     //return
} NodeType;

typedef struct Token {
    int8_t type;    //TokenType
    char *ident;    //TokenType=NUMBER以外の識別子
    uint8_t length; //identの長さ
    int32_t value;  //TokenType=NUMBERの値
    uint16_t row;   //行数
    uint8_t col;    //列数
} Token;

typedef struct Variable {
    char *name;     //変数名
    uint8_t length; //変数名の長さ
    uint8_t byte;   //バイトサイズ
    uint8_t offset; //RBPからのビット距離
} Variable;

typedef struct Vector {
    void **data;        //要素
    uint16_t capacity;  //最大要素数
    uint16_t length;    //現在要素数
} Vector;

typedef struct Node {
    int8_t type;            //NodeType
    struct Node *left;      //左辺
    struct Node *right;     //右辺
    int32_t value;          //NodeType=NUMの時の値
    char *name;             //NodeType=VARの時の値
    uint8_t length;         //変数名の長さ
    struct Node *init;      //初期化式
    struct Node *cond;      //条件式
    struct Node *after;     //変化式
    struct Node *then;      //実行処理
    Vector *block;   //ブロック
    Vector *arg;     //引数
} Node;

typedef struct Map {
    Vector *key;    //キー
    Vector *value;  //値
} Map;

Vector *tokenVector;
Vector *variableVector;
Vector *nodeVector;
Vector *codeVector;

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
Variable *getVariable(char *name, uint8_t length);
char *duplicateString(char *start, uint8_t length);
#endif
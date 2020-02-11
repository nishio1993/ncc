#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum TokenType {
    OPERATOR,   //演算子
    NUMBER,     //整数
    VARIABLE,   //変数
    ASSIGNMENT, //代入
    END_OF_FILE //終端
} TokenType;

typedef struct Token {
    int type;       //TokenType
    char* operator; //TokenType=OPERATORの時の値
    int value;      //TokenType=NUMBERの時の値
    char name;      //TokenType=VARIABLEの時の名前
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
    EQ,     //==
    NEQ,    //!=
    LT,     //<
    LTE,    //<=
    VAR,    //変数
    ASSIGN, //=
    NUM     //整数
} NodeType;

typedef struct Node {
    int type;           //NodeType
    struct Node *left;  //左辺
    struct Node *right; //右辺
    int value;          //NodeType=NUMの時の値
    char name;          //NodeType=ALLOC,STORE,LOADの時の値
} Node;

Node *codeList[255];

Node *newSymbolNode(int type, Node *left, Node *right);
Node *newNumberNode(int value);
Node *newVariableNode(char name);

void program();
Node *statement();
Node *expression();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void generate(Node *node);
void generateForVariable(Node *node);

void outputError(int position);
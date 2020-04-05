#include "ncc.h"

void program(void);
Node *statement(void);
Node *expression(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

Node *newSymbolNode(int type, Node *left, Node *right);
Node *newNumberNode(int value);
Node *newVariableNode(char *name, int length);

void parse() {
    program();
}

/**
 * program = statement*
 */
void program() {
    codeVector = newVector();
    while(tokenIndex < tokenVector->length) {
        vectorPush(codeVector, statement());
    }
}

/**
 * statement = expression ";" | "return" expression ";"
 */
Node *statement() {
    Node *node = expression();
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, ";") == 0) {
        tokenIndex++;
        return node;
    } else {
        outputError(tokenIndex);
    }
}

/**
 * expression = assign
 */
Node *expression() {
    Node *node = assign();
    return node;
}

/**
 * assign = equality ("=" assign)?
 */
Node *assign() {
    Node *node = equality();
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, "=") == 0) {
        tokenIndex++;
        return newSymbolNode(ASG, node, assign());
    } else {
        return node;
    }
}

/**
 * equality = relational ("==" relational | "!=" relational)*
 */
Node *equality() {
    Node *node = relational();
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, "==") == 0) {
        tokenIndex++;
        return newSymbolNode(EQ, node, relational());
    } else if (strcmp(token->operator, "!=") == 0) {
        tokenIndex++;
        return newSymbolNode(NEQ, node, relational());
    } else {
        return node;
    }
}

/**
 * relational = add ("<" add | "<=" add)*
 */
Node *relational() {
    Node *node = add();
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, "<") == 0) {
        tokenIndex++;
        return newSymbolNode(LT, node, add());
    } else if (strcmp(token->operator, ">") == 0) {
        tokenIndex++;
        return newSymbolNode(LT, add(), node);
    } else if (strcmp(token->operator, "<=") == 0) {
        tokenIndex++;
        return newSymbolNode(LTE, node, add());
    } else if (strcmp(token->operator, ">=") == 0) {
        tokenIndex++;
        return newSymbolNode(LTE, add(), node);
    } else {
        return node;
    }
}

/**
 * add = mul ("+" add | "-" add)*
 */
Node *add() {
    Node *node = mul();
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, "+") == 0) {
        tokenIndex++;
        return newSymbolNode(ADD, node, add());
    } else if (strcmp(token->operator, "-") == 0) {
        tokenIndex++;
        return newSymbolNode(SUB, node, add());
    } else {
        return node;
    }
}

/**
 * mul = unary ("*" mul | "/" mul)*
 */
Node *mul() {
    Node *node = unary();
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, "*") == 0) {
        tokenIndex++;
        return newSymbolNode(MUL, node, mul());
    } else if (strcmp(token->operator, "/") == 0) {
        tokenIndex++;
        return newSymbolNode(DIV, node, mul());
    } else {
        return node;
    }
}

/**
 * unary = ("+" unary | "-" unary)? primary
 */
Node *unary() {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->operator, "-") == 0) {
        Node *node = newNumberNode(0);
        return newSymbolNode(SUB, node, unary());
    } else if (strcmp(token->operator, "+") == 0) {
        tokenIndex++;
        return unary();
    } else {
        return primary();
    }
}

/**
 * primary = num | "(" expr ")"
 */
Node *primary() {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (token->type == NUMBER) {
        return newNumberNode(token->value);
    } else if (token->type == VARIABLE) {
        return newVariableNode(token->name, token->length);
    } else if (strcmp(token->operator, "(") == 0) {
        tokenIndex++;
        Node *node = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->operator, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        return node;
    }
}

/**
 * 左辺にleft、右辺にrightを持つノードを返却する。
 */
Node *newSymbolNode(int type, Node *left, Node *right) {
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    node->left = left;
    node->right = right;
    return node;
}

/**
 * 数値を持つノードを返却する。
 */
Node *newNumberNode(int value) {
    tokenIndex++;
    Node *node = calloc(1, sizeof(Node));
    node->type = NUM;
    node->value = value;
    return node;
}

/**
 * 変数を持つノードを返却する。
 */
Node *newVariableNode(char *name, int length) {
    tokenIndex++;
    Node *node = calloc(1, sizeof(Node));
    node->type = VAR;
    node->name = name;
    node->length = length;
    return node;
}
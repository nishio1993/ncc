#include "ncc.h"

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
    position++;
    Node *node = calloc(1, sizeof(Node));
    node->type = NUM;
    node->value = value;
    return node;
}

/**
 * expr = mul ("+" mul | "-" mul)*
 */
Node *expr() {
    Node *node = equality();
}

/**
 * equality = relational ("==" relational | "!=" relational)*
 */
Node *equality() {
    Node *node = relational();

    if (!strcmp(tokenList[position].operator, "==")) {
        position++;
        return newSymbolNode(EQ, node, relational());
    } else if (!strcmp(tokenList[position].operator, "!=")) {
        position++;
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

    if (!strcmp(tokenList[position].operator, "<")) {
        position++;
        return newSymbolNode(LT, node, add());
    } else if (!strcmp(tokenList[position].operator, ">")) {
        position++;
        return newSymbolNode(LT, add(), node);
    } else if (!strcmp(tokenList[position].operator, "<=")) {
        position++;
        return newSymbolNode(LTE, node, add());
    } else if (!strcmp(tokenList[position].operator, ">=")) {
        position++;
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

    if (!strcmp(tokenList[position].operator, "+")) {
        position++;
        return newSymbolNode(ADD, node, add());
    } else if (!strcmp(tokenList[position].operator, "-")) {
        position++;
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

    if (!strcmp(tokenList[position].operator, "*")) {
        position++;
        return newSymbolNode(MUL, node, mul());
    } else if (!strcmp(tokenList[position].operator, "/")) {
        position++;
        return newSymbolNode(DIV, node, mul());
    } else {
        return node;
    }
}

/**
 * unary = ("+" unary | "-" unary)? primary
 */
Node *unary() {
    if (!strcmp(tokenList[position].operator, "-")) {
        Node *node = newNumberNode(0);
        return newSymbolNode(SUB, node, unary());
    } else if (!strcmp(tokenList[position].operator, "+")) {
        position++;
        return unary();
    } else {
        return primary();
    }
}

/**
 * primary = num | "(" expr ")"
 */
Node *primary() {
    if (tokenList[position].type == NUMBER) {
        return newNumberNode(tokenList[position].value);
    } else if (!strcmp(tokenList[position].operator, "(")) {
        position++;
        Node *node = expr();
        if (strcmp(tokenList[position].operator, ")")) {
            outputError(position);
        }
        position++;
        return node;
    }
}
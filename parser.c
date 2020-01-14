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
    Node *node = mul();

    if (tokenList[position].type == '+' ) {
        position++;
        return newSymbolNode(ADD, node, expr());
    } else if (tokenList[position].type == '-') {
        position++;
        return newSymbolNode(SUB, node, expr());
    } else {
        return node;
    }
}

/**
 * mul = unary ("*" unary | "/" unary)*
 */
Node *mul() {
    Node *node = unary();

    if (tokenList[position].type == '*') {
        position++;
        return newSymbolNode(MUL, node, unary());
    } else if (tokenList[position].type == '/') {
        position++;
        return newSymbolNode(DIV, node, unary());
    } else {
        return node;
    }
}

/**
 * unary = ("+" | "-")? primary
 */
Node *unary() {
    if (tokenList[position].type == '-') {
        Node *node = newNumberNode(0);
        return newSymbolNode(SUB, node, primary());
    } else if (tokenList[position].type == '+') {
        position++;
        return primary();
    } else {
        return primary();
    }
}

/**
 * primary = num | "(" expr ")"
 */
Node *primary() {
    if (tokenList[position].type == '(') {
        position++;
        Node *node = expr();
        if (tokenList[position].type != ')') {
            outputError(position);
        }
        position++;
        return node;
    } else {
        return newNumberNode(tokenList[position].value);
    }
}
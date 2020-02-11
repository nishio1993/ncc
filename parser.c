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
 * 変数を持つノードを返却する。
 */
Node *newVariableNode(char name) {
    position++;
    Node *node = calloc(1, sizeof(Node));
    node->type = VAR;
    node->name = name;
    return node;
}

/**
 * program = statement*
 */
void program() {
    int line;
    for (line = 0; tokenList[position].type != END_OF_FILE; line++) {
        codeList[line] = statement();
    }
}

/**
 * statement = expression ";"
 */
Node *statement() {
    Node *node = expression();
    if (strcmp(tokenList[position].operator, ";") == 0) {
        position++;
        return node;
    } else {
        outputError(position);
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
    if (strcmp(tokenList[position].operator, "=") == 0) {
        position++;
        return newSymbolNode(ASSIGN, node, assign());
    } else {
        return node;
    }
}

/**
 * equality = relational ("==" relational | "!=" relational)*
 */
Node *equality() {
    Node *node = relational();

    if (strcmp(tokenList[position].operator, "==") == 0) {
        position++;
        return newSymbolNode(EQ, node, relational());
    } else if (strcmp(tokenList[position].operator, "!=") == 0) {
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

    if (strcmp(tokenList[position].operator, "<") == 0) {
        position++;
        return newSymbolNode(LT, node, add());
    } else if (strcmp(tokenList[position].operator, ">") == 0) {
        position++;
        return newSymbolNode(LT, add(), node);
    } else if (strcmp(tokenList[position].operator, "<=") == 0) {
        position++;
        return newSymbolNode(LTE, node, add());
    } else if (strcmp(tokenList[position].operator, ">=") == 0) {
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

    if (strcmp(tokenList[position].operator, "+") == 0) {
        position++;
        return newSymbolNode(ADD, node, add());
    } else if (strcmp(tokenList[position].operator, "-") == 0) {
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

    if (strcmp(tokenList[position].operator, "*") == 0) {
        position++;
        return newSymbolNode(MUL, node, mul());
    } else if (strcmp(tokenList[position].operator, "/") == 0) {
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
    if (strcmp(tokenList[position].operator, "-") == 0) {
        Node *node = newNumberNode(0);
        return newSymbolNode(SUB, node, unary());
    } else if (strcmp(tokenList[position].operator, "+") == 0) {
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
    } else if (tokenList[position].type == VARIABLE) {
        return newVariableNode(tokenList[position].name);
    } else if (strcmp(tokenList[position].operator, "(") == 0) {
        position++;
        Node *node = expression();
        if (strcmp(tokenList[position].operator, ")") != 0) {
            outputError(position);
        }
        position++;
        return node;
    }
}
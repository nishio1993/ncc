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
        Node *node = statement();
        vectorPush(codeVector, node);
    }
}

/**
 * statement = expression ";"
 *           | "{" stmt* "}"
 *           | "if" "(" expression ")" statement 
 *           | "for" "(" expression? ";" expression? ";" expression? ")" statement
 *           | "while" "(" expression ")" statement
 *           | "return" expression ";"
 */
Node *statement() {
    Node *node = calloc(1, sizeof(Node));
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->identifier, "{") == 0){
        tokenIndex++;
        Vector *blockVector = newVector();
        while(strcmp(token->identifier, "}") != 0) {
            Node *block = statement();
            vectorPush(blockVector, block);
            token = (Token*)tokenVector->data[tokenIndex];
        }
        tokenIndex++;
        node->type = BLK;
        node->block = blockVector;
        return node;
    } else if (strcmp(token->identifier, "if") == 0) {
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, "(") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *cond = calloc(1, sizeof(Node));
        cond = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *then = calloc(1, sizeof(Node));
        then = statement();
        node->type = IF;
        node->cond = cond;
        node->then = then;
        return node;
    } else if (strcmp(token->identifier, "for") == 0) {
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, "(") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *init = calloc(1, sizeof(Node));
        Node *cond = calloc(1, sizeof(Node));
        Node *after = calloc(1, sizeof(Node));
        Node *then = calloc(1, sizeof(Node));
        init = statement();
        cond = statement();
        after = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        then = statement();
        node->type = FOR;
        node->init = init;
        node->cond = cond;
        node->after = after;
        node->then = then;
        return node;
    } else if (strcmp(token->identifier, "while") == 0) {
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, "(") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *cond = calloc(1, sizeof(Node));
        cond = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *then = calloc(1, sizeof(Node));
        then = statement();
        node->type = WHL;
        node->cond = cond;
        node->then = then;
        return node;
    } else if (strcmp(token->identifier, "return") == 0) {
        tokenIndex++;
        node = newSymbolNode(RET, expression(), NULL);
    } else {
        node = expression();
    }

    token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->identifier, ";") == 0) {
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
    if (strcmp(token->identifier, "=") == 0) {
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
    if (strcmp(token->identifier, "==") == 0) {
        tokenIndex++;
        return newSymbolNode(EQ, node, relational());
    } else if (strcmp(token->identifier, "!=") == 0) {
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
    if (strcmp(token->identifier, "<") == 0) {
        tokenIndex++;
        return newSymbolNode(LT, node, add());
    } else if (strcmp(token->identifier, ">") == 0) {
        tokenIndex++;
        return newSymbolNode(LT, add(), node);
    } else if (strcmp(token->identifier, "<=") == 0) {
        tokenIndex++;
        return newSymbolNode(LTE, node, add());
    } else if (strcmp(token->identifier, ">=") == 0) {
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
    if (strcmp(token->identifier, "+") == 0) {
        tokenIndex++;
        return newSymbolNode(ADD, node, add());
    } else if (strcmp(token->identifier, "-") == 0) {
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
    if (strcmp(token->identifier, "*") == 0) {
        tokenIndex++;
        return newSymbolNode(MUL, node, mul());
    } else if (strcmp(token->identifier, "/") == 0) {
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
    if (strcmp(token->identifier, "-") == 0) {
        Node *node = newNumberNode(0);
        return newSymbolNode(SUB, node, unary());
    } else if (strcmp(token->identifier, "+") == 0) {
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
        return newVariableNode(token->identifier, token->length);
    } else if (strcmp(token->identifier, "(") == 0) {
        tokenIndex++;
        Node *node = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->identifier, ")") != 0) {
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
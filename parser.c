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

Node *newSymbolNode(int8_t type, Node *left, Node *right);
Node *newNumberNode(int32_t value);
Node *newVariableNode(char *name, uint8_t length);
Vector *newBlockVector(void);

uint16_t tokenIndex;

void parse() {
    program();
}

/**
 * program = statement*
 */
void program() {
    codeVector = newVector();
    Token *token = (Token*)tokenVector->data[0];
    while(token->type != END_OF_FILE) {
        Node *node = statement();
        vectorPush(codeVector, node);
        token = (Token *)tokenVector->data[tokenIndex];
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
    if (strcmp(token->ident, "{") == 0){
        tokenIndex++;
        Vector *blockVector = newBlockVector();
        node->type = BLK;
        node->block = blockVector;
        return node;
    } else if (strcmp(token->ident, "if") == 0) {
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, "(") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *cond = calloc(1, sizeof(Node));
        cond = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *then = calloc(1, sizeof(Node));
        then = statement();
        node->type = IF;
        node->cond = cond;
        node->then = then;
        return node;
    } else if (strcmp(token->ident, "for") == 0) {
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, "(") != 0) {
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
        if (strcmp(token->ident, ")") != 0) {
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
    } else if (strcmp(token->ident, "while") == 0) {
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, "(") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *cond = calloc(1, sizeof(Node));
        cond = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        Node *then = calloc(1, sizeof(Node));
        then = statement();
        node->type = WHL;
        node->cond = cond;
        node->then = then;
        return node;
    } else if (strcmp(token->ident, "return") == 0) {
        tokenIndex++;
        node = newSymbolNode(RET, expression(), NULL);
    } else {
        node = expression();
    }

    token = (Token*)tokenVector->data[tokenIndex];
    if (strcmp(token->ident, ";") == 0) {
        tokenIndex++;
        return node;
    } else if (token->type == END_OF_FILE
            || node->type == FNK) {
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
    if (strcmp(token->ident, "=") == 0) {
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
    if (strcmp(token->ident, "==") == 0) {
        tokenIndex++;
        return newSymbolNode(EQ, node, relational());
    } else if (strcmp(token->ident, "!=") == 0) {
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
    if (strcmp(token->ident, "<") == 0) {
        tokenIndex++;
        return newSymbolNode(LT, node, add());
    } else if (strcmp(token->ident, ">") == 0) {
        tokenIndex++;
        return newSymbolNode(LT, add(), node);
    } else if (strcmp(token->ident, "<=") == 0) {
        tokenIndex++;
        return newSymbolNode(LTE, node, add());
    } else if (strcmp(token->ident, ">=") == 0) {
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
    if (strcmp(token->ident, "+") == 0) {
        tokenIndex++;
        return newSymbolNode(ADD, node, add());
    } else if (strcmp(token->ident, "-") == 0) {
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
    if (strcmp(token->ident, "*") == 0) {
        tokenIndex++;
        return newSymbolNode(MUL, node, mul());
    } else if (strcmp(token->ident, "/") == 0) {
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
    if (strcmp(token->ident, "-") == 0) {
        Node *node = newNumberNode(0);
        return newSymbolNode(SUB, node, unary());
    } else if (strcmp(token->ident, "+") == 0) {
        tokenIndex++;
        return unary();
    } else {
        return primary();
    }
}

/**
 * primary = num
 *         | ident ("(" ")")?
 *         | "(" expr ")"
 */
Node *primary() {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (token->type == NUMBER) {
        return newNumberNode(token->value);
    } else if (token->type == VARIABLE) {
        return newVariableNode(token->ident, token->length);
    } else if (token->type == FUNCTION) {
        char *name = duplicateString(token->ident, token->length);
        int length = token->length;
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, "(") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        token = (Token*)tokenVector->data[tokenIndex];
        Node *node = calloc(1, sizeof(Node));
        node->name = name;
        node->length = length;
        if (strcmp(token->ident, "{") == 0) {
            tokenIndex++;
            Vector *block = newBlockVector();
            node->type = FNK;
            node->block = block;
        } else {
            node->type = CAL;
        }
        return node;
    } else if (strcmp(token->ident, "(") == 0) {
        tokenIndex++;
        Node *node = expression();
        token = (Token*)tokenVector->data[tokenIndex];
        if (strcmp(token->ident, ")") != 0) {
            outputError(tokenIndex);
        }
        tokenIndex++;
        return node;
    }
}

/**
 * 左辺にleft、右辺にrightを持つノードを返却する。
 */
Node *newSymbolNode(int8_t type, Node *left, Node *right) {
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    node->left = left;
    node->right = right;
    return node;
}

/**
 * 数値を持つノードを返却する。
 */
Node *newNumberNode(int32_t value) {
    tokenIndex++;
    Node *node = calloc(1, sizeof(Node));
    node->type = NUM;
    node->value = value;
    return node;
}

/**
 * 変数を持つノードを返却する。
 */
Node *newVariableNode(char *name, uint8_t length) {
    tokenIndex++;
    Node *node = calloc(1, sizeof(Node));
    node->type = VAR;
    node->name = name;
    node->length = length;
    return node;
}

/**
 * ブロック構文を返却する。
 */
Vector *newBlockVector() {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    Vector *blockVector = newVector();
    while(strcmp(token->ident, "}") != 0) {
        Node *block = statement();
        vectorPush(blockVector, block);
        token = (Token*)tokenVector->data[tokenIndex];
    }
    tokenIndex++;
    return blockVector;
}
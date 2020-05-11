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

bool isExpectedToken(char *ident);
bool isExpectedTokenType(int8_t type);
Node *newSymbolNode(int8_t type, Node *left, Node *right);
Node *newNumberNode(Token *token);
Node *newVariableNode(Token *token);
Node *newFunctionNode(Token *token);
Vector *newBlockVector(void);
void outputError(int tokenIndex);

uint16_t tokenIndex;

void parse(void) {
    program();
}

/**
 * program = statement*
 */
void program(void) {
    codeVector = newVector();
    while (isExpectedTokenType(END_OF_FILE) == false) {
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
Node *statement(void) {
    Node *node = calloc(1, sizeof(Node));
    if (isExpectedToken("{")){
        Vector *blockVector = newBlockVector();
        node->type = BLK;
        node->block = blockVector;
        return node;
    } else if (isExpectedToken("if")) {
        if (isExpectedToken("(") == false) {
            outputError(tokenIndex);
        }
        Node *cond = expression();
        if (isExpectedToken(")") == false) {
            outputError(tokenIndex);
        }
        Node *then = statement();
        node->type = IF;
        node->cond = cond;
        node->then = then;
        return node;
    } else if (isExpectedToken("for")) {
        if (isExpectedToken("(") == false) {
            outputError(tokenIndex);
        }
        Node *init = statement();
        Node *cond = statement();
        Node *after = expression();
        if (isExpectedToken(")") == false) {
            outputError(tokenIndex);
        }
        Node *then = statement();
        node->type = FOR;
        node->init = init;
        node->cond = cond;
        node->after = after;
        node->then = then;
        return node;
    } else if (isExpectedToken("while")) {
        if (isExpectedToken("(") == false) {
            outputError(tokenIndex);
        }
        Node *cond = expression();
        if (isExpectedToken(")") == false) {
            outputError(tokenIndex);
        }
        Node *then = statement();
        node->type = WHL;
        node->cond = cond;
        node->then = then;
        return node;
    } else if (isExpectedToken("return")) {
        node = newSymbolNode(RET, expression(), NULL);
    } else if (isExpectedToken("break")) {
        node->type = BRK;
    } else {
        node = expression();
    }

    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (isExpectedToken(";")) {
        return node;
    } else if (isExpectedTokenType(END_OF_FILE) || node->type == FNK) {
        return node;
    } else {
        outputError(tokenIndex);
    }
}

/**
 * expression = assign
 */
Node *expression(void) {
    Node *node = assign();
    return node;
}

/**
 * assign = equality ("=" assign)?
 */
Node *assign(void) {
    Node *node = equality();
    if (isExpectedToken("=")) {
        return newSymbolNode(ASG, node, assign());
    } else {
        return node;
    }
}

/**
 * equality = relational ("==" relational | "!=" relational)*
 */
Node *equality(void) {
    Node *node = relational();
    if (isExpectedToken("==")) {
        return newSymbolNode(EQ, node, relational());
    } else if (isExpectedToken("!=")) {
        return newSymbolNode(NEQ, node, relational());
    } else {
        return node;
    }
}

/**
 * relational = add ("<" add | "<=" add)*
 */
Node *relational(void) {
    Node *node = add();
    if (isExpectedToken("<")) {
        return newSymbolNode(LT, node, add());
    } else if (isExpectedToken(">")) {
        return newSymbolNode(LT, add(), node);
    } else if (isExpectedToken("<=")) {
        return newSymbolNode(LTE, node, add());
    } else if (isExpectedToken(">=")) {
        return newSymbolNode(LTE, add(), node);
    } else {
        return node;
    }
}

/**
 * add = mul ("+" add | "-" add)*
 */
Node *add(void) {
    Node *node = mul();
    if (isExpectedToken("+")) {
        return newSymbolNode(ADD, node, add());
    } else if (isExpectedToken("-")) {
        return newSymbolNode(SUB, node, add());
    } else {
        return node;
    }
}

/**
 * mul = unary ("*" mul | "/" mul)*
 */
Node *mul(void) {
    Node *node = unary();
    if (isExpectedToken("*")) {
        return newSymbolNode(MUL, node, mul());
    } else if (isExpectedToken("/")) {
        return newSymbolNode(DIV, node, mul());
    } else {
        return node;
    }
}

/**
 * unary = ("+" unary | "-" unary)? primary
 */
Node *unary(void) {
    if (isExpectedToken("-")) {
        Node *node = calloc(1, sizeof(Node));
        node->type = NUM;
        node->value = 0;
        return newSymbolNode(SUB, node, unary());
    } else if (isExpectedToken("+")) {
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
Node *primary(void) {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (isExpectedTokenType(NUMBER)) {
        return newNumberNode(token);
    } else if (isExpectedTokenType(VARIABLE)) {
        return newVariableNode(token);
    } else if (isExpectedTokenType(FUNCTION)) {
        return newFunctionNode(token);
    } else if (isExpectedToken("(")) {
        Node *node = expression();
        if (isExpectedToken(")") == false) {
            outputError(tokenIndex);
        }
        return node;
    }
}

/**
 * 次のトークンが予期したトークンか調べる
 */
bool isExpectedToken(char *ident) {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (strncmp(ident, token->ident, token->length) == 0 && ident[token->length] == '\0') {
        tokenIndex++;
        return true;
    } else {
        return false;
    }
}

/**
 * 次のトークンが予期したタイプか調べる
 */
bool isExpectedTokenType(int8_t type) {
    Token *token = (Token*)tokenVector->data[tokenIndex];
    return token->type == type;
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
Node *newNumberNode(Token *token) {
    Node *node = calloc(1, sizeof(Node));
    node->type = NUM;
    node->value = token->value;
    tokenIndex++;
    return node;
}

/**
 * 変数を持つノードを返却する。
 */
Node *newVariableNode(Token *token) {
    Node *node = calloc(1, sizeof(Node));
    node->type = VAR;
    node->name = token->ident;
    node->length = token->length;
    tokenIndex++;
    return node;
}

/**
 * 関数を持つノードを返却する。
 */
Node *newFunctionNode(Token *token) {
    char *name = duplicateString(token->ident, token->length);
    int length = token->length;
    tokenIndex++;
    if (isExpectedToken("(") == false) {
        outputError(tokenIndex);
    }
    if (isExpectedToken(")") == false) {
        outputError(tokenIndex);
    }
    Node *node = calloc(1, sizeof(Node));
    node->name = name;
    node->length = length;
    if (isExpectedToken("{")) {
        Vector *block = newBlockVector();
        node->type = FNK;
        node->block = block;
    } else {
        node->type = CAL;
    }
    return node;
}

/**
 * ブロック構文を返却する。
 */
Vector *newBlockVector(void) {
    Vector *blockVector = newVector();
    while (isExpectedToken("}") == false) {
        Node *block = statement();
        vectorPush(blockVector, block);
    }
    return blockVector;
}

/**
 * 予期せぬトークンがあることを表記して処理終了する。
 */
void outputError(int index) {
    Token *token = (Token*)tokenVector->data[index];
    fprintf(stderr, "%d行目%d文字目：予期せぬ「%s」トークンがありました。\n", token->row, token->col, token->ident);
    exit(1);
}
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
Node *newSymbolNode(int8_t type, Node *left, Node *right);
Node *newNumberNode(Token *token);
Node *newVariableNode(Token *token);
Node *newFunctionNode(Token *token);
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
    if (isExpectedToken("{") == true){
        Vector *blockVector = newBlockVector();
        node->type = BLK;
        node->block = blockVector;
        return node;
    } else if (isExpectedToken("if") == true) {
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
    } else if (isExpectedToken("for") == true) {
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
    } else if (isExpectedToken("while") == true) {
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
    } else if (isExpectedToken("return") == true) {
        node = newSymbolNode(RET, expression(), NULL);
    } else {
        node = expression();
    }

    Token *token = (Token*)tokenVector->data[tokenIndex];
    if (isExpectedToken(";") == true) {
        return node;
    } else if (token->type == END_OF_FILE || node->type == FNK) {
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
    if (isExpectedToken("=") == true) {
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
    if (isExpectedToken("==") == true) {
        return newSymbolNode(EQ, node, relational());
    } else if (isExpectedToken("!=") == true) {
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
    if (isExpectedToken("<") == true) {
        return newSymbolNode(LT, node, add());
    } else if (isExpectedToken(">") == true) {
        return newSymbolNode(LT, add(), node);
    } else if (isExpectedToken("<=") == true) {
        return newSymbolNode(LTE, node, add());
    } else if (isExpectedToken(">=") == true) {
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
    if (isExpectedToken("+") == true) {
        return newSymbolNode(ADD, node, add());
    } else if (isExpectedToken("-") == true) {
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
    if (isExpectedToken("*") == true) {
        return newSymbolNode(MUL, node, mul());
    } else if (isExpectedToken("/") == true) {
        return newSymbolNode(DIV, node, mul());
    } else {
        return node;
    }
}

/**
 * unary = ("+" unary | "-" unary)? primary
 */
Node *unary() {
    if (isExpectedToken("-") == true) {
        Node *node = calloc(1, sizeof(Node));
        node->type = NUM;
        node->value = 0;
        return newSymbolNode(SUB, node, unary());
    } else if (isExpectedToken("+") == true) {
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
        return newNumberNode(token);
    } else if (token->type == VARIABLE) {
        return newVariableNode(token);
    } else if (token->type == FUNCTION) {
        return newFunctionNode(token);
    } else if (isExpectedToken("(") == true) {
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
    if (isExpectedToken("{") == true) {
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
Vector *newBlockVector() {
    Vector *blockVector = newVector();
    while(isExpectedToken("}") == false) {
        Node *block = statement();
        vectorPush(blockVector, block);
    }
    return blockVector;
}
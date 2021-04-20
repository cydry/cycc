#include "cycc.h"

// Reports the error with the location in the input.
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports the error with a formatted message.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// If a next token is equel to expected one,
// then consume a token and returns True.
// Otherwise, returns False value.
bool consume(char* op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len  ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// If a next token is identifier,
// then consume a token and returns True.
// Otherwise, returns False value.
Token* consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token* cur = token;
  token = token->next;
  return cur;
}

// If a next token is equel to expected one,
// then consume a token and returns True.
// Otherwise, reports error.
void expect(char* op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len  ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "expected '%s', but '%s'", op, token->str);
  }
  token = token->next;
}

// If the next token type is a number,
// consume a token and returns the value.
// Otherwise, reports error.
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "Token is not a NUMBER");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// Make a new token, add it to current token as the next token.
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}


// Returns if alphernumeric or underscore.
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}


// Tokenize input charactors, returns it.
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {

    if (isspace(*p)) {
      p++;
      continue;
    }

    // Return statemment
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }

    // Control flow, if else
    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_RESERVED, cur, p, 4);
      p += 4;
      continue;
    }
    // Control flow, for, while
    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_RESERVED, cur, p, 3);
      p += 3;
      continue;
    }
    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_RESERVED, cur, p, 5);
      p += 5;
      continue;
    }

    // Identifier
    if (isalpha(*p)) {
      char* begin = p;
      while (isalnum(*p))
	p++;
      cur = new_token(TK_IDENT, cur, begin, p - begin);
      continue;
    }

    // equality
    if (strncmp(p, "==", 2) == 0 ||
	strncmp(p, "!=", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // relational
    if (strncmp(p, "<=", 2) == 0 ||
	strncmp(p, ">=", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (*p == '<' || *p == '>' ) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // assign
    if (*p == '=') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // statement
    if (*p == ';') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }
    if (*p == '{' || *p == '}') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // comma separated expressions
    if (*p == ',') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // Referencing.
    if (*p == '&') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (*p == '+' || *p == '-' ||
        *p == '*' || *p == '/' ||
	*p == '(' || *p == ')' ) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "cannot tokenize");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}


// Find local variable for offset infomation with the name.
// If not find the name, returns NULL.
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = func();
  code[i] = NULL;
}

Node *func() {
  Node *node = NULL;
  Token *tok = consume_ident();
  if (tok) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FUNC;
    if (consume("(")) {
      node->call = calloc(1, sizeof(tok->len)+1);
      strncpy(node->call, tok->str, tok->len);
      node->call[tok->len] = '\0';

      while(!consume(")")) {
	Vec* param = calloc(1, sizeof(Vec));
	param->node = new_node(ND_DECL, primary(), NULL);
	param->next = node->param;
	node->param = param;
	consume(",");
      }
    }
  } else {
    error("Not found identifier of function");
  }
  node->rhs = stmt();
  node->locals = locals ? locals->offset : 0; // total of offsets for locals.
  return node;
}

Node *stmt() {
  Node* node;

  // Compound statements.
  if (consume("{")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;

    while(!consume("}")) {
      Vec* elem = calloc(1, sizeof(Vec));
      elem->node = stmt();
      elem->next = node->block;
      node->block = elem;
    }
    return node;
  }

  // Control flow if-else.
  if (consume("if")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();

    if (consume("else")) {
      Node* tmp = calloc(1, sizeof(Node));
      tmp->kind = ND_ELSE;
    }
    return node;
  }
  // Control flow, for, while.
  if (consume("for")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;

    Node* tmp = calloc(1, sizeof(Node));
    expect("(");
    tmp->lhs = expr();
    expect(";");
    tmp->rhs = expr();
    expect(";");
    node->lhs = tmp;

    tmp = calloc(1, sizeof(Node));
    tmp->lhs = expr();
    expect(")");
    tmp->rhs = stmt();

    node->rhs = tmp;
    return node;
  }
  if (consume("while")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();
    return node;
  }

  if (consume("return")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }

  expect(";");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQL, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<="))
      node = new_node(ND_LEQ, node, add());
    else if (consume(">="))
      node = new_node(ND_LEQ, add(), node);
    else if (consume("<"))
      node = new_node(ND_LES, node, add());
    else if (consume(">"))
      node = new_node(ND_LES, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  if (consume("&"))
    return new_node(ND_ADDR, NULL, primary());
  if (consume("*"))
    return new_node(ND_DEREF, NULL, primary());
  return primary();
}

Node *primary() {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    if (consume("(")) {
      node->kind = ND_CALL;
      node->call = calloc(1, sizeof(tok->len)+1);
      strncpy(node->call, tok->str, tok->len);
      node->call[tok->len] = '\0';

      while(!consume(")")) {
	Vec* param = calloc(1, sizeof(Vec));
	param->node = expr();
	param->next = node->param;
	node->param = param;
	consume(",");
      }

    } else {
      node->kind = ND_LVAR;

      LVar *lvar = find_lvar(tok);
      if (lvar) {
	node->offset = lvar->offset;
      } else {
	lvar = calloc(1, sizeof(LVar));
	lvar->next = locals;
	lvar->name = tok->str;
	lvar->len = tok->len;
	lvar->offset = locals ? locals->offset + 8 : 0 + 8;
	node->offset = lvar->offset;
	locals = lvar;
      }
    }
    return node;
  }

  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}


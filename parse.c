#include "cycc.h"

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

// If a next token is literals,
// then consume a token and returns True.
// Otherwise, returns False value.
Token* consume_literal() {
  if (token->kind != TK_LITERAL)
    return NULL;
  Token* cur = token;
  token = token->next;
  return cur;
}

// If a next token is char ( 1 charactor ),
// then consume a token and returns True.
// Otherwise, returns False value.
Token* consume_char() {
  if (token->kind != TK_CHAR)
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

// If a next token is equel to expected one,　then returns True.
// Otherwise, returns False value.
// In contract to 'consume', this does not consume the token.
// This is mainly for parsing function, global variable on top-level space.
int inspect(char* op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len  ||
      memcmp(token->str, op, token->len))
    return false;
  return true;
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

    // Skip Comments.
    if (strncmp(p, "//", 2) == 0) {
      p += 2;
      while (*p != '\n')
	p++;
      continue;
    }
    if (strncmp(p, "/*", 2) == 0) {
      char* q = strstr(p+2, "*/");
      if (!q)
	error("Not end block comments.");
      p = q+2;
      continue;
    }

    // literals
    if (*p == '"') {
      char* begin = p;
      p++;
      while (*p != '"') {
	p++;
      }
      p++;
      cur = new_token(TK_LITERAL, cur, begin, p - begin);
      continue;
    }
    // 1 charactor
    if (*p == '\'') {
      char* begin = p;
      p = p + 2;
      if (*p != '\'')
	error("Invalid char literal");
      p++;
      cur = new_token(TK_CHAR, cur, begin, p - begin);
      continue;
    }

    // Size of the type.
    if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }

    // Type
    if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_RESERVED, cur, p, 3);
      p += 3;
      continue;
    }
    if (strncmp(p, "char", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_RESERVED, cur, p, 4);
      p += 4;
      continue;
    }
    if ((*p == '[') || (*p == ']')) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
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


    // postfix
    if (strncmp(p, "++", 2) == 0 ||
	strncmp(p, "--", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
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

// Find global variable for 'offset(data size)' infomation with the name.
// If not find the name, returns NULL.
LVar *find_gvar(Token *tok) {
  for (LVar *var = globals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

// Find literals for initializer by the name.
// If not find the name, returns NULL.
LVar *find_initials(char* name) {
  int len = strlen(name);
  for (LVar *var = initials; var; var = var->next)
    if (var->len == len && !memcmp(name, var->name, var->len))
      return var;
  return NULL;
}

Node* decl_param() {
  Node* node;
  if (inspect("int") || inspect("char")) {
    Type* ty = calloc(1, sizeof(Type));
    if (consume("int"))
      ty->kind = INT;
    else if (consume("char"))
      ty->kind = CHAR;
    else
      error("Unsupported type at decl_param");

    Type* ptr;
    while(consume("*")) {
      ptr = calloc(1, sizeof(Type));
      ptr->kind = PTR;
      ptr->ptr_to = ty;
      ty = ptr;
    }


    Token* tok = consume_ident();
    if (tok) {
      LVar* lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals ? locals->offset + 8 : 0 + 8;
      lvar->ty = ty;
      locals = lvar;

      node = calloc(1, sizeof(Node));
      node->kind = ND_LVAR;
      node->offset = lvar->offset;
      node->ty = ty;
    }
  } else {
    error_at(token->str, "No type for declaration.");
  }
  return node;
}

// Find a node of local variables in a partial tree.
// For using at evaluating a sizeof dereference.
Node* find_lvar_node(Node* node) {
  if (!node)
    return NULL;

  Node* ret;
  ret = find_lvar_node(node->lhs);
  if (ret)
    return ret;

  if (node->kind == ND_LVAR)
    return node;

  ret = find_lvar_node(node->rhs);
  if (ret)
    return ret;
}

// At Evaluating a sizeof, find size of the node.
int find_size(Node* node) {
  // Supports only Integer(32bit), about number.
  if (node->kind == ND_NUM)
    return 4;

  // Local Variable's size with the type
  if (node->kind == ND_LVAR) {
    if (node->ty->kind == CHAR)
      return 1;

    if (node->ty->kind == INT)
      return 4;

    // Array's size is the total bytes.
    if (node->ty->kind == PTR && node->ty->ptr_to->kind == ARRAY) {
      // array's structure is..  PTR -> ARRAY -> (Element's type)
      if (node->ty->ptr_to->ptr_to->kind == CHAR)
	return 1 * node->ty->ptr_to->array_size;
      if (node->ty->ptr_to->ptr_to->kind == INT)
	return 4 * node->ty->ptr_to->array_size;
      if (node->ty->ptr_to->ptr_to->kind == PTR)
	return 8 * node->ty->ptr_to->array_size;
    }

    if (node->ty->kind == PTR)
      return 8;
  }

  // Evaluates size of dereferecing a variable.
  if(node->kind == ND_DEREF) {
    node = find_lvar_node(node);
    if (node->ty->ptr_to->kind == CHAR)
      return 1;
    if (node->ty->ptr_to->kind == INT)
      return 4;
    if (node->ty->ptr_to->kind == PTR)
      return 8;

    if (node->ty->ptr_to->kind == ARRAY) {
      if (node->ty->ptr_to->ptr_to->kind == CHAR)
	return 1;
      if (node->ty->ptr_to->ptr_to->kind == INT)
	return 4;
      if (node->ty->ptr_to->ptr_to->kind == PTR)
	return 8;
    }
    error_at(token->str, "Invalid evaluation at `sizeof`: dereference");
  }


  if (node->kind == ND_ADD || node->kind == ND_SUB) {
    // Calcuraing addtion and substraction of Pointer to array,
    // the size is pointer size.
    if (node->lhs->ty &&
	node->lhs->ty->kind == PTR &&
	node->lhs->ty->ptr_to->kind == ARRAY) {
	return 8;
    }
    return find_size(node->lhs);
  }

  // Evaluates expressions by recursive call with the AST.
  // Or addressing is pointer size. Otherwise causes an error.
  switch (node->kind) {
  case ND_ASSIGN:
  case ND_LEQ:
  case ND_EQL:
  case ND_LES:
  case ND_MUL:
  case ND_DIV:
    return find_size(node->lhs);
  case ND_SIZE:
    return find_size(node->rhs);
  case ND_ADDR:
    return 8;
  case ND_CALL:
    error_at(token->str, "Invalid evaluation at `sizeof`: function call");
  default:
    break;
  }
  error_at(token->str, "Unreachable line at `sizeof`, calling with unsupported kind of a node");
}

// Evaluate the size of types, expressions.
Node* size_of() {
  int size = 0;
  consume("(");
  if (consume("int")) {
    size = 4;
    if (consume("*")) {
      while(consume("*"))
	;
      size = 8;
    }
    consume(")");
    return new_node_num(size);
  }

  size = find_size(expr());
  consume(")");
  return new_node_num(size);
}


void program() {
  Node* node;
  Token *tok;
  Type* ty;
  int i = 0;
  while (!at_eof()) {
    // Return type.
    // Only supports int and the pointer.
    if (inspect("int") || inspect("char")) {
      ty = calloc(1, sizeof(Type));
      if (consume("int"))
	ty->kind = INT;
      else if (consume("char"))
	ty->kind = CHAR;
      else
	error("Unsupported type at top-level definition");

      Type* ptr;
      while(consume("*")) {
	ptr = calloc(1, sizeof(Type));
	ptr->kind = PTR;
	ptr->ptr_to = ty;
	ty = ptr;
      }
    } else {
      error_at(token->str, "Not found a type on top-level space.");
    }

    tok = consume_ident();
    if (inspect("(")) {
      node = func(tok);

    } else if (inspect(";") || inspect("[") || inspect("=")) {
      // Definition of Global variable

      node = calloc(1, sizeof(Node));
      node->kind = ND_GDECL;
      node->call = calloc(1, sizeof(tok->len)+1);
      strncpy(node->call, tok->str, tok->len);
      node->call[tok->len] = '\0';

      LVar* gvar = calloc(1, sizeof(LVar));
      gvar->next = globals;
      gvar->name = tok->str;
      gvar->len = tok->len;

      if (ty->kind == CHAR)
	gvar->offset = 1;
      else if (ty->kind == INT)
	gvar->offset = 4;
      else if (ty->kind == PTR)
	gvar->offset = 8;
      else
	error("Unsupported type at definition of global variable");
      gvar->ty = ty;
      globals = gvar;

      if (consume("[")) {
	if (inspect("]")) { // syntax for only initializer
	  expect("]");
	  while (consume("["))
	    expect("]");

	  ty = calloc(1, sizeof(Type));
	  ty->kind = ARRAY;
	  ty->array_size = 0; //  syntax for only initializer
	  ty->ptr_to = gvar->ty;
	  gvar->ty = ty;

	} else {
	  int elem = expect_number();
	  expect("]");

	  while (consume("[")) {
	    elem *= expect_number();
	    expect("]");
	  }

	  ty = calloc(1, sizeof(Type));
	  ty->kind = ARRAY;
	  ty->array_size = elem;
	  gvar->offset *= elem;
	  ty->ptr_to = gvar->ty;
	  gvar->ty = ty;
	}
      }
      node->ty = gvar->ty;
      node->offset = gvar->offset;

      // Initialization
      if (consume("=")) {
	Token* initok = consume_literal();
	if (initok) {
	  Type* ty = calloc(1, sizeof(Type));
	  ty->kind = PTR;
	  ty->ptr_to = calloc(1, sizeof(Type));
	  ty->ptr_to->kind = ARRAY;
	  ty->ptr_to->array_size = initok->len;
	  ty->ptr_to->ptr_to = calloc(1, sizeof(Type));
	  ty->ptr_to->ptr_to->kind = CHAR;

	  Node* ininode = calloc(1, sizeof(Node));
	  ininode->kind = ND_LITER;

	  ininode->call = calloc(1, 64);
	  int uniq = unique_num();
	  int len = sprintf(ininode->call, "IC%d", uniq); // initializer's `name'
	  ininode->call[len] = '\0';

	  LVar* lvar = calloc(1, sizeof(LVar));
	  lvar->next = initials;
	  lvar->name = ininode->call; // name
	  lvar->len  = len;        // length of name

	  lvar->lite = calloc(1, sizeof(initok->len)+1);
	  strncpy(lvar->lite, initok->str, initok->len); // literal string
	  lvar->lite[initok->len] = '\0';
	  lvar->offset = initok->len;                 // length of literal string
	  lvar->ty = ty;
	  initials = lvar;

	  node->rhs = ininode;
	} else {
	  if (consume("{")) {
	    Node* ininode = calloc(1, sizeof(Node));
	    ininode->kind = ND_PARAM;                // in block's parameter, not function's.
	    while(!consume("}")) {
	      Vec* elem = calloc(1, sizeof(Vec));
	      elem->node = primary();
	      elem->next = ininode->param;
	      ininode->param = elem;
	      consume(",");
	    }
	    node->rhs = ininode;

	  } else {
	    Node* ininode = add();
	    if (ininode->kind == ND_NUM) {
	      node->rhs = ininode;
	    } else if (ininode->kind == ND_ADDR) {
	      node->rhs = ininode;
	    } else if (ininode->kind == ND_ADD) {// addition of address and constants, on global variables
	      node->rhs = ininode;
	    } else {
	      error("Unsupported initializtion at a global variable");
	    }
	  }
	}
      }

      expect(";");
    }

    code[i++] = node;
  }
  code[i] = NULL;
}

Node *func(Token* tok) {
  Node *node = NULL;

  // The tok is identifier.
  if (tok) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FUNC;
    if (consume("(")) {
      node->call = calloc(1, sizeof(tok->len)+1);
      strncpy(node->call, tok->str, tok->len);
      node->call[tok->len] = '\0';

      while(!consume(")")) {
	Vec* param = calloc(1, sizeof(Vec));
	param->node = new_node(ND_PARAM, decl_param(), NULL);
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

  // Reset dictionary of local variable, for total of offset to be correct.
  // About memory leaks, let the kernel do the memory freeing.
  locals = NULL;
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

  // Declaration.
  if (inspect("int") || inspect("char")) {
    Type* ty = calloc(1, sizeof(Type));
    if (consume("int"))
      ty->kind = INT;
    else if (consume("char"))
      ty->kind = CHAR;
    else
      error("Unsupported type at definition for local variables");

    Type* ptr;
    while(consume("*")) {
      ptr = calloc(1, sizeof(Type));
      ptr->kind = PTR;
      ptr->ptr_to = ty;
      ty = ptr;
    }

    node = calloc(1, sizeof(Node));
    node->kind = ND_DECL;
    Token* tok = consume_ident();
    LVar* lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->offset = locals ? locals->offset + 8 : 0 + 8;
    lvar->ty = ty;
    locals = lvar;

    if (consume("[")) {
      int elem;
      if (consume("]")) {
	while (consume("[")) {
	  expect("]");
	}
	elem = 0;
      } else {
	elem = expect_number();
	expect("]");
	while (consume("[")) {
	  elem *= expect_number();
	  expect("]");
	}
      }

      ty = calloc(1, sizeof(Type));
      ty->kind = ARRAY;
      ty->array_size = elem;
      lvar->offset = (lvar->offset - 8) + (elem * 8);
      ty->ptr_to = lvar->ty;
      lvar->ty = ty;
    }

    // Initialization, local variables
    if (consume("=")) {
      Node* ininode = calloc(1, sizeof(Node));
      LVar *ilvar = find_lvar(tok);
      if (ilvar) {
	ininode->kind = ND_LVAR;
	ininode->offset = ilvar->offset;
	ininode->ty = ilvar->ty;
      }

      if (consume("{")) {
	Node* paranode = calloc(1, sizeof(Node));
	paranode->kind = ND_PARAM;                // in block's parameter, not function's.
	while(!consume("}")) {
	  Vec* belem = calloc(1, sizeof(Vec));
	  belem->node = primary();
	  belem->next = paranode->param;
	  paranode->param = belem;
	  consume(",");
	}

	// set length of elements in blocks to local variables.
	if (ilvar->ty->array_size == 0) {
	  int len = vec_len(paranode->param);
	  ilvar->offset = ilvar->offset + (len * 8); // without number of elements. `x[]'
	  ininode->offset = ilvar->offset;
	}

	node->rhs = new_node(ND_ASSIGN, ininode, paranode);

      } else {
	node->rhs = new_node(ND_ASSIGN, ininode, assign());
      }
    }
    expect(";");
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
  Type* ty;
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      ty = node->ty;
      node = new_node(ND_ADD, node, mul());
      node->ty = ty;
      if (node->rhs->ty && node->rhs->ty->kind == PTR)
	node->ty = node->rhs->ty;
    } else if (consume("-")) {
      ty = node->ty;
      node = new_node(ND_SUB, node, mul());
      node->ty = ty;
    } else
      return node;
  }
}

Node *mul() {
  Type* ty;
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      ty = node->ty;
      node = new_node(ND_MUL, node, unary());
      node->ty = ty;
    } else if (consume("/")) {
      ty = node->ty;
      node = new_node(ND_DIV, node, unary());
      node->ty = ty;
    } else if (consume("[")) {
      // Change `index of array` to dereferencing of a pointer calculation.
      //
      // a[i] -> *(a + i)
      //
      ty = node->ty;
      node = new_node(ND_ADD, node, mul());
      node->ty = ty;
      if (node->rhs->ty && node->rhs->ty->kind == PTR)
	node->ty = node->rhs->ty;

      ty = node->ty;
      node = new_node(ND_DEREF, NULL, node);
      node->ty = ty->ptr_to;

      expect("]");
    } else
      return node;
  }
}

Node *unary() {
  Type* ty;
  Node* node;

  if (consume("sizeof"))
    return new_node(ND_SIZE, NULL, size_of());
  if (consume("+")) {
      node = postfix();
      if (node->ty && node->ty->kind == ARRAY) {
	Type* ty = calloc(1, sizeof(Type));
	ty->kind = PTR;
	ty->ptr_to = node->ty;
	node->ty = ty;
      }
      return node;
  }
  if (consume("-")) {
    node = postfix();
    if (node->ty && node->ty->kind == ARRAY) {
      Type* ty = calloc(1, sizeof(Type));
      ty->kind = PTR;
      ty->ptr_to = node->ty;
      node->ty = ty;
    }
    ty = node->ty;
    node = new_node(ND_SUB, new_node_num(0), node);
    node->ty = ty;
    return node;
  }
  if (consume("&")) {
    node = postfix();
    ty = node->ty;
    node = new_node(ND_ADDR, NULL, node);
    node->ty = ty;
    return node;
  }
  if (consume("*")) {
    node = unary();
    if (node->ty && node->ty->kind == ARRAY) {
      Type* ty = calloc(1, sizeof(Type));
      ty->kind = PTR;
      ty->ptr_to = node->ty;
      node->ty = ty;
    }
    ty = node->ty;
    node = new_node(ND_DEREF, NULL, node);
    node->ty = ty->ptr_to;
    return node;
  }

  node = postfix();
  if (node->ty && node->ty->kind == ARRAY) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = PTR;
    ty->ptr_to = node->ty;
    node->ty = ty;
  }
  return node;
}

Node *postfix() {
  Node* node = primary();
  if (consume("++"))
    return new_node(ND_POST, node, new_node(ND_ASSIGN, node, new_node(ND_ADD, node, new_node_num(1))));
  if (consume("--"))
    return new_node(ND_POST, node, new_node(ND_ASSIGN, node, new_node(ND_SUB, node, new_node_num(1))));
  return node;
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

      LVar *lvar = find_lvar(tok);
      if (lvar) {
	node->kind = ND_LVAR;
	node->offset = lvar->offset;
	node->ty = lvar->ty;
      } else {
	LVar *gvar = find_gvar(tok);
	if (gvar) {
	  node->kind = ND_GVAR;

	  node->call = calloc(1, sizeof(gvar->len)+1);
	  strncpy(node->call, gvar->name, gvar->len);
	  node->call[gvar->len] = '\0';

	  node->ty = gvar->ty;
	} else {
	  error_at(tok->str, "No declaration.");
	}
      }
    }
    return node;
  }

  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  tok = consume_literal();
  if (tok) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = PTR;
    ty->ptr_to = calloc(1, sizeof(Type));
    ty->ptr_to->kind = ARRAY;
    ty->ptr_to->array_size = tok->len;
    ty->ptr_to->ptr_to = calloc(1, sizeof(Type));
    ty->ptr_to->ptr_to->kind = CHAR;

    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LITER;

    node->call = calloc(1, 64);
    int uniq = unique_num();
    int len = sprintf(node->call, ".LC%d", uniq);
    node->call[len] = '\0';

    LVar* lvar = calloc(1, sizeof(LVar));
    lvar->next = literals;
    lvar->name = node->call; // name
    lvar->len  = len;        // length of name

    lvar->lite = calloc(1, sizeof(tok->len)+1);
    strncpy(lvar->lite, tok->str, tok->len); // literal string
    lvar->lite[tok->len] = '\0';
    lvar->offset = tok->len;                 // length of literal string
    lvar->ty = ty;
    literals = lvar;

    return node;
  }

  tok = consume_char();
  if (tok) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = CHAR;

    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_CHAR;
    node->call = calloc(1, sizeof(tok->len)+1);
    strncpy(node->call, tok->str, tok->len); // literal string
    node->call[tok->len] = '\0';

    return node;
  }

  return new_node_num(expect_number());
}


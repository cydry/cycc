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

// Find struct for size of user defined type, by the name.
// If not find the name, returns NULL.
//
// args:
//      tag: tag's name
//      len: tag's length
//
Tag *find_struct(char* name, int len) {
  for (Tag *tag = structs; tag; tag = tag->next)
    if (tag->len == len && !memcmp(name, tag->name, tag->len))
      return tag;
  return NULL;
}

// Find enum to replace token to iota by the token.
// If not find the token, returns NULL.
Tag *find_enum(Token *tok) {
  for (Tag *tag = enums; tag; tag = tag->next)
    if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len))
      return tag;
  return NULL;
}

// Find type defined by typedef.
// If not find the token, returns NULL.
Type *find_typedef(Token *tok) {
  for (Tag *tag = typedefs; tag; tag = tag->next)
    if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len))
      return tag->ty;
  return NULL;
}

// Find function name for calling.
// If not find the token, returns NULL.
Type *find_func(Token *tok) {
  for (Tag *tag = funcs; tag; tag = tag->next)
    if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len))
      return tag->ty;
  return NULL;
}

Node* decl_param() {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;

  Type* ty = consume_type();
  if (ty) {

    Token* tok = consume_ident();
    if (tok) {
      LVar* lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals ? locals->offset + ceil_bound8(type_size(ty)) : 0 + ceil_bound8(type_size(ty));
      lvar->ty = ty;
      locals = lvar;

      node->offset = lvar->offset;
    } else {
      // No name for declaration, valid for prototype.
      // Also, builtin macro: valist.
      if (ty && ty->kind == VA) {
	LVar* lvar = calloc(1, sizeof(LVar));
	lvar->next = locals;
	lvar->name = "__builtin_va_list";
	lvar->len = 17;
	lvar->offset = locals ? locals->offset + ceil_bound8(48) : 0 + ceil_bound8(48);
	lvar->ty = ty;
	locals = lvar;

	node->offset = lvar->offset;
      }
    }
    node->ty = ty;

  } else {
    error_at(token->str, "No type for declaration.");
  }
  return node;
}

Node* decl_lvar(Type* ty, Token* tok) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_DECL;

  LVar* lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->offset = locals ? locals->offset + ceil_bound8(type_size(ty)) : 0 + ceil_bound8(type_size(ty));
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
    ty->ptr_to = lvar->ty;
    lvar->offset = (lvar->offset - ceil_bound8(type_size(ty))) + ceil_bound8((elem * type_size(ty)));
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


// Parsing struct declaration.
void decl_struct(Type* ty) {
  if (consume("{")) {
    Tag* st = calloc(1, sizeof(Tag));
    st->ty = ty;
    st->name = calloc(1, (ty->tag_len)+1);
    strncpy(st->name, ty->tag, ty->tag_len);
    st->name[ty->tag_len] = '\0';
    st->len = ty->tag_len;
    st->next = structs;
    structs = st;

    while (!consume("}")) {

      // Member's type
      Type* memb_ty = consume_type();
      Tag* memb_tag = calloc(1, sizeof(Tag));
      memb_tag->ty = memb_ty;

      // Member's tag name
      Token* memb_tok = consume_ident();
      memb_tag->name = calloc(1, (memb_tok->len)+1);
      strncpy(memb_tag->name, memb_tok->str, memb_tok->len);
      memb_tag->name[memb_tok->len] = '\0';
      memb_tag->len = memb_tok->len;

      // Updates struct's dictionary.
      memb_tag->next = st->memb;
      st->memb = memb_tag;

      consume(";");
    }
  }
}

// Parsing enum declaration.
void decl_enum(Type* ty) {
  Tag* enu = calloc(1, sizeof(Tag));
  enu->ty = ty;
  enu->name = calloc(1, (ty->tag_len)+1);
  strncpy(enu->name, ty->tag, ty->tag_len);
  enu->name[ty->tag_len] = '\0';

  if (consume("{")) {
    int i = 0;
    while (!consume("}")) {
      Tag*   idx_tag = calloc(1, sizeof(Tag));
      Token* iota_tok = consume_ident();
      idx_tag->name = calloc(1, (iota_tok->len)+1);
      strncpy(idx_tag->name, iota_tok->str, iota_tok->len);
      idx_tag->name[iota_tok->len] = '\0';
      idx_tag->memb = enu;
      idx_tag->len = iota_tok->len;

      // have an index.
      idx_tag->iota = i;
      i++;

      idx_tag->next = enums;
      enums = idx_tag;

      consume(",");
    }
  }
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
  if (node->kind == ND_LVAR || node->kind == ND_GVAR) {
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
  Type* ty = consume_type();
  if (ty) {
    size = type_size(ty);
    consume(")");
    return new_node_num(size);
  }

  size = find_size(expr());
  consume(")");
  return new_node_num(size);
}

// Parse type declarator.
Type* consume_type() {
  Type* ty = calloc(1, sizeof(Type));
  if (consume("int")) {
    ty->kind = INT;
  } else if (consume("char")) {
    ty->kind = CHAR;
  } else if (consume("void")) {
    ty->kind = VOID;

  } else if (consume("struct")) {
    ty->kind = STRUCT;

    if (inspect("{")) {
      ty->tag = "";     // No tag name, Anonymous.
      ty->tag_len = 0;
    } else {
      Token* tok = consume_ident();
      ty->tag = tok->str;
      ty->tag_len = tok->len;
    }

  } else if (consume("enum")) {
    ty->kind = ENUM;
    if (inspect("{")) {
      ty->tag = "";     // No tag name, Anonymous.
      ty->tag_len = 0;
    } else {
      Token* tok = consume_ident();
      ty->tag = tok->str;
      ty->tag_len = tok->len;
    }

  } else if (consume("...")) { // builtin, variadic list. signature.
    ty->kind = VA;
    ty->tag = "...";
    ty->tag_len = 3;

  } else if (consume("__builtin_va_list")) { // builtin, variadic list, type.
    ty->kind = VA;
    Type* ptr = calloc(1, sizeof(Type));
    ptr->kind = PTR;
    ty->tag = "__builtin_va_list";
    ty->tag_len = 17;
    ptr->ptr_to = ty;
    ty = ptr;

  } else {
    Token* tok = token;
    ty = find_typedef(tok);
    if (ty)
      consume_ident();
    else
      return NULL;
  }

  Type* ptr;
  while(consume("*")) {
    ptr = calloc(1, sizeof(Type));
    ptr->kind = PTR;
    ptr->ptr_to = ty;
    ty = ptr;
  }
  return ty;
}

// Converts type to the size.
int type_size(Type* ty) {
  if (ty->kind == CHAR)
    return 1;
  else if (ty->kind == INT)
    return 4;
  else if (ty->kind == PTR)
    return 8;
  else if (ty->kind == ARRAY)
    return ty->array_size * type_size(ty->ptr_to);

  else if (ty->kind == STRUCT) {
    Tag* st_mem = find_struct(ty->tag, ty->tag_len);
    st_mem = st_mem->memb;

    int size = 0;
    while (st_mem) {
      size += type_size(st_mem->ty);
      st_mem = st_mem->next;
    }
    return size;
  }
  else if (ty->kind == ENUM)
    return 8;
  else if (ty->kind == VOID)
    return 0;
  else
    error("Unsupported type at type_size");
  return 0; // unreachable.
}

int ceil_bound8(int size) {
  if (size % 8 == 0)
    return size;
  return (size / 8 * 8) + 8;
}


// Returns offset of a member of struct, by the member's name.
// args:
//      memb: struct's members
//      tok : member's tag name
//      flag: For offset calculation. should be set 0, in use.
//
int memb_off(Tag* memb, Token* tok, int flag) {
  if (!memb)
    return 0;

  int acc = 0;
  if (memb->len == tok->len && !strncmp(memb->name, tok->str, tok->len))
    acc = memb_off(memb->next, tok, 1);
  else
    acc = memb_off(memb->next, tok, flag);

  if (flag)
    acc = acc + type_size(memb->ty);
  return acc;
}

// Returns type of a member of struct, by the member's name.
Type* memb_type(Tag* memb, Token* tok) {
  if (!memb)
    return NULL;

  if (memb->len == tok->len && !strncmp(memb->name, tok->str, tok->len))
    return memb->ty;

  return memb_type(memb->next, tok);
}

bool at_eof() {
  return token->kind == TK_EOF;
}

void program() {
  Node* node;
  Token *tok;
  Type* ty;
  int i = 0;
  while (!at_eof()) {
    if (consume("typedef")) {
      ty = consume_type();
      if (!ty) {
	error_at(token->str, "Not found a type on top-level space.");
      }

      if (ty->kind == STRUCT) {
	decl_struct(ty);
      } else if (ty->kind == ENUM) {
	decl_enum(ty);
      }

      Token* tok = consume_ident(); // typedef's tag name
      Tag* def = calloc(1, sizeof(Tag));
      def->ty = ty;
      def->name = calloc(1, (tok->len)+1);
      strncpy(def->name, tok->str, tok->len);
      def->name[tok->len] = '\0';
      def->len = tok->len;

      def->next = typedefs;
      typedefs = def;

      expect(";");
      continue;
    }

    // External definition
    bool is_external = false;
    if (consume("extern")) {
      is_external = true;
    }

    ty = consume_type();
    if (!ty) {
      error_at(token->str, "Not found a type on top-level space.");
    }

    if (ty->kind == STRUCT) {
      decl_struct(ty);
    } else if (ty->kind == ENUM) {
      decl_enum(ty);
    }
    if (consume(";"))
      continue;


    tok = consume_ident();

    if (inspect("(")) {
      node = func(tok, ty);
      if (!node) // The func is prototype.
	continue;

    } else if (inspect(";") || inspect("[") || inspect("=")) {
      // Definition of Global variable

      node = calloc(1, sizeof(Node));
      node->kind = ND_GDECL;
      node->call = calloc(1, (tok->len)+1);
      strncpy(node->call, tok->str, tok->len);
      node->call[tok->len] = '\0';

      node->is_ext = is_external;

      LVar* gvar = calloc(1, sizeof(LVar));
      gvar->next = globals;
      gvar->name = tok->str;
      gvar->len = tok->len;

      gvar->offset = type_size(ty);
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

	  lvar->lite = calloc(1, (initok->len)+1);
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
	    node->ty->array_size = vec_len(ininode->param);
	    node->rhs = ininode;

	  } else {
	    Node* ininode = add();
	    if (ininode->kind == ND_NUM) {
	      node->rhs = ininode;
	    } else if (ininode->kind == ND_ADDR) {
	      node->rhs = ininode;
	    } else if (ininode->kind == ND_ADD) {// addition of address and constants, on global variables
	      node->rhs = ininode;
	    } else if (ininode->kind == ND_NULL) {// addition of address and constants, on global variables
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

Node *func(Token* tok, Type* ty) {
  Node *node = NULL;
  locals = NULL;

  // The tok is identifier.
  if (tok) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FUNC;
    if (consume("(")) {
      node->call = calloc(1, (tok->len)+1);
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

  // register node -> funcs
  Tag* func = calloc(1, sizeof(Tag));
  func->name = node->call;
  func->len = tok->len;
  func->ty = ty;

  Tag* param_tag;
  for (Vec* param = node->param; param; param = param->next) {
    param_tag = calloc(1, sizeof(Tag));
    param_tag->ty = param->node->lhs->ty;

    param_tag->next = func->memb;
    func->memb = param_tag;
  }

  func->next = funcs;
  funcs = func;

  if (consume(";")) {
    return NULL;
  }

  node->rhs = stmt();
  node->locals = locals ? locals->offset : 0; // total of offsets for locals.

  // Reset dictionary of local variable, for total of offset to be correct.
  // About memory leaks, let the kernel do the memory freeing.
  locals = NULL;
  return node;
}

Node *stmt() {
  Node* node = NULL;

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
  Type* ty = consume_type();
  if (ty) {
    Token* decl_tok = consume_ident();
    node = decl_lvar(ty, decl_tok);
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
      Vec* elist = calloc(1, sizeof(Vec));
      elist->node = stmt();
      elist->next = node->block;
      node->block = elist;
    }
    return node;
  }
  // Control flow switch.
  if (consume("switch")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_SWITCH;
    expect("(");
    node->lhs = expr();
    expect(")");

    expect("{");
    while(!consume("}")) {
      Vec* elem = calloc(1, sizeof(Vec));
      elem->node = stmt();
      elem->next = NULL;

      Vec* last = vec_last(node->block);
      if (last)
	last->next = elem;
      else
	node->block = elem;
    }
    return node;
  }

  if (consume("case")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_CASE;
    node->lhs = primary();
    expect(":");

    // Body of case statement.
    Node* cbody = calloc(1, sizeof(Node));
    cbody->kind = ND_BLOCK;

    while(!inspect("case") &&
	  !inspect("default") && !inspect("}")) {

      Vec* elem = calloc(1, sizeof(Vec));
      elem->node = stmt();
      elem->next = cbody->block;
      cbody->block = elem;
    }

    node->rhs = cbody;
    return node;
  }

  if (consume("break")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BRK;
    consume(";");
    return node;
  }

  if (consume("default")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_DEFAU;
    consume(":");

    // Body of default label.
    Node* dbody = calloc(1, sizeof(Node));
    dbody->kind = ND_BLOCK;

    while(!inspect("case") && !inspect("}")) {
      Vec* elem = calloc(1, sizeof(Vec));
      elem->node = stmt();
      elem->next = dbody->block;
      dbody->block = elem;
    }
    node->lhs = dbody;
    return node;
  }

  // Control flow, for, while.
  if (consume("for")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;

    Type* bty;              // For a loop scope variable
    Token* for_tok = NULL;

    Node* tmp = calloc(1, sizeof(Node));
    expect("(");
    if (consume(";")) {     // Clause 1
      tmp->lhs = NULL;
    } else {
      bty = consume_type();
      if (bty) {            // With initialization of block scope variable.
	for_tok = consume_ident();
	tmp->lhs = decl_lvar(bty, for_tok);
      } else {
	tmp->lhs = expr();
	expect(";");
      }
    }

    if (consume(";")) {     // Clause 2
      tmp->rhs = new_node_num(1);
    } else {
      tmp->rhs = expr();
      expect(";");
    }
    node->lhs = tmp;

    tmp = calloc(1, sizeof(Node));
    if (consume(")")) {     // Clause 3
      tmp->lhs = NULL;
    } else {
      tmp->lhs = expr();
      expect(")");
    }

    tmp->rhs = stmt();      // Loop Body
    node->rhs = tmp;

    if (for_tok) {
      LVar* lvar = find_lvar(for_tok);
      lvar->name = "";      // Disable to use the block variable, but reserve space for it.
      lvar->len  = 0;
    }
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
    if (consume(";"))
      return node;
    else
      node->lhs = expr();
    expect(";");
    return node;
  }

  if (consume("continue")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_CONTIN;
    expect(";");
    return node;
  }

  if (consume(";"))
    return node;

  node = expr();
  expect(";");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = cond();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  if (consume("+="))
    node = new_node(ND_ASSIGN, node, new_node(ND_ADD, node, add()));
  if (consume("-="))
    node = new_node(ND_ASSIGN, node, new_node(ND_SUB, node, add()));
  if (consume("*="))
    node = new_node(ND_ASSIGN, node, new_node(ND_MUL, node, add()));
  if (consume("/="))
    node = new_node(ND_ASSIGN, node, new_node(ND_DIV, node, add()));
  if (consume("%="))
    node = new_node(ND_ASSIGN, node, new_node(ND_REM, node, add()));
  return node;
}

Node *cond() {
  Node *node = logical_or();
  for (;;) {
    if (consume("?"))
      node = new_node(ND_COND, node, logical_or());
    else if (consume(":"))
      node = new_node(ND_CONDOR, node, cond());
    else
      return node;
  }
  return node;
}

Node *logical_or() {
  Node *node = logical_and();
  for (;;) {
    if (consume("||"))
      node = new_node(ND_OR, node, logical_and());
    else
      return node;
  }
}

Node *logical_and() {
  Node *node = bitwise_or();
  for (;;) {
    if (consume("&&"))
      node = new_node(ND_AND, node, bitwise_or());
    else
      return node;
  }
}

Node *bitwise_or() {
  Node *node = bitwise_xor();
  for (;;) {
    if (consume("|"))
      node = new_node(ND_BITOR, node, bitwise_xor());
    else
      return node;
  }
}

Node *bitwise_xor() {
  Node *node = bitwise_and();
  for (;;) {
    if (consume("^"))
      node = new_node(ND_BITXOR, node, bitwise_and());
    else
      return node;
  }
}

Node *bitwise_and() {
  Node* node = equality();
  for (;;) {
    if (consume("&"))
      node = new_node(ND_BITAND, node, equality());
    else
      return node;
  }
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
    } else if (consume("%")) {
      ty = node->ty;
      node = new_node(ND_REM, node, unary());
      node->ty = ty;
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
    for (; ty; ty = ty->ptr_to)
      ;
    node->ty = ty;
    return node;
  }

  if (consume("!")) {
    node = unary();
    ty = node->ty;
    node = new_node(ND_EQL, node, new_node_num(0));
    node->ty = ty;
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
  if (consume(".")) {
    Token* tok = consume_ident();
    Tag* st = find_struct(node->ty->tag, node->ty->tag_len);
    int offset = memb_off(st->memb, tok, 0);
    Type* mem_ty = memb_type(st->memb, tok);

    node = new_node(ND_ADDR, NULL, node);
    node->ty = node->rhs->ty;
    node = new_node(ND_ADD, node, new_node_num(offset));
    node->ty = node->lhs->ty;

    node = new_node(ND_MEMB, node, NULL);
    node->ty = mem_ty;

    return node;
  }
  if (inspect("->")) {
    for (;;) {
      if (consume("->")) {
	Token* tok = consume_ident();
	Tag*   st  = find_struct(node->ty->ptr_to->tag, node->ty->ptr_to->tag_len);
	Type* mem_ty = memb_type(st->memb, tok);
	int offset   = memb_off(st->memb, tok, 0);

	node = new_node(ND_ADD, node, new_node_num(offset));
	node->ty = node->lhs->ty;

	node = new_node(ND_MEMB, node, NULL);
	node->ty = mem_ty;

      } else
	break;
    }
  }
  if (consume("[")) {
    // Change `index of array` to dereferencing of a pointer calculation.
    //
    // a[i] -> *(a + i)
    //
    Type* ty;
    if (node->ty && node->ty->kind == ARRAY) {
      ty = calloc(1, sizeof(Type));
      ty->kind = PTR;
      ty->ptr_to = node->ty;
      node->ty = ty;
    }

    ty = node->ty;
    node = new_node(ND_ADD, node, add());
    node->ty = ty;
    if (node->rhs->ty && node->rhs->ty->kind == PTR)
      node->ty = node->rhs->ty;

    ty = node->ty;
    node = new_node(ND_DEREF, NULL, node);
    node->ty = ty->ptr_to->ptr_to;

    expect("]");
    return node;
  }
  return node;
}

Node *primary() {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    if (consume("(")) {
      node->kind = ND_CALL;
      node->call = calloc(1, (tok->len)+1);
      strncpy(node->call, tok->str, tok->len);
      node->call[tok->len] = '\0';

      while(!consume(")")) {
	Vec* param = calloc(1, sizeof(Vec));
	param->node = expr();
	param->next = node->param;
	node->param = param;
	consume(",");
      }

      // Return type
      Type* fn_ty = find_func(tok);
      if (fn_ty) {
	node->ty = fn_ty;
      }

    } else {

      LVar *lvar = find_lvar(tok);
      if (lvar) {
	if (lvar->ty->kind == PTR && lvar->ty->ptr_to->kind == VA) {
	  node->kind = ND_VA;
	} else {
	  node->kind = ND_LVAR;
	}
	node->offset = lvar->offset;
	node->ty = lvar->ty;
      } else {
	LVar *gvar = find_gvar(tok);
	if (gvar) {
	  node->kind = ND_GVAR;

	  node->call = calloc(1, (gvar->len)+1);
	  strncpy(node->call, gvar->name, gvar->len);
	  node->call[gvar->len] = '\0';

	  node->ty = gvar->ty;
	} else {
	  Tag* enu = find_enum(tok);
	  if (enu) {
	    node = new_node_num(enu->iota);
	  } else {
	    error_at(tok->str, "No declaration.");
	  }
	}
      }
    }
    return node;
  }

  if (consume("__builtin_va_startf")) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_ASSIGN;
    expect("(");
    node->lhs  = expr();
    expect(",");
    node->rhs  = new_node(ND_SUB, new_node(ND_ADDR, NULL, primary()),
			  new_node_num(48));
    expect(")");
    return node;
  }

  if (consume("__builtin_null_ptr")) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NULL;
    return node;
  }

  if (consume("(")) {
    Node *node;

    // cast
    Type* ty = consume_type();
    if (ty) {
      expect(")");
      node = unary();
      node->ty = ty;
      return node;
    }

    node = expr();
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

    lvar->lite = calloc(1, (tok->len)+1);
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
    node->call = calloc(1, (tok->len)+1);
    strncpy(node->call, tok->str, tok->len); // literal string
    node->call[tok->len] = '\0';
    node->val = tok->val;
    return node;
  }

  return new_node_num(expect_number());
}


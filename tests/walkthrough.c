/* TEST TARGET: read_file_buflen */


/* Header contents for test. */
typedef __builtin_va_list __cycc_va_list;
typedef __cycc_va_list va_list;

typedef void FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int errno = 0;

char *user_input;
char *filename;

typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_LITERAL,
  TK_CHAR,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

Token *token;

typedef enum {
  ND_CONDOR,
  ND_COND,
  ND_OR,
  ND_AND,
  ND_BITOR,
  ND_BITXOR,
  ND_BITAND,
  ND_POST,
  ND_EQL,
  ND_NEQ,
  ND_LEQ,
  ND_LES,
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_REM,
  ND_ASSIGN,
  ND_LVAR,
  ND_NUM,
  ND_RETURN,
  ND_IF,
  ND_SWITCH,
  ND_CASE,
  ND_BRK,
  ND_DEFAU,
  ND_WHILE,
  ND_FOR,
  ND_CONTIN,
  ND_BLOCK,
  ND_CALL,
  ND_FUNC,
  ND_PARAM,
  ND_DECL,
  ND_ADDR,
  ND_DEREF,
  ND_SIZE,
  ND_NEGA,
  ND_GDECL,
  ND_GVAR,
  ND_LITER,
  ND_CHAR,
  ND_MEMB,
  ND_VA,
  ND_NULL,
} NodeKind;


typedef struct Vec Vec;


typedef struct Type Type;

typedef struct Node Node;


struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
  Type* ty;
  Vec* block;
  char* call;
  Vec* param;
  int locals;
  int is_ext;
};

struct Vec {
  Vec*  next;
  Node* node;
};

typedef enum {
  CHAR,
  INT,
  PTR,
  ARRAY,
  STRUCT,
  ENUM,
  VOID,
  VA,
} TypeKind;

struct Type {
  TypeKind kind;
  Type* ptr_to;
  int array_size;
  char* tag;
  int tag_len;
};

// AST of the code.
Node* code[1000];

typedef struct LVar LVar;

// Local variable.
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
  Type* ty;
  char* lite;
};


LVar *locals;


LVar *globals;


LVar *literals;


LVar *initials;

typedef struct Tag Tag;


struct Tag {
  Tag *next;
  Type* ty;
  char* name;
  int len;
  Tag* memb;
  int  iota;
};


Tag *structs;

Tag *enums;


Tag *typedefs;


Tag *funcs;

// stub.
void program();
Node *func();
Node *stmt();
Node *expr();
Node *assign();
Node *cond();
Node *logical_or();
Node *logical_and();
Node *bitwise_or();
Node *bitwise_xor();
Node *bitwise_and();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();
int vec_len(Vec* elem);
int unique_num();
Vec* vec_last(Vec* elem);
void gen(Node *node);






void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at_file(char *loc, char *msg) {
  char *line = loc;
  while (user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  // Counts lines.
  int line_num = 1;
  for (char *p = user_input; p < line; p++)
    if (*p == '\n')
      line_num++;

  int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // Indicates the error.
  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  if (filename) {
    error_at_file(loc, fmt);
    return;
  }
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

char* read_file_buflen(char* path, int* len) {
  FILE* fp = fopen(path, "r");
  if(!fp)
    error("cannot open %s: %s", path, strerror(errno));

  // length of file.
  if (fseek(fp, 0, SEEK_END) == -1)
    error("%s: fseek: %s", path, strerror(errno));
  int size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s: fseek: %s", path, strerror(errno));

  char* buf = calloc(1, size+2+256); // 256 is for macro use space.
  fread(buf, size, 1, fp);

  // File should end with '\n\0'.
  if (size == 0 || buf[size-1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\n';
  fclose(fp);

  *len = size;
  return buf;
}



int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

bool bool_to_int(char* p) {
  if (strncmp(p, "bool", 4) == 0 && !is_alnum(p[4])) {
    strncpy(p, "int ", 4);
    p += 4;
    return true;
  }

  if (strncmp(p, "true", 4) == 0 && !is_alnum(p[4])) {
    strncpy(p, "1   ", 4);
    p += 4;
    return true;
  }

  if (strncmp(p, "false", 5) == 0 && !is_alnum(p[5])) {
    strncpy(p, "0    ", 5);
    p += 4;
    return true;
  }

  return false;
}

bool is_include(char* p){
  if (strncmp(p, "#include", 8) == 0) {
    return true;
  }
  return false;
}

char* include_name(char* p){
  char* bufp   = p; // name string copied.
  char* startp = NULL;
  char* endp   = NULL;
  int len = 0;

  while (*bufp != '\n') {
    if (*bufp == 34 && !startp)  // '\"',  searching current directory, ONLY .
      startp = bufp;
    else if (*bufp == 34 && startp)
      endp = bufp;

    bufp++;
  }

  if (!startp) {
    return NULL ;
  }

  // "include"
  len = endp - startp;
  bufp = calloc(1, len);

  strncpy(bufp, startp+1, len);
  bufp[len-1] = '\0';
  return bufp;
}



char* preproc_buflen(char* p, int len) {
  char* startp = p;
  int ctr_line = 0;
  char* inc;
  char* incp;
  int   inclen = 0;
  int in_lit = 0;

  if (*p == '#') {
    ctr_line = 1;
    if (is_include(p)) {
      inc = include_name(p);
      if (inc) {
	incp = read_file_buflen(inc, &inclen);// inc is buffer of include file.
      }
    } else {
      inc = NULL;
    }
  }


  while (*p) {
    if ((*p == '#') && (*(p-1) == '\n')) {
      ctr_line = 1;
    }
    if (*p == '\n')
      ctr_line = 0;
    if (ctr_line)
      *p = 32;


    if (!in_lit && (*p == '"')) {
      in_lit = 1;
    } else if (in_lit && (*p == '"')) {
      in_lit = 0;
    }
    if (!in_lit) {
      if (bool_to_int(p)) {
	continue;
      }
    }
    if (!in_lit) {
      if (strncmp(p, "NULL", 4) == 0 && !is_alnum(p[4]) && !is_alnum(*(p-1)) && !in_lit) {
	memmove(p+18, p+4, len - (p+4 - startp));
	strncpy(p, "__builtin_null_ptr", 18);
	p += 18;
	len += 14;
	continue;
      }
    }
    p++;
  }

  if (inclen) {
    inc = incp;
    incp = calloc(1, (inclen+len));
    incp = strncpy(incp, inc, inclen);
    incp = strcat(incp, startp);
    startp = incp;

    startp = preproc_buflen(startp, inclen+len+1);
    startp[inclen+len] = '\0';
  }

  return startp;
}


Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}


int is_alpha(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         (c == '_');
}




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
    if (*p == 39) {     // single quot '\''
      char* begin = p;

      p = p + 2;
      while (*p != 39) {
	if (*p == 92)   // escape sequence '\\'
	  p++;
	p++;
      }
      p++;

      cur = new_token(TK_CHAR, cur, begin, p - begin);
      if (cur->len == 3) {
	cur->val = *(begin + 1);
      } else if (cur->len == 4) {
	if (strncmp(begin+1, "\\0", 2) == 0)
	  cur->val = 0;
	if (strncmp(begin+1, "\\t", 2) == 0)
	  cur->val = 9;
	if (strncmp(begin+1, "\\n", 2) == 0)
	  cur->val = 10;
	if (strncmp(begin+1, "\\r", 2) == 0)
	  cur->val = 13;
      } else {
	error("Invalid char literal");
      }
      continue;
    }

    // builtin va list
    if (strncmp(p, "__builtin_va_list", 17) == 0 && !is_alnum(p[17])) {
      cur = new_token(TK_RESERVED, cur, p, 17);
      p += 17;
      continue;
    }
    if (strncmp(p, "__builtin_va_startf", 19) == 0 && !is_alnum(p[19])) {
      cur = new_token(TK_RESERVED, cur, p, 19);
      p += 19;
      continue;
    }


    // builtin null pointer constant.
    if (strncmp(p, "__builtin_null_ptr", 18) == 0 && !is_alnum(p[18])) {
      cur = new_token(TK_RESERVED, cur, p, 18);
      p += 18;
      continue;
    }


    // Jump statement.
    if (strncmp(p, "continue", 8) == 0 && !is_alnum(p[8])) {
      cur = new_token(TK_RESERVED, cur, p, 8);
      p += 8;
      continue;
    }

    // Size of the type.
    if (strncmp(p, "typedef", 7) == 0 && !is_alnum(p[7])) {
      cur = new_token(TK_RESERVED, cur, p, 7);
      p += 7;
      continue;
    }

    // Linkage.
    if (strncmp(p, "extern", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }

    // Size of the type.
    if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }

    // default in switch.
    if (strncmp(p, "default", 7) == 0 && !is_alnum(p[7])) {
      cur = new_token(TK_RESERVED, cur, p, 7);
      p += 7;
      continue;
    }
    // break in switch.
    if (strncmp(p, "break", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_RESERVED, cur, p, 5);
      p += 5;
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
    if (strncmp(p, "struct", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }
    if (strncmp(p, "enum", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_RESERVED, cur, p, 4);
      p += 4;
      continue;
    }
    if (strncmp(p, "void", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_RESERVED, cur, p, 4);
      p += 4;
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
    // Control switch
    if (strncmp(p, "switch", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }
    if (strncmp(p, "case", 4) == 0 && !is_alnum(p[4])) {
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

    // For valist builtin.
    if (strncmp(p, "...", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_RESERVED, cur, p, 3);
      p += 3;
      continue;
    }

    // Identifier
    if (is_alpha(*p)) {
      char* begin = p;
      while (is_alnum(*p))
	p++;
      cur = new_token(TK_IDENT, cur, begin, p - begin);
      continue;
    }


    // assign
    if (strncmp(p, "+=", 2) == 0||
        strncmp(p, "-=", 2) == 0||
        strncmp(p, "*=", 2) == 0||
	strncmp(p, "/=", 2) == 0||
	strncmp(p, "%=", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // logical
    if (strncmp(p, "&&", 2) == 0 ||
        strncmp(p, "||", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // postfix
    if (strncmp(p, "++", 2) == 0 ||
	strncmp(p, "--", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (strncmp(p, "->", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // bitwise or
    if (*p == '|') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }
    // bitwise xor
    if (*p == '^') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }
    // Referencing. Bitwise AND.
    if (*p == '&') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
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
    // Case statement. Conditional branch (OR).
    if (*p == ':') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }


    // comma separated expressions
    if (*p == ',') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // Struct member access expression
    if (*p == '.') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // Conditional branch.
    if (*p == '?') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

   // Negation.
    if (*p == '!') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (*p == '+' || *p == '-' ||
        *p == '*' || *p == '/' ||
	*p == '(' || *p == ')' || *p == '%') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("cannot tokenize");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}





bool consume(char* op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len  ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token* consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token* cur = token;
  token = token->next;
  return cur;
}

Token* consume_char() {
  if (token->kind != TK_CHAR)
    return NULL;
  Token* cur = token;
  token = token->next;
  return cur;
}

Token* consume_literal() {
  if (token->kind != TK_LITERAL)
    return NULL;
  Token* cur = token;
  token = token->next;
  return cur;
}


void expect(char* op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len  ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "expected '%s', but '%s'", op, token->str);
  }
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "Token is not a NUMBER");
  int val = token->val;
  token = token->next;
  return val;
}

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

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

LVar *find_gvar(Token *tok) {
  for (LVar *var = globals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

LVar *find_initials(char* name) {
  int len = strlen(name);
  for (LVar *var = initials; var; var = var->next)
    if (var->len == len && !memcmp(name, var->name, var->len))
      return var;
  return NULL;
}

Tag *find_struct(char* name, int len) {
  for (Tag *tag = structs; tag; tag = tag->next)
    if (tag->len == len && !memcmp(name, tag->name, tag->len))
      return tag;
  return NULL;
}

Tag *find_enum(Token *tok) {
  for (Tag *tag = enums; tag; tag = tag->next)
    if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len))
      return tag;
  return NULL;
}

Type *find_typedef(Token *tok) {
  for (Tag *tag = typedefs; tag; tag = tag->next)
    if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len))
      return tag->ty;
  return NULL;
}

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
      memb_tag->name = memb_tok->len;

      // Updates struct's dictionary.
      memb_tag->next = st->memb;
      st->memb = memb_tag;

      consume(";");
    }
  }
}

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

int memb_off(Tag* memb, Token* tok, int flag) {
  if (!memb)
    return 0;

  int acc = 0;
  if (!strncmp(memb->name, tok->str, tok->len))
    acc = memb_off(memb->next, tok, 1);
  else
    acc = memb_off(memb->next, tok, flag);

  if (flag)
    acc = acc + type_size(memb->ty);
  return acc;
}

Type* memb_type(Tag* memb, Token* tok) {
  if (!memb)
    return NULL;

  if (!strncmp(memb->name, tok->str, tok->len))
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
      node = func(tok);
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

  if (consume(";")) {
    // register node -> funcs
    Tag* func = calloc(1, sizeof(Tag));
    func->name = node->call;
    func->ty = NULL;

    Tag* param_tag;
    for (Vec* param = node->param; param; param = param->next) {
      param_tag = calloc(1, sizeof(Tag));
      param_tag->ty = param->node->lhs->ty;

      param_tag->next = func->memb;
      func->memb = param_tag;
    }

    func->next = funcs;
    funcs = func;
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



char* arg_to_reg[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

int unique_number = 0;
int unique_num() {
  return unique_number++;
}

// Uniqueness for Jump statements in a switch, break loops.
int switch_uniq_number  = 0;
int switch_uniq_current = 0;

int uniq_stack[64];
int uniq_stack_i = 0;

void push_uniq(int uniq) {
  uniq_stack_i++;
  uniq_stack[uniq_stack_i] = uniq;
}

int pop_uniq() {
  int uniq;
  uniq = uniq_stack[uniq_stack_i];
  uniq_stack_i--;
  return uniq;
}

int top_uniq() {
  return uniq_stack[uniq_stack_i];
}

int switch_uniq_num() {
  int u = switch_uniq_number;
  switch_uniq_current = u;

  switch_uniq_number++;
  return u;
}

int switch_uniq_has() {
  return switch_uniq_current;
}

void gen_vec_rev(Vec* elem) {
  if (!elem)
    return;
  gen_vec_rev(elem->next);
  gen(elem->node);
}

void gen_vec(Vec* elem) {
  if (!elem)
    return;
  gen(elem->node);
  gen_vec(elem->next);
}

Vec* vec_last(Vec* elem) {
  if (!elem)
    return NULL;
  if (!elem->next) {
    return elem;
  }
  return vec_last(elem->next);
}

int vec_len(Vec* elem) {
  if (!elem)
    return 0;
  int acc = vec_len(elem->next);
  acc++;
  return acc;
}

int init_block_vec(Vec* elem, int acc) {
  if (!elem)
    return 0;
  acc = init_block_vec(elem->next, acc);
  acc++;

  printf("  .long %d\n", elem->node->val);
  return acc;
}

int init_block_vec_char(Vec* elem, int acc) {
  if (!elem)
    return 0;
  acc = init_block_vec_char(elem->next, acc);
  acc++;

  printf("  .byte %d\n", elem->node->val);
  return acc;
}



int init_block_vec_liter(Vec* elem, int acc) {
  if (!elem)
    return 0;
  acc = init_block_vec_liter(elem->next, acc);
  acc++;

  printf("  .quad %s\n", elem->node->call);
  return acc;
}

bool has_va(Vec* elem) {
  if (!elem)
    return false;
  if (elem->node->kind == ND_PARAM && elem->node->lhs->ty && elem->node->lhs->ty->kind == VA ||
      elem->node->kind == ND_VA)
    return true;
  return has_va(elem->next);
}

void gen_deref(Node* node, int acc) {
  // Counts the number of dereferencing, using accumrator parameter.
  if (node->rhs->kind == ND_DEREF) {
    acc++;
    gen_deref(node->rhs, acc);
    return;
  }

  // Do dereferencing.
  gen(node->rhs);
  for (int i = 0; i < acc; i++) {
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
  }

  if (node->rhs) {
    // The type, be pointed to.
    if (node->rhs->ty->ptr_to->kind == CHAR) {
      printf("  pop rax\n");
      printf("  movzx rax, BYTE PTR[rax]\n");
      printf("  push rax\n");
    } else if (node->rhs->ty->ptr_to->kind == INT) {
      printf("  pop rax\n");
      printf("  mov eax, DWORD PTR[rax]\n");
      printf("  push rax\n");
    } else if (node->rhs->ty->ptr_to->kind == PTR) {
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax]\n");
      printf("  push rax\n");

    } else if (node->rhs->ty->ptr_to->kind == ARRAY) {  // Dereferencing an array,
      if (node->rhs->ty->ptr_to->ptr_to->kind == CHAR) { // the unit is element's size.
	printf("  pop rax\n");
	printf("  movzx eax, BYTE PTR[rax]\n");
	printf("  push rax\n");

      } else if (node->rhs->ty->ptr_to->ptr_to->kind == INT) { // the unit is element's size.
	printf("  pop rax\n");
	printf("  mov eax, DWORD PTR[rax]\n");
	printf("  push rax\n");
      } else if (node->rhs->ty->ptr_to->ptr_to->kind == PTR) {
	printf("  pop rax\n");
	printf("  mov rax, QWORD PTR[rax]\n");
	printf("  push rax\n");
      } else {
	error("Invalid type at dereferencing an array.\n");
      }
    } else if (node->rhs->ty->ptr_to->kind == STRUCT) {
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax]\n");
      printf("  push rax\n");
    } else {
      error("Invalid type at dereferencing.\n");
    }
  } else {
    error("Failed to find lvar node at dereferencing.\n");
  }
  return;
}


void gen_lval(Node *node) {
  if (node->kind != ND_LVAR && node->kind != ND_GVAR &&
      node->kind != ND_VA) {
    error("Not lvalue of the assignment");
  }

  if (node->kind == ND_LVAR || node->kind == ND_VA) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");

  } else  if (node->kind == ND_GVAR) {
    if (node->ty->kind == CHAR) {
      printf("  lea rax, BYTE PTR %s[rip]\n", node->call);

    } else if (node->ty->kind == INT) {
      printf("  lea rax, DWORD PTR %s[rip]\n", node->call);

    } else if (node->ty->kind == PTR) {
      if (node->ty->ptr_to->kind == ARRAY) {
	if (node->ty->ptr_to->ptr_to->kind == CHAR) {
	  printf("  lea rax, BYTE PTR %s[rip]\n", node->call);
	} else if (node->ty->ptr_to->ptr_to->kind == INT) {
	  printf("  lea rax, DWORD PTR %s[rip]\n", node->call);
	} else if(node->ty->ptr_to->ptr_to->kind == PTR) {
	  printf("  lea rax, QWORD PTR %s[rip]\n", node->call);
	} else {
	  error("Unsupported type of array at `gen_lval'");
	}
      } else {
	  printf("  lea rax, QWORD PTR %s[rip]\n", node->call);
      }
    } else if (node->ty->kind == ARRAY) {
      if (node->ty->ptr_to->kind == CHAR) {
	printf("  lea rax, BYTE PTR %s[rip]\n", node->call);
      } else if (node->ty->ptr_to->kind == INT) {
	printf("  lea rax, DWORD PTR %s[rip]\n", node->call);
      } else if(node->ty->ptr_to->kind == PTR) {
	printf("  lea rax, QWORD PTR %s[rip]\n", node->call);
      } else {
	error("Unsupported type of array at `gen_lval'");
      }
    } else {
      error("Unsupported type of `gen_lval'");
    }
    printf("  push rax\n");
  }
}

void gen(Node *node) {
  int uniq; // Make a label to have a unique label.
  int  sw_uniq;           // Have a label to end in ND_SWITCH, ND_CONTIN.
  Vec* sw_default = NULL; // Have a temprarily node to end in ND_SWITCH.
  Vec* sw_elem = NULL;    // Have a temprarily node to be handled in ND_SWITCH.

  switch (node->kind) {
  case ND_NUM:
  case ND_CHAR:
    printf("  push %d\n", node->val);
    return;
  case ND_NULL:
    printf("  xor rax, rax\n");
    printf("  push rax\n");
    return;
  case ND_LVAR:
    gen_lval(node);
    if (node->ty->kind == PTR &&
	node->ty->ptr_to &&
	node->ty->ptr_to->kind == ARRAY)
      return;
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_VA:
    return;
  case ND_GVAR:
    gen_lval(node);
    if (node->ty->kind == PTR &&
	node->ty->ptr_to &&
	node->ty->ptr_to->kind == ARRAY)
      return;
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_LITER:
    printf("  lea rax, BYTE PTR %s[rip]\n", node->call);
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    if (node->lhs->kind == ND_DEREF)
      gen(node->lhs->rhs); // as rvalue
    else if (node->lhs->kind == ND_MEMB)
      gen(node->lhs->lhs); // as rvalue
    else
      gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    if (node->lhs && node->lhs->kind == ND_MEMB) {
      switch (type_size(node->lhs->ty)) {
      case 1:
	printf("  mov BYTE PTR  [rax], dil\n");
	break;
      case 4:
	printf("  mov DWORD PTR [rax], edi\n");
	break;
      case 8:
	printf("  mov QWORD PTR [rax], rdi\n");
	break;
      default:
	printf("  mov [rax], rdi\n");
	break;
      }
    } else if (node->lhs && node->lhs->kind == ND_DEREF) {
      switch (type_size(node->lhs->rhs->ty->ptr_to)) {
      case 1:
	printf("  mov BYTE PTR  [rax], dil\n");
	break;
      case 4:
	printf("  mov DWORD PTR [rax], edi\n");
	break;
      case 8:
	printf("  mov QWORD PTR [rax], rdi\n");
	break;
      default:
	printf("  mov [rax], rdi\n");
	break;
      }
    } else {
	printf("  mov [rax], rdi\n"); // default
    }
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    if (node->lhs) {
      gen(node->lhs);
      printf("  pop rax\n");
    } else {
      printf("  mov rax, 0\n");
    }
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    uniq = unique_num();
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", uniq);
    gen(node->rhs);
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lelse%d:\n", uniq);
    gen_vec_rev(node->block);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_SWITCH:
    sw_uniq = switch_uniq_num();
    push_uniq(sw_uniq);
    printf("#SW-BEGIN %d\n", sw_uniq);
    gen(node->lhs);
    printf("#SWITCH-COND-VALUE^\n");
    for (sw_elem = node->block; sw_elem; sw_elem = sw_elem->next) {
      if (sw_elem->node->kind == ND_DEFAU) {
	sw_default = sw_elem;
      } else if (sw_elem->node->kind == ND_CASE) {
	uniq = unique_num();
	printf("#CASE-BEGIN %d\n", uniq);
	gen(sw_elem->node->lhs);
	printf("  pop rdi\n");
	printf("  pop rax  #SWITCH-VALUE\n");
	printf("  push rax #SWITCH-VALUE-REUSE\n");
	printf("  cmp rax, rdi\n");
	printf("  je .Lcase%d\n", uniq);
	printf("  jmp .Lcaend%d\n", uniq);
	printf(".Lcase%d:\n", uniq);
	gen(sw_elem->node->rhs);
	printf("  pop rax #DISCARD-STMT-VALUE\n");
	printf(".Lcaend%d:\n", uniq);
      } else {
	error("Unsupportd node in switch.");
      }
    }
    if (sw_default)
      gen(sw_default->node);
    printf(".Lbrk%d: #SW-BREAK\n", sw_uniq);
    pop_uniq();
    return;
  case ND_DEFAU:
    printf("#ND_DEFAU\n");
    gen(node->lhs);
    return;
  case ND_BRK:
    sw_uniq = top_uniq();
    printf("  jmp .Lbrk%d\n", sw_uniq);
    return;
  case ND_WHILE:
    uniq = unique_num();
    sw_uniq = switch_uniq_num();
    push_uniq(sw_uniq);
    printf(".Lbegin%d:\n", uniq);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", uniq);

    if (node->rhs->kind == ND_BLOCK) {
      gen_vec_rev(node->rhs->block);
    } else {
      gen(node->rhs);
    }
    printf(".Lcontin%d:\n", sw_uniq);
    printf("  jmp .Lbegin%d\n", uniq);
    printf(".Lend%d:\n", uniq);
    printf(".Lbrk%d: #WHILE-END\n", sw_uniq);
    pop_uniq();
    return;
  case ND_CONTIN:
    printf("  jmp .Lcontin%d\n", top_uniq());
    return;
  case ND_FOR:
    uniq = unique_num();
    sw_uniq = switch_uniq_num();
    push_uniq(sw_uniq);
    if (node->lhs->lhs)     // Clause-1
      gen(node->lhs->lhs);
    printf(".Lbegin%d:\n", uniq);
    gen(node->lhs->rhs);    // Clause-2
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", uniq);
    if (node->rhs->rhs)     // Loop-Body
      gen(node->rhs->rhs);
    if (node->rhs->lhs)     // Clause-3
      gen(node->rhs->lhs);
    printf(".Lcontin%d:\n", sw_uniq);
    printf("  jmp .Lbegin%d\n", uniq);
    printf(".Lbrk%d: #FOR-BREAK\n", sw_uniq);
    printf(".Lend%d:\n", uniq);
    pop_uniq();
    return;
  case ND_BLOCK:
    gen_vec_rev(node->block);
    return;
  case ND_CALL:
    if (has_va(node->param)) {
      gen_vec(node->param);
      int i;
      for (i = 0; i < vec_len(node->param); i++)
	printf("  pop %s\n", arg_to_reg[i]);

      printf("  mov rax, %s\n", arg_to_reg[i-1]); // start position on ap, address.
      printf("  push [rax]\n");
      printf("  pop %s\n", arg_to_reg[i-1]);

      for (i = vec_len(node->param); i < 6; i++) {
	printf("  add rax, 8\n");
        printf("  push [rax]\n");
	printf("  pop %s\n", arg_to_reg[i]);
      }

    } else {
      gen_vec(node->param);
      for (int i = 0; i < vec_len(node->param); i++)
	printf("  pop %s\n", arg_to_reg[i]);
    }
    uniq = unique_num();
    printf("  mov rax, rsp\n");
    printf("  mov r10b, 15\n");
    printf("  movzb r10, r10b\n");
    printf("  and rax, r10\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lelse%d\n", uniq);
    printf("  push 1\n");
    printf("  jmp  .Lend%d\n", uniq);
    printf(".Lelse%d:\n", uniq);
    printf("  push 1\n");
    printf("  push 2\n");
    printf(".Lend%d:\n", uniq);
    printf("  mov rax, %d\n", vec_len(node->param));
    printf("  call %s\n", node->call);

    uniq = unique_num();
    printf("  pop rdi\n");
    printf("  cmp rdi, 1\n");
    printf("  je  .Lend%d\n", uniq);
    printf("  pop rdi\n");
    printf(".Lend%d:\n", uniq);
    printf("  push rax\n");
    return;
  case ND_FUNC:
    printf("%s:\n", node->call);
    // Prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", node->locals);
    if (has_va(node->param)) {
      gen_vec(node->param);
      for (int i = 0; i < vec_len(node->param); i++) {
	printf("  pop rax\n");
	printf("  mov [rax], %s\n", arg_to_reg[i]);
      }
      for (int i = vec_len(node->param); i < 6; i++) {
	printf("  add rax, 8\n");
	printf("  mov [rax], %s\n", arg_to_reg[i]);
      }
    } else {
      gen_vec(node->param);
      for (int i = 0; i < vec_len(node->param); i++) {
	printf("  pop rax\n");
	printf("  mov [rax], %s\n", arg_to_reg[i]);
      }
    }

    // statements of the function.
    gen(node->rhs);
    printf("  pop rax\n");

    // Epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_PARAM:
    gen_lval(node->lhs);
    return;
  case ND_GDECL:
    printf("%s:\n", node->call);
    if (node->rhs) {
      if (node->rhs->kind == ND_NUM) {
	printf("  .long %d\n", node->rhs->val);

      } else if (node->rhs->kind == ND_LITER) {
	LVar* ivar = find_initials(node->rhs->call);
	if (ivar) {
	  char* p = ivar->lite;
	  p++;
	  while(*p != '"') {
	    printf("  .byte 0x%x\n", *p);
	    p++;
	  }
	  printf("  .byte '\\0'\n");
	} else {
	  error("Not found a literal with initializer.");
	}
      } else if (node->rhs->kind == ND_ADDR) {
	printf("  .quad %s\n", node->rhs->rhs->call);
      } else if (node->rhs->kind == ND_ADD) {
	printf("  .quad %s + %d\n", node->rhs->lhs->call, node->rhs->rhs->val);
      } else if (node->rhs->kind == ND_PARAM) {
	int acc = 0;
	if (node->rhs->param->node->kind == ND_NUM)
	  acc = init_block_vec(node->rhs->param, 0);
	else if (node->rhs->param->node->kind == ND_CHAR)
	  acc = init_block_vec_char(node->rhs->param, 0);
	else if (node->rhs->param->node->kind == ND_LITER)
	  acc = init_block_vec_liter(node->rhs->param, 0);
	else
	  error("Unsupported type with block initializer.");
	if (node->ty->kind == ARRAY) {
	  acc = node->ty->array_size - acc;
	  for (; acc > 0; acc--) {
	    if (node->ty->ptr_to->kind == INT)
	      printf("  .long 0\n");
	    else if (node->ty->ptr_to->kind == CHAR)
	      printf("  .byte 0\n");
	    else if (node->ty->ptr_to->kind == PTR)
	      printf("  .quad 0\n");
	    else
	      error("Unsupported type with block initializer, remaining elements with zero.");
	  }
	}
      } else {
	error("Unsupported type with initializer.");
      }
    } else {
      printf("  .zero %d\n", node->offset);
    }
    return;
  case ND_DECL:
    if (node->rhs) {
      if (node->rhs->rhs && node->rhs->rhs->kind == ND_PARAM) {
	for (Vec* elem = node->rhs->rhs->param; elem; elem = elem->next) {
	  gen(elem->node);
	}
	gen_lval(node->rhs->lhs);
	for (Vec* elem = node->rhs->rhs->param; elem; elem = elem->next) {
	  printf("  pop rax\n");
	  printf("  pop rdi\n");
	  printf("  mov [rax], rdi\n");
	  if (node->rhs->lhs->ty->kind == ARRAY) {
	    if (node->rhs->lhs->ty->ptr_to->kind == INT)
	      printf("  add rax, 4\n");
	    else if (node->rhs->lhs->ty->ptr_to->kind == PTR)
	      printf("  add rax, 8\n");
	    else if (node->rhs->lhs->ty->ptr_to->kind == CHAR)
	      printf("  add rax, 1\n");
	    else
	      error("Unsupported type at block initialization of local variable.");
	  }
	  printf("  push rax\n");
	}
	printf("  pop rax\n"); // Drop address on top of stack.
      } else {
	gen(node->rhs);
      }
    }
    return;
  case ND_ADDR:
    gen_lval(node->rhs);
    return;
  case ND_DEREF:
    gen_deref(node, 0);
    return;
  case ND_SIZE:
    gen(node->rhs);
    return;
  case ND_POST:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rax\n");
    return;
  case ND_MEMB:
    gen(node->lhs);
    switch (type_size(node->ty)) {
    case 1:
      printf("  pop rax\n");
      printf("  movzx rax, BYTE PTR[rax] #ND_MEMB\n");
      printf("  push rax\n");
      break;
    case 4:
      printf("  pop rax\n");
      printf("  mov eax, DWORD PTR[rax] #ND_MEMB\n");
      printf("  push rax\n");
      break;
    case 8:
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax] #ND_MEMB\n");
      printf("  push rax\n");
      break;
    }
    return;
  case ND_CONDOR:
    if (!node->lhs || node->lhs->kind != ND_COND)
      error("Should have conditional node.");
    uniq = unique_num();
    gen(node->lhs->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .Lok%d\n", uniq);
    gen(node->rhs);
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lok%d:\n", uniq);
    gen(node->lhs->rhs);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_AND:
    uniq = unique_num();
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  xor rsi, rsi\n");
    printf("  cmp rax, rsi\n");
    printf("  je .Lante%d\n", uniq);
    gen(node->rhs);
    printf("  xor rsi, rsi\n");
    printf("  pop rdi\n");
    printf("  cmp rdi, rsi\n");
    printf("  je .Ldesc%d\n", uniq);
    printf("  mov rax, 1\n");
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lante%d:\n", uniq);
    printf(".Ldesc%d:\n", uniq);
    printf("  mov rax, 0\n");
    printf(".Lend%d:\n", uniq);
    printf("  push rax\n");
    return;
  case ND_OR:
    uniq = unique_num();
    gen(node->lhs);
    printf("  xor rsi, rsi\n");
    printf("  pop rax\n");
    printf("  cmp rax, rsi\n");
    printf("  jne .Lante%d\n", uniq);
    gen(node->rhs);
    printf("  xor rsi, rsi\n");
    printf("  pop rdi\n");
    printf("  cmp rdi, rsi\n");
    printf("  jne .Ldesc%d\n", uniq);
    printf("  mov rax, 0\n");
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lante%d:\n", uniq);
    printf(".Ldesc%d:\n", uniq);
    printf("  mov rax, 1\n");
    printf(".Lend%d:\n", uniq);
    printf("  push rax\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    if (node->lhs && node->lhs->ty && node->lhs->ty->kind == PTR){

      if (node->lhs->ty->ptr_to->kind == PTR)
	printf("  imul rdi, 8\n");
      if (node->lhs->ty->ptr_to->kind == INT)
	printf("  imul rdi, 4\n");
      if (node->lhs->ty->ptr_to->kind == CHAR)
	printf("  imul rdi, 1\n");

      // Evaluating addition of pointer to array, the unit is determined by the element's size.
      // Array's data structure is...
      //
      // PTR -> ARRAY -> (Element's type)
      //
      if (node->lhs->ty->ptr_to->kind == ARRAY) {
	if (node->lhs->ty->ptr_to->ptr_to->kind == PTR) // elements of the array..
	  printf("  imul rdi, 8\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rdi, 4\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == CHAR)
	  printf("  imul rdi, 1\n");
      }
    }
    if (node->rhs && node->rhs->ty && node->rhs->ty->kind == PTR) {

      if (node->rhs->ty->ptr_to->kind == PTR)
	printf("  imul rax, 8\n");
      if (node->rhs->ty->ptr_to->kind == INT)
	printf("  imul rax, 4\n");
      if (node->rhs->ty->ptr_to->kind == CHAR)
	printf("  imul rax, 1\n");

      // Evaluating addition of pointer to array, the unit is determined by the element's size.
      // Array's data structure is...
      //
      // PTR -> ARRAY -> (Element's type)
      //
      if (node->rhs->ty->ptr_to->kind == ARRAY) {
	if (node->rhs->ty->ptr_to->ptr_to->kind == PTR) // elements of the array..
	  printf("  imul rax, 8\n");
	if (node->rhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rax, 4\n");
	if (node->rhs->ty->ptr_to->ptr_to->kind == CHAR)
	  printf("  imul rax, 1\n");
      }
    }
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    if (node->lhs && node->lhs->ty && node->lhs->ty->kind == PTR) {

      if (node->lhs->ty->ptr_to->kind == PTR)
	printf("  imul rdi, 8\n");
      if (node->lhs->ty->ptr_to->kind == INT)
	printf("  imul rdi, 4\n");
      if (node->lhs->ty->ptr_to->kind == CHAR)
	printf("  imul rdi, 1\n");

      // Evaluating substraction of pointer to array, same as addition of it.
      if (node->lhs->ty->ptr_to->kind == ARRAY) {
	if (node->lhs->ty->ptr_to->ptr_to->kind == PTR)
	  printf("  imul rdi, 8\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rdi, 4\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == CHAR)
	  printf("  imul rdi, 1\n");
      }
    }
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_REM:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  mov rax, rdx\n");
    break;
  case ND_EQL:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LEQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LES:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_BITOR:
    printf("  or rax, rdi\n");
    break;
  case ND_BITXOR:
    printf("  xor rax, rdi\n");
    break;
  case ND_BITAND:
    printf("  and eax, edi\n");
    break;
  }

  printf("  push rax\n");
}



/* TEST TARGET */


void genasm() {
  printf(".intel_syntax noprefix\n");

  Node* node;
  int i = 0;
  while (code[i]) {
    node = code[i];
    if (node->kind == ND_FUNC && strcmp(node->call, "")) {
      printf(".globl %s\n", node->call);
    }
    i++;
  }

  printf(".data\n");
  for (LVar* var = literals; var; var = var->next) {
    printf("%s:\n", var->name);
    printf("  .string %s\n", var->lite);
  }

  int text = 0;
  i = 0;
  while (code[i]) {
    node = code[i];
    if (node->kind == ND_FUNC && !text) {
      printf(".text\n");
      text = 1;
    }

    if (node->is_ext) {
      i++;
      continue;
    }

    gen(code[i++]);
  }
}

/* MAIN FOR TEST */
int main(int argc, char **argv) {
  int in_len = 0;  // Length of a buffer for a input file.
  Token* cur;

  if (argc < 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  if (strcmp("-e", argv[1]) == 0) {
    user_input = argv[2];
    token = tokenize(argv[2]);
  } else {
    filename = argv[1];
    user_input = read_file_buflen(filename, &in_len);
    user_input = preproc_buflen(user_input, in_len);
    token = tokenize(user_input);
  }

  program();

  genasm();

  return 0;
}



#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Kinds of node for Abstract Syntax Tree.
typedef enum {
  ND_CONDOR, // :
  ND_COND,   // ?
  ND_OR,   // ||
  ND_AND,  // &&
  ND_BITOR,// |
  ND_BITXOR,// ^
  ND_BITAND,// &
  ND_POST, // ++, --
  ND_EQL, // ==
  ND_NEQ, // !=
  ND_LEQ, // <=
  ND_LES, // <
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_REM, // /
  ND_ASSIGN, // =
  ND_LVAR,   // local variable.
  ND_NUM, // number
  ND_RETURN, // return statement
  ND_IF,     // control flow, if, else, else-if
  ND_SWITCH, // control flow, switch
  ND_CASE,   // control flow, case
  ND_BRK,    // control flow, break
  ND_DEFAU,  // control flow, default
  ND_WHILE,  // control flow, while
  ND_FOR,    // control flow, for
  ND_CONTIN, // control flow, continue
  ND_BLOCK,  // {}, compound statement.
  ND_CALL,   // function call
  ND_FUNC,   // function
  ND_PARAM,  // declaration for function parameters
  ND_DECL,   // declaration
  ND_ADDR,   // &, addressing, reference.
  ND_DEREF,  // *, addressing, dereference.
  ND_SIZE,   // sizeof operator.
  ND_NEGA,   // !, negation operator.
  ND_GDECL,  // declaration of global var.
  ND_GVAR,   // global variable.
  ND_LITER,  // string, literals
  ND_CHAR,   // characters
  ND_MEMB,   // '.' accessing member of struct.
} NodeKind;

// A Vector for block, compound statement.
typedef struct Vec Vec;

// A Type for variables.
typedef struct Type Type;

typedef struct Node Node;

// A node for AST.
struct Node {
  NodeKind kind;
  Node *lhs;     // left  hand side
  Node *rhs;     // right hand side
  int val;       // If kind is TK_NUM、then it's value.
  int offset;    // If kind is ND_LVAR, then it's offset from stack base pointer.
  Type* ty;      // If kind is ND_LVAR, then it's type of the data.
  Vec* block;    // If kind is ND_BLOCK, then it's Vector for statements.
  char* call;    // If kind is ND_CALL, then it's name of the function.
  Vec* param;    // If kind is ND_CALL, then it's parameters for function call.
                 // Also if kind is ND_GDECL, use in initializer with blocks.
  int locals;    // If kind is ND_FUNC, it's length of stack space for local variables.
  int is_ext;    // If kind is ND_GDECL, then it's external definition.
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
} TypeKind;

struct Type {
  TypeKind kind;
  Type* ptr_to;
  int array_size;
  char* tag;       // tag name of struct
  int tag_len;     // length of the tag name
};

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
  int val;        // If kind is TK_NUM、TK_CHAR, then it's value.
  char *str;      // A string of the token
  int len;        // Length of the token.
};

// Point to Current token.
Token *token;

// Input program.
char *user_input;
char *filename;

// AST of the code.
Node* code[100];

typedef struct LVar LVar;

// Local variable.
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset; // Offset from stack base pointer
  Type* ty;   // Type of the data.
  char* lite; // for literal
};

// Dictionary for Local variable.
LVar *locals;

// Dictionary for Global variable.
LVar *globals;

// Dictionary for literals.
LVar *literals;

// Dictionary for literals for initializer.
LVar *initials;

typedef struct Tag Tag;

// Tag for user defined type.
struct Tag {
  Tag *next;
  Type* ty;
  char* name;
  int len;
  Tag* memb;
  int  iota; // enum use.
};

// Dictionary for tags of data structure, struct.
Tag *structs;

// Dictionary for tags of constants, enum.
Tag *enums;

// Dictionary for tags of type defined..
Tag *typedefs;

// Dictionary for tags of function prototype, external.
Tag *funcs;



// Preprocess
char* preproc(char* p);
char* preproc_buflen(char* p, int len);

// Tokenize
Token *tokenize(char *p);

// Parse.
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

// Generates assemly.
void genasm();

// Utilities.
void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

int unique_num();

char* read_file(char* path);
char* read_file_buflen(char* path, int* len);

LVar *find_initials(char* name);

int vec_len(Vec* elem);

Type* consume_type();

int type_size(Type* ty);

int ceil_bound8(int size);

Vec* vec_last(Vec* elem);

int is_alnum(char c);


// Externals, terporarily.
struct _IO_FILE;
typedef struct _IO_FILE FILE;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Kinds of node for Abstract Ayntax Tree.
typedef enum {
  ND_EQL, // ==
  ND_NEQ, // !=
  ND_LEQ, // <=
  ND_LES, // <
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_ASSIGN, // =
  ND_LVAR,   // local variable.
  ND_NUM, // number
} NodeKind;

typedef struct Node Node;

// A node for AST.
struct Node {
  NodeKind kind;
  Node *lhs;     // left  hand side
  Node *rhs;     // right hand side
  int val;       // If kind is TK_NUM、then it's value.
  int offset;    // If kind is ND_LVAR, then it's offset from stack base pointer.
};

typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;        // If kind is TK_NUM、then it's value.
  char *str;      // A string of the token
  int len;        // Length of the token.
};

// Point to Current token.
Token *token;

// Input program.
char *user_input;

// AST of the code.
Node* code[100];

typedef struct LVar LVar;

// Local variable.
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset; // Offset from stack base pointer
};

// Dictionary for Local variable.
LVar *locals;

// Parse.
Token *tokenize(char *p);
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// Generates assemly.
void genasm();


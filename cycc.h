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
  ND_NUM, // number
} NodeKind;

typedef struct Node Node;

// A node for AST.
struct Node {
  NodeKind kind;
  Node *lhs;     // left  hand side
  Node *rhs;     // right hand side
  int val;       // If kind is TK_NUM、then it's value.
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

// Parse.
Token *tokenize(char *p);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// Generates assemly.
void genasm(Node *node);


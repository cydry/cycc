#include "cycc.h"

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

    // Identifier
    if (isalpha(*p)) {
      char* begin = p;
      while (is_alnum(*p))
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
    if (strncmp(p, "->", 2) == 0) {
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

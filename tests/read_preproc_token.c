/* TEST TARGET: read_file_buflen */


/* Header contents for test. */
typedef __builtin_va_list __cycc_va_list;
typedef __cycc_va_list va_list;

typedef void FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int SEEK_END = 2;
int SEEK_SET = 0;
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




/* TEST TARGET */

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

    printf("%c", *p);
    p++;
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}


/* MAIN FOR TEST */
int main(int argc, char **argv) {
  int in_len;  // Length of a buffer for a input file.

  filename = "tests/preproc.c";
  in_len = 0;
  user_input = read_file_buflen(filename, &in_len);
  user_input = preproc_buflen(user_input, in_len);
  printf("before...");
  token = tokenize(user_input);
  printf("after...");
  //  printf("%s\n", user_input);
  assert(5645, strlen(user_input));
  return 0;
}



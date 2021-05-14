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
int NULL = 0;

char *user_input;
char *filename;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
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
  char* curp = p;
  char* bufp = NULL; // name string copied.
  char* startp = NULL;
  char* endp   = NULL;
  int len = 10;
  int in = 0;
  
  printf("curp:%d\n", curp);
  while (*curp != 10) {
    if (*curp == 34 && !startp) {
      startp = curp;
      printf("startp:%d\n", startp);
    } else if (*curp == 34 && startp) {
      endp = curp;
      printf("endp:%d\n", endp);
    }

    curp++;
  }

  len = endp - startp;
  printf("startp:%d\n", startp);
  printf("len:%d\n", len);
  bufp = calloc(1, len);
  bufp[0] = 97;
  return bufp;
}



/* TEST TARGET */

char* preproc_buflen(char* p, int len) {
  char* startp = p;
  int ctr_line = 0;
  char* inc;
  int in_lit = 0;

  if (*p == '#') {
    ctr_line = 1;
      if (is_include(p)) {
	inc = include_name(p);
	printf("%c\n", *inc);
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

    p++;
  }

  return startp;
}


/* MAIN FOR TEST */
int main(int argc, char **argv) {
  int in_len;  // Length of a buffer for a input file.

  filename = "tests/preproc.c";
  in_len = 0;
  user_input = read_file_buflen(filename, &in_len);
  user_input = preproc_buflen(user_input, in_len);

  printf("%s\n", user_input);
  assert(595, in_len);
  return 0;
}



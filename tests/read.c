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

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


/* TEST TARGET */

// Read file for user input.
//
// args:
//      path: file path
//      len : as a return value, a length of result buffer.
//            invariantly, pass a buffer for the result.
//
// ret : buffer of the file.
//
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


/* MAIN FOR TEST */
int main(int argc, char **argv) {
  int in_len;  // Length of a buffer for a input file.

  filename = "tests/read.c";
  in_len = 0;
  user_input = read_file_buflen(filename, &in_len);

  printf("%s\n", user_input);
  assert(1587, in_len);
  return 0;
}


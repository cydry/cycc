#include "cycc.h"

// For input by file.
char* read_file(char* path) {

  FILE* fp = fopen(path, "r");
  if(!fp)
    error("cannot open %s: %s", path, strerror(errno));

  // length of file.
  if (fseek(fp, 0, SEEK_END) == -1)
    error("%s: fseek: %s", path, strerror(errno));
  int size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s: fseek: %s", path, strerror(errno));

  char* buf = calloc(1, size+2);
  fread(buf, size, 1, fp);

  // File should end with '\n\0'.
  if (size == 0 || buf[size-1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\n';
  fclose(fp);
  return buf;
}

// Preprocessor (Stub)
// NOTE: Replace control-line to SPACE (32|Dec), ONLY.
// args:
//      p: user input, provided by read_file.
//
char* preproc(char* p) {
  char* startp = p;
  int ctr_line = 0;

  if (*p == '#')
      ctr_line = 1;

  while (*p) {
    if ((*p == '#') && (*(p-1) == '\n'))
      ctr_line = 1;
    if (*p == '\n')
      ctr_line = 0;
    if (ctr_line)
      *p = 32;

    p++;
  }
  return startp;
}

#include "cycc.h"


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

  char* buf = calloc(1, size+2);
  fread(buf, size, 1, fp);

  // File should end with '\n\0'.
  if (size == 0 || buf[size-1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\n';
  fclose(fp);

  *len = size;
  return buf;
}


// For input by file.
char* read_file(char* path) {
  int no = 0;
  return read_file_buflen(path, &no);
}


// Built-in (Psuedo) Macro:
// (Alternative to stdbool.h)
// arg:
//     p: user input.
//
// convert user input, below;
//   bool  -> int
//   true  -> 1
//   false -> 0
//
bool bool_to_int(char* p) {
  if (strncmp(p, "bool", 4) == 0) {
    strncpy(p, "int ", 4);
    p += 4;
    return true;
  }

  if (strncmp(p, "true", 4) == 0) {
    strncpy(p, "1   ", 4);
    p += 4;
    return true;
  }

  if (strncmp(p, "false", 5) == 0) {
    strncpy(p, "0    ", 5);
    p += 4;
    return true;
  }

  return false;
}


// Preprocessing a user input.
//
// args:
//      p   : a buffer having user input.
//      len : length of the user input buffer.
//
// ret : buffer of the file, preprocessed.
//
char* preproc_buflen(char* p, int len) {
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

    if (bool_to_int(p))
      continue;

    p++;
  }
  return startp;
}


// Preprocessor (Stub)
// NOTE: Replace control-line to SPACE (32|Dec), ONLY.
// args:
//      p: user input, provided by read_file.
//
char* preproc(char* p) {
  return preproc_buflen(p, 0);
}

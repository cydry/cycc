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

  char* buf = calloc(1, (size+2)+size); // for macro use space.
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

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

// Return true if a line with poiter has #include directive.
bool is_include(char* p){
  if (strncmp(p, "#include", 8) == 0) {
    return true;
  }
  return false;
}


// Return name of include file.
// args:
//      p  : pointer to a input buffer.
//      len: as result value, length of name of a include file.
//
// return value:
//      pointer to a start of a filename
//
char* include_name(char* p){
  char* bufp   = p; // name string copied.
  char* startp = NULL;
  char* endp   = NULL;
  int len = 0;

  while (*bufp != '\n') {
    if (*bufp == 34 && !startp)  // '\"',  searching current directory, ONLY .
      startp = bufp;
    if (*bufp == 34 && startp)
      endp = bufp;

    bufp++;
  }

  if (!startp)
    return NULL;

  // "include"
  len = endp - startp;
  bufp = calloc(1, len);

  strncpy(bufp, startp+1, len);
  bufp[len-1] = '\0';
  return bufp;
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
  char* inc;         // a name of include file.
  char* incp;        // pointer to buffer for include file.
  int   inclen = 0;  // length of the buffer included.

  if (*p == '#') {
      ctr_line = 1;
      if (is_include(p)) {
	inc = include_name(p);               // inc is name of include file.
	if (inc)
	  incp = read_file_buflen(inc, &inclen);// inc is buffer of include file.
      } else {
	inc = NULL;
      }
  }

  while (*p) {
    if ((*p == '#') && (*(p-1) == '\n')) {
      ctr_line = 1;
      if (is_include(p)) {
	inc = include_name(p);               // inc is name of include file.
	if (inc)
	  incp = read_file_buflen(inc, &inclen);// inc is buffer of include file.
      } else {
	inc = NULL;
      }
    }
    if (*p == '\n')
      ctr_line = 0;
    if (ctr_line)
      *p = 32;

    if (bool_to_int(p))
      continue;

    p++;
  }

  if (inclen) {
    inc = incp;                        // Temporarily let include buffer be in 'inc'.
    incp = calloc(1, (inclen+len));
    incp = strncpy(incp, inc, inclen);
    incp = strcat(incp, startp);       // 'include buffer' + 'src file buffer'
    startp = incp;

    startp = preproc_buflen(startp, inclen+len);
  }

  return startp;
}


// Preprocessor (Stub)
// args:
//      p: user input, provided by read_file.
//
char* preproc(char* p) {
  return preproc_buflen(p, 0);
}

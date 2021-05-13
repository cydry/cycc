#include "cycc.h"

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

char* define_builtin(char* defword, int deflen, char* src, int srclen) {
  char* chunk = calloc(1, deflen+srclen+1);
  strncpy(chunk, defword, deflen);
  strcat(chunk, src);
  *(chunk + deflen + srclen) = '\0';
  return chunk;
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
    else if (*bufp == 34 && startp)
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
  int in_lit = 0;    // flag in processing a literal

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


    if (!in_lit && (*p == '"')) {
      in_lit = 1;
    } else if (in_lit && (*p == '"')) {
      in_lit = 0;
    }

    if (!in_lit) {
      if (bool_to_int(p))
	continue;
    }

    if (strncmp(p, "va_start", 8) == 0 && !is_alnum(p[8])) {
      memmove(p+19, p+8, len - (p+8 - startp));
      strncpy(p, "__builtin_va_startf", 19);
      p += 19;
      len += 11;
      continue;
    }

    p++;
  }

  if (inclen) {
    inc = incp;                        // Temporarily let include buffer be in 'inc'.
    incp = calloc(1, (inclen+len));
    incp = strncpy(incp, inc, inclen);
    incp = strcat(incp, startp);       // 'include buffer' + 'src file buffer'
    startp = incp;

    startp = preproc_buflen(startp, inclen+len);

    char* builtin_valist = "typedef void FILE;\nenum{NULL}CyccDef;\nenum{SEEK_SET,SEEK_CUR,SEEK_END}CyccStdIoFseekDef;int errno;\n";
    startp = define_builtin(builtin_valist, strlen(builtin_valist),
			    startp, inclen+len);
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

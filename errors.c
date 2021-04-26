#include "cycc.h"

// Report errors if a file is given.
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

// Reports the error with the location in the input.
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

// Reports the error with a formatted message.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

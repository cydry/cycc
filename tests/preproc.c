#include "cycc.h"

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

int main() {
  return 0;
}

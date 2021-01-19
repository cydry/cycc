#include "cycc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  user_input = argv[1]; // error_at reports using this pointer.

  token = tokenize(argv[1]);
  Node* node = expr();

  genasm(node);

  return 0;
}


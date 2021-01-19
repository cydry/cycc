#include "cycc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  user_input = argv[1]; // error_at reports using this pointer.

  token = tokenize(argv[1]);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  gen(node);
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}


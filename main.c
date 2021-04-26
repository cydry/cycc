#include "cycc.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  if (strcmp("-e", argv[1]) == 0) {
    user_input = argv[2];
    token = tokenize(argv[2]);
  } else {
    filename = argv[1];
    user_input = read_file(filename);
    token = tokenize(user_input);
  }

  program();

  genasm();

  return 0;
}


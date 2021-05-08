#include "cycc.h"

int main(int argc, char **argv) {
  int in_len;  // Length of a buffer for a input file.

  if (argc < 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  if (strcmp("-e", argv[1]) == 0) {
    user_input = argv[2];
    token = tokenize(argv[2]);

  } else if (strcmp("-E", argv[1]) == 0) {
    filename = argv[2];
    in_len = 0;
    user_input = read_file_buflen(filename, &in_len);
    user_input = preproc_buflen(user_input, in_len);
    printf("%s\n", user_input);
    return 0;

  } else {
    filename = argv[1];
    in_len = 0;
    user_input = read_file_buflen(filename, &in_len);
    user_input = preproc_buflen(user_input, in_len);
    token = tokenize(user_input);
  }

  program();

  genasm();

  return 0;
}


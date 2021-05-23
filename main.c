#include "cycc.h"

// Point to Current token.
Token *token = NULL;

// Input program.
char *user_input = NULL;
char *filename = NULL;

// AST of the code.
Node* code[1000];

// Dictionary for Local variable.
LVar *locals = NULL;

// Dictionary for Global variable.
LVar *globals = NULL;

// Dictionary for literals.
LVar *literals = NULL;

// Dictionary for literals for initializer.
LVar *initials = NULL;

// Dictionary for tags of data structure, struct.
Tag *structs = NULL;

// Dictionary for tags of constants, enum.
Tag *enums = NULL;

// Dictionary for tags of type defined..
Tag *typedefs = NULL;

// Dictionary for tags of function prototype, external.
Tag *funcs = NULL;


int main(int argc, char **argv) {
  int in_len = 0;  // Length of a buffer for a input file.

  if (argc < 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  if (strcmp("-e", argv[1]) == 0) {
    user_input = argv[2];
    token = tokenize(argv[2]);

  } else if (strcmp("-E", argv[1]) == 0) {
    filename = argv[2];
    user_input = read_file_buflen(filename, &in_len);
    user_input = preproc_buflen(user_input, in_len);
    printf("%s\n", user_input);
    return 0;

  } else {
    filename = argv[1];
    user_input = read_file_buflen(filename, &in_len);
    user_input = preproc_buflen(user_input, in_len);
    token = tokenize(user_input);
  }

  program();

  genasm();

  return 0;
}


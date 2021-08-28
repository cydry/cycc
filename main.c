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

// Command line option.
int is_cmd_option(char** argv, const char* option) {
  return strcmp(argv[1], option) == 0;
}

int main(int argc, char **argv) {
  int input_length = 0;  // Length of a buffer for a input file.

  if (argc < 2) {
    fprintf(stderr, "The number of arguments is not correct\n");
    return 1;
  }

  if (is_cmd_option(argv, "-e")) {
    user_input = argv[2];
    token = tokenize(argv[2]);

  } else if (is_cmd_option(argv, "-E")) {
    filename = argv[2];
    user_input = read_file_buflen(filename, &input_length);
    user_input = preproc_buflen(user_input, input_length);
    printf("%s\n", user_input);
    return 0;

  } else {
    filename = argv[1];
    user_input = read_file_buflen(filename, &input_length);
    user_input = preproc_buflen(user_input, input_length);
    token = tokenize(user_input);
  }

  program();

  genasm();

  return 0;
}


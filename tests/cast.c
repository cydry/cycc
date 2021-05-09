int cast_parsing_test() {
  char a = 1;
  char b = 3;
  int  c = 0;

  c = (int)(b - a);

  return c;
}

int main() {
  int got = 0;

  got = cast_parsing_test();
  assert(2, got);

  return 0;
}

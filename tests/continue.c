int continue_nested_while_loop_test() {
  int p = 19;
  while (p) {
    if (p > 11) {
      p--;
      while (p > 15) {
	p--;
      }
      continue;
    }
    p--;
  }
  return p;
}

void continue_test2() {
  char p = 0;
  while (p<98) {
    if (isspace(p)) {
      printf("%d[%c]", p, p);
      p++;
      continue;
    }
    printf("{%c}", p);
    p++;
  }
}

int main() {
  int got = 2;
  got = continue_nested_while_loop_test();
  assert(0, got);

  continue_test2();
  return 0;
}

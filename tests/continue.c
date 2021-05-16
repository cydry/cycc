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

int main() {
  int got = 2;
  got = continue_nested_while_loop_test();
  assert(0, got);
  return 0;
}

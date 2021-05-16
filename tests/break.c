int break_nested_while_loop_test() {
  int p = 19;
  while (p) {
    if (p > 11) {
      p--;
      while (p > 15)
	p--;
      continue;
    }
    if (p < 3)
      break;
    p--;
  }
  return p;
}

int main() {
  int got = 0;
  got = break_nested_while_loop_test();
  assert(2, got);
  return 0;
}

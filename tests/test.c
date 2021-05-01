int foo() {
  int x[3];
  x[0] = -1;
  x[1] = 2;
  int y;
  y = 4;
  return x[0] + y;
}

int main() {
  int got = 0;
  got = foo();
  assert(4, got);
  return 0;
}

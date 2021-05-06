int continue_test() {
  int x = 0;
  int y = 0;
  while (y < 5){
    x++;
    if (x < 5)
      continue;
    y++;
  }
  return x;
}

int main() {
  int got = 0;
  got = continue_test();
  assert(10, got);
  return 0;
}

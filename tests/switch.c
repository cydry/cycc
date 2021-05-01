int switcher(int x) {
  switch (x) {}
  return 2;
}

int main() {
  int got;

  got = switcher(0);
  assert(2, got);

  return 0;
}

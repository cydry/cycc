enum E {
  A,
  B,
  C,
};

int switcher(enum E x) {
  int y = 0;
  switch (x) {
  case A:
    y = 2;
  case B:
    y = 3;
  case C:
    y = 5;
    break;
  case C:
    y = 7;
  }
  return y;
}

int main() {
  int got = 0;
  got = switcher(C);
  assert(5, got);
  return 0;
}

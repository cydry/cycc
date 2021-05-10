int switcher(int x) {
  int y = 0;
  switch (x) {
  case 3:
    y = 2;
  case 5:
    y = 3;
  case 7:
    y = 5;
  case 11:
    y = 7;
  }
  return y;
}

int switcher1(int x) {
  int y = 0;
  switch (x) {
  case 3:
    y = 2;
  case 3:
    y = 3;
  case 7:
    y = 5;
  case 11:
    y = 7;
  }
  return y;
}

int switcher2(int x) {
  int y = 0;
  switch (x) {
  case 3:
    y = 2;
    break;
  case 3:
    y = 3;
  case 7:
    y = 5;
  case 11:
    y = 7;
  }
  return y;
}

int switcher3(int x) {
  int y = 0;
  switch (x) {
  case 3:
    y = 2;
  case 5:
    y = 3;
  case 7:
    y = 5;
    break;
  case 7:
    y = 7;
  case 11:
    y = 7;
  }
  return y;
}

int switcher4(int x) {
  int y = 0;
  switch (x) {
  case 3:
    y = 2;
  case 5:
    y = 3;
  case 7:
    y = 5;
  case 11:
    y = 7;
  default:
    y = 11;
  }
  return y;
}

int switcher5(int x) {
  int y = 0;
  switch (x) {
  case 3:
    y = 2;
  case 5:
    y = 3;
  case 7:
    y = 4;
    y = y + 1;
    break;
  case 11:
    y = 7;
  default:
    y = 11;
  }
  return y;
}

int main() {
  int got;

  got = switcher(0);
  assert(0, got);

  got = switcher(3);
  assert(2, got);

  got = switcher(5);
  assert(3, got);

  got = switcher(7);
  assert(5, got);

  got = switcher(11);
  assert(7, got);

  got = switcher(4);
  assert(0, got);

  got = switcher(6);
  assert(0, got);

  got = switcher(8);
  assert(0, got);

  got = switcher1(3);
  assert(3, got);

  got = switcher2(3);
  assert(2, got);

  got = switcher3(7);
  assert(5, got);

  got = switcher4(13);
  assert(11, got);

  got = switcher5(7);
  assert(5, got);

  return 0;
}

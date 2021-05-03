char x[3] = {'b', 'a', 'r'};  

int char_test() { return x[0]; }
int char_test1() { return x[2]; }

char y[]  = {'b', 'a', 'r'};  

int char_test2() { return y[0]; }
int char_test3() { return y[2]; }

char z[]  = {'\0', '\t', '\n', '\r'};  

int char_test4() { return z[0]; }
int char_test5() { return z[1]; }
int char_test6() { return z[2]; }
int char_test7() { return z[3]; }

int main() {
  int got = 0;

  got = char_test();
  assert(98, got);

  got = char_test1();
  assert(114, got);

  got = char_test2();
  assert(98, got);

  got = char_test3();
  assert(114, got);

  got = char_test4();
  assert(0, got);

  got = char_test5();
  assert(9, got);

  got = char_test6();
  assert(10, got);

  got = char_test7();
  assert(13, got);

  return 0;
}

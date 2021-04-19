#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./main "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert_fail() {
  input=$1
  ./main "$input" > tmp.s
  if [ $? -ne 1 ]; then
      echo "$input should cause an error."
      exit 1
  fi
}

assert_stdout() {
  expected="$1"
  input="$2"

  ./main "$input" > tmp.s
  cc -c foo.c
  cc -o tmp tmp.s foo.o
  actual=`./tmp`

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'main() {0;}'
# assert 42 '42;'
# assert 41 " 12 + 34 - 5 ;"
# assert_fail "1+3++;"
# #assert_fail "1 + foo + 5;"
# assert 47 '5+6*7;'
# assert 15 '5*(9-6);'
# assert 4 '(3+5)/2;'
# assert 10 '-10+20;'

# assert 1 '2 == 2;'
# assert 0 '2 == 3;'
# assert 0 '2 != 2;'
# assert 1 '2 != 3;'

# assert 0 '3 <= 2;'
# assert 1 '2 <= 2;'
# assert 1 '2 <= 3;'
# assert 0 '3 < 2;'
# assert 0 '2 < 2;'
# assert 1 '2 < 3;'

# assert 0 '2 >= 3;'
# assert 1 '2 >= 2;'
# assert 1 '3 >= 2;'
# assert 0 '2 > 3;'
# assert 0 '2 > 2;'
# assert 1 '3 > 2;'

assert 1 'main(){a = 1;}'
# assert 1 'a = 1; return a;'
# assert 5 'foo = 5;'
# assert 5 'foo = 5; return foo;'

# assert 5 'return 5;'
# assert 8 'return 8;'
# assert 5 'return 2 + 3;'

# assert 2 'if (1) return 2; return 1;'
# assert 3 'if (0) return 1; else return 3; return 2;'
# assert 1 'if (2>1) return 1; else return 3; return 2;'
# assert 5 'if (2<1) return 1; else return 2 + 3; return 2;'

# assert 9 'a = 0; while(a<9) a = a + 1; return a;'
# assert 18 'b = 0; for (a = 0; a<9; a = a + 1) b = b + 2; return b;'

# assert 18 'a = 0; while(a<18) { a = a + 1; a = a + 1; } return a;'
# assert 36 'b = 0; for (a = 0; a<9; a = a + 1) { b = b + 2; b = b + 2; } return b;'
# assert 5  'a = 2<1; if (a) { return 1; } else { return 2 + 3; } return 2;'

assert_stdout 2 'main(){foo();}'
assert_stdout 5 'main(){bar(2,3);}'
assert_stdout 41 'main(){buzz(2,3,5,7,11,13);}'

assert 2 'fuzz(){return 2;} main(){return fuzz();}'
assert 2 'fuzz(a){return a;} main(){return fuzz(2);}'
assert 5 'fuzz(a,b){return a+b;} main(){return fuzz(2,3);}'
assert 41 'fuzz(a,b,c,e,f,g){return a+b+c+e+f+g;} main(){return fuzz(2,3,5,7,11,13);}'

echo "OK;"

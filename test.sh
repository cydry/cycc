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

assert 0 '0;'
assert 42 '42;'
assert 41 " 12 + 34 - 5 ;"
assert_fail "1+3++;"
#assert_fail "1 + foo + 5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'

assert 1 '2 == 2;'
assert 0 '2 == 3;'
assert 0 '2 != 2;'
assert 1 '2 != 3;'

assert 0 '3 <= 2;'
assert 1 '2 <= 2;'
assert 1 '2 <= 3;'
assert 0 '3 < 2;'
assert 0 '2 < 2;'
assert 1 '2 < 3;'

assert 0 '2 >= 3;'
assert 1 '2 >= 2;'
assert 1 '3 >= 2;'
assert 0 '2 > 3;'
assert 0 '2 > 2;'
assert 1 '3 > 2;'

assert 1 'a = 1;'
assert 1 'foo = 1;'

assert 5 'return 5;'
assert 8 'return 8;'
assert 5 'return 2 + 3;'

assert 2 'if (1) return 2; return 1;'
assert 3 'if (0) return 1; else return 3; return 2;'
assert 1 'if (2>1) return 1; else return 3; return 2;'
assert 5 'if (2<1) return 1; else return 2 + 3; return 2;'

echo "OK;"

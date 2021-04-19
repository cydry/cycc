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

assert 0 'main(){0;}'
assert 42 'main(){42;}'
assert 41 "main(){ 12 + 34 - 5 ;}"
assert_fail "main(){1+3++;}"
#assert_fail "1 + foo + 5;"
assert 47 'main(){5+6*7;}'
assert 15 'main(){5*(9-6);}'
assert 4 'main(){(3+5)/2;}'
assert 10 'main(){-10+20;}'

assert 1 'main(){2 == 2;}'
assert 0 'main(){2 == 3;}'
assert 0 'main(){2 != 2;}'
assert 1 'main(){2 != 3;}'

assert 0 'main(){3 <= 2;}'
assert 1 'main(){2 <= 2;}'
assert 1 'main(){2 <= 3;}'
assert 0 'main(){3 < 2;}'
assert 0 'main(){2 < 2;}'
assert 1 'main(){2 < 3;}'

assert 0 'main(){2 >= 3;}'
assert 1 'main(){2 >= 2;}'
assert 1 'main(){3 >= 2;}'
assert 0 'main(){2 > 3;}'
assert 0 'main(){2 > 2;}'
assert 1 'main(){3 > 2;}'

assert 1 'main(){a = 1;}'
assert 1 'main(){a = 1; return a;}'
assert 5 'main(){foo = 5;}'
assert 5 'main(){foo = 5; return foo;}'

assert 5 'main(){return 5;}'
assert 8 'main(){return 8;}'
assert 5 'main(){return 2 + 3;}'

assert 2 'main(){if (1) return 2; return 1;}'
assert 3 'main(){if (0) return 1; else return 3; return 2;}'
assert 1 'main(){if (2>1) return 1; else return 3; return 2;}'
assert 5 'main(){if (2<1) return 1; else return 2 + 3; return 2;}'

assert 9 'main(){a = 0; while(a<9) a = a + 1; return a;}'
assert 18 'main(){b = 0; for (a = 0; a<9; a = a + 1) b = b + 2; return b;}'

assert 18 'main(){a = 0; while(a<18) { a = a + 1; a = a + 1; } return a;}'
assert 36 'main(){b = 0; for (a = 0; a<9; a = a + 1) { b = b + 2; b = b + 2; } return b;}'
assert 5  'main(){a = 2<1; if (a) { return 1; } else { return 2 + 3; } return 2;}'

assert_stdout 2 'main(){foo();}'
assert_stdout 5 'main(){bar(2,3);}'
assert_stdout 41 'main(){buzz(2,3,5,7,11,13);}'

assert 2 'fuzz(){return 2;} main(){return fuzz();}'
assert 2 'fuzz(a){return a;} main(){return fuzz(2);}'
assert 5 'fuzz(a,b){return a+b;} main(){return fuzz(2,3);}'
assert 41 'fuzz(a,b,c,e,f,g){return a+b+c+e+f+g;} main(){return fuzz(2,3,5,7,11,13);}'

assert 1   'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(1); }'
assert 1   'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(2); }'
assert 2   'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(3); }'
assert 3   'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(4); }'
assert 5   'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(5); }'
assert 8   'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(6); }'
assert 13  'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(7); }'
assert 233 'fib(n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} main(){return fib(13);}'

assert 1 'main(){a; &a;}'
echo "OK;"

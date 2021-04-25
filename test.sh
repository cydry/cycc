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

assert_with() {
  expected="$1"
  input="$2"

  ./main "$input" > tmp.s
  cc -c foo.c
  cc -o tmp tmp.s foo.o
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


assert 0 'int main(int a){0;}'
assert 0 'int main(){0;}'
assert 42 'int main(){42;}'
assert 41 "int main(){ 12 + 34 - 5 ;}"
assert_fail "int(){1+3++;}"
#assert_fail "1 + foo + 5;"
assert 47 'int main(){5+6*7;}'
assert 15 'int main(){5*(9-6);}'
assert 4 'int main(){(3+5)/2;}'
assert 10 'int main(){-10+20;}'

assert 1 'int main(){2 == 2;}'
assert 0 'int main(){2 == 3;}'
assert 0 'int main(){2 != 2;}'
assert 1 'int main(){2 != 3;}'

assert 0 'int main(){3 <= 2;}'
assert 1 'int main(){2 <= 2;}'
assert 1 'int main(){2 <= 3;}'
assert 0 'int main(){3 < 2;}'
assert 0 'int main(){2 < 2;}'
assert 1 'int main(){2 < 3;}'

assert 0 'int main(){2 >= 3;}'
assert 1 'int main(){2 >= 2;}'
assert 1 'int main(){3 >= 2;}'
assert 0 'int main(){2 > 3;}'
assert 0 'int main(){2 > 2;}'
assert 1 'int main(){3 > 2;}'

assert 1 'int main(){int a; a = 1;}'
assert 1 'int main(){int a; a = 1; return a;}'
assert 5 'int main(){int foo; foo = 5;}'
assert 5 'int main(){int foo; foo = 5; return foo;}'

assert 5 'int main(){return 5;}'
assert 8 'int main(){return 8;}'
assert 5 'int main(){return 2 + 3;}'

assert 2 'int main(){if (1) return 2; return 1;}'
assert 3 'int main(){if (0) return 1; else return 3; return 2;}'
assert 1 'int main(){if (2>1) return 1; else return 3; return 2;}'
assert 5 'int main(){if (2<1) return 1; else return 2 + 3; return 2;}'

assert 9 'int main(){int a; a = 0; while(a<9) a = a + 1; return a;}'
assert 18 'int main(){int a; int b; b = 0; for (a = 0; a<9; a = a + 1) b = b + 2; return b;}'

assert 18 'int main(){int a; a = 0; while(a<18) { a = a + 1; a = a + 1; } return a;}'
assert 36 'int main(){int a; int b; b = 0; for (a = 0; a<9; a = a + 1) { b = b + 2; b = b + 2; } return b;}'
assert 5  'int main(){int a; a = 2<1; if (a) { return 1; } else { return 2 + 3; } return 2;}'

assert_stdout 2 'int main(){foo();}'
assert_stdout 5 'int main(){bar(2,3);}'
assert_stdout 41 'int main(){buzz(2,3,5,7,11,13);}'

assert 2 'int fuzz(){return 2;} int main(){return fuzz();}'
assert 2 'int fuzz(int a){return a;} int main(){return fuzz(2);}'
assert 5 'int fuzz(int a,int b){return a+b;} int main(){return fuzz(2,3);}'
assert 41 'int fuzz(int a, int b, int c, int e, int f, int g){return a+b+c+e+f+g;} int main(){return fuzz(2,3,5,7,11,13);}'

assert 1   'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(1); }'
assert 1   'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(2); }'
assert 2   'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(3); }'
assert 3   'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(4); }'
assert 5   'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(5); }'
assert 8   'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(6); }'
assert 13  'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(7); }'
assert 233 'int fib(int n){if(n==1) return 1; if(n==2) return 1; return fib(n-2) + fib(n-1);} int main(){return fib(13);}'

assert 2 'int main(){int a; int* b; a=2; b=&a; *b;}'
assert 2 'int main(){int a; int* b; b=&a; a=2; *b;}'
assert 2 'int main(){int a; int* b; a=2; b=&a; *b;}'
assert 2 'int main(){int a; int* b; a=2; b=&a; return *b;}'
assert 3 'int main(){int a; int* b; a=2; b=&a; a=a+1; return *b;}'
assert 3 'int main(){int a; int* b; a=2; b=&a; *b=*b+1; return a;}'

assert 1 'int foo(int a, int b, int c, int d, int e){a;} int bar(int a){a;} int main(){int b; b=bar(1); b;}'

assert 1 'int foo(int *a){a;} int main(){1;}'
assert 1 'int foo(int **a){a;} int main(){1;}'
assert 2 'int foo(int ********a){a;} int main(){2;}'

assert 3 'int foo(int *a){*a;} int main(){int a; int p; p=&a; a=3; foo(p);}'
assert 5 'int foo(int **a){int *p; p=*a; *p;} int main(){int a; int *p; int **pp; pp=&p; p=&a; a=5; foo(pp);}'
assert 7 'int foo(int ***a){***a;} int main(){int a; int *p; int **pp; int ***ppp; ppp=&pp; pp=&p; p=&a; a=7; foo(ppp);}'
assert 7 'int foo(int ***a){***a;} int main(){int a; int *p; int **pp; int ***ppp; a=7; ppp=&pp; pp=&p; p=&a; foo(ppp);}'

assert_with 1 'int main(){int *p; int **pp; pp=&p; alloc4(pp, 1, 2, 4, 8); *p;}'
assert_with 1 'int main(){int *p; alloc4(&p, 1, 2, 4, 8); p=p+0; *p;}'
assert_with 2 'int main(){int *p; alloc4(&p, 1, 2, 4, 8); p=p+1; *p;}'
assert_with 4 'int main(){int *p; alloc4(&p, 1, 2, 4, 8); p=p+2; *p;}'
assert_with 8 'int main(){int *p; alloc4(&p, 1, 2, 4, 8); p=p+3; *p;}'

assert 4 'int main(){ sizeof(int); }'
assert 8 'int main(){ sizeof(int*); }'
assert 8 'int main(){ sizeof(int**); }'
assert 8 'int main(){ sizeof(int********); }'

assert 4 'int main(){int x;  sizeof(x);}'
assert 4 'int main(){int x;  sizeof(x+3);}'
assert 8 'int main(){int* x; sizeof(x);}'
assert 8 'int main(){int* x; sizeof(x+3);}'
assert 4 'int main(){int* x; sizeof(*x);}'
assert 4 'int main(){sizeof(1);}'
assert 4 'int main(){sizeof(sizeof(1));}'

assert 40 'int main(){int  a[10]; sizeof(a);}'
assert 80 'int main(){int* a[10]; sizeof(a);}'
assert 4  'int main(){int  a[10]; sizeof(*a);}'
assert 8  'int main(){int* a[10]; sizeof(*a);}'
assert 8  'int main(){int  a[10]; sizeof(a+1);}'
assert 8  'int main(){int* a[10]; sizeof(a+1);}'
assert 4  'int main(){int  a[10]; sizeof(*(a+1));}'
assert 8  'int main(){int* a[10]; sizeof(*(a+1));}'

assert 2 'int main(){int a[10]; int b; *a=2; b=9; *a;}'
assert 2 'int main(){int a[10]; int b; int* p; a=2; p=&a; p=p+1; *p=3; b=9; *a;}'
assert 3 'int main(){int a[10]; int b; int* p; a=2; p=&a; p=p+1; *p=3; b=9; *p;}'
assert 3 'int main(){int a[10]; int b; int* p; a=2; p=&a; p=p+1; *p=3; p=p+1; *p=5; p=p-1; b=9; *p;}'

assert 2 'int main(){int a[10]; int b; int* p; p=a;   a=2; b=9; *p;}'
assert 2 'int main(){int a[10]; int b; int* p; p=a+1; a=2; p=p-1; b=9; *p;}'
assert 3 'int main(){int a[10]; int b; int* p; p=a+2; a=3; p=p-2; b=9; *p;}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+9; a=5; p=p-9; b=9; *p;}'

assert 2 'int main(){int a[10]; int b; int* p; p=a;   *p=2; b=9; *a;}'
assert 3 'int main(){int a[10]; int b; int* p; p=a+1; *p=3; b=9; *(a+1);}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+2; *p=5; b=9; *(a+2);}'
assert 7 'int main(){int a[10]; int b; int* p; p=a+9; *p=7; b=9; *(a+9);}'

assert 3 'int main(){int a[10]; int b; int* p; p=a+1; *p=3; b=9; *(1+a);}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+2; *p=5; b=9; *(2+a);}'
assert 7 'int main(){int a[10]; int b; int* p; p=a+9; *p=7; b=9; *(9+a);}'

assert 2 'int main(){int a[10]; int b; int* p; p=a;   *p=2; b=9; *(a-1+1);}'
assert 3 'int main(){int a[10]; int b; int* p; p=a+1; *p=3; b=9; *(a-2+3);}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+2; *p=5; b=9; *(a-3+5);}'
assert 7 'int main(){int a[10]; int b; int* p; p=a+9; *p=7; b=9; *(a-9+18);}'

assert 2 'int main(){int a[10]; int b; int* p; p=a;   *p=2; b=9; *(a+1-1);}'
assert 3 'int main(){int a[10]; int b; int* p; p=a+1; *p=3; b=9; *(a+3-2);}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+2; *p=5; b=9; *(a+5-3);}'
assert 7 'int main(){int a[10]; int b; int* p; p=a+9; *p=7; b=9; *(a+18-9);}'

assert 2 'int main(){int a[10]; int b; int* p; p=a;   *p=2; b=9; a[0];}'
assert 3 'int main(){int a[10]; int b; int* p; p=a+1; *p=3; b=9; a[1];}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+2; *p=5; b=9; a[2];}'
assert 7 'int main(){int a[10]; int b; int* p; p=a+9; *p=7; b=9; a[9];}'

assert 2 'int main(){int a[10]; int b; int* p; p=a;   *p=2; b=9; 0[a];}'
assert 3 'int main(){int a[10]; int b; int* p; p=a+1; *p=3; b=9; 1[a];}'
assert 5 'int main(){int a[10]; int b; int* p; p=a+2; *p=5; b=9; 2[a];}'
assert 7 'int main(){int a[10]; int b; int* p; p=a+9; *p=7; b=9; 9[a];}'

echo "OK;"

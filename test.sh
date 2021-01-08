#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./cycc "$input" > tmp.s
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
  ./cycc "$input" > tmp.s
  if [ $? -ne 1 ]; then
      echo "$input should cause an error."
      exit 1
  fi
}

assert 0 0
assert 42 42
assert 41 " 12 + 34 - 5 "
assert_fail "1+3++"
assert_fail "1 + foo + 5"
echo "OK"

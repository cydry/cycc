assert() {
  expected="$1"
  input="$2"

  ./main -e "$input" > tmp.s
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

  ./main -e "$input" > tmp.s
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
  ./main -e "$input" > tmp.s
  if [ $? -ne 1 ]; then
      echo "$input should cause an error."
      exit 1
  fi
}

assert_ffail() {
  file=$1
  ./main "$file"
  if [ $? -ne 1 ]; then
      echo "$input should cause an error."
      exit 1
  fi
}

assert_stdout() {
  expected="$1"
  input="$2"

  ./main -e "$input" > tmp.s
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

assert_file() {
  expected="$1"
  file="$2"

  ./main "$file" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$file => $actual"
  else
    echo "$file => $expected expected, but got $actual"
    exit 1
  fi
}

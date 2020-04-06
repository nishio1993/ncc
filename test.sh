#!/bin/bash
try() {
	expected="$1"
	input="$2"

	./ncc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

try 21 "5+20-4;"
try 41 "12 + 34 - 5; "
try 37 "5 * 6 + 7;"
try 12 "12 / 4 + 9;"
try 47 "5+ 6 *7;"
try 15 "5 * ( 9- 6);"
try 4 "(3 +5 ) /2;" 
try 2 "- 3 +5;"
try 6 "+ 2 + 4;"
try 0 "0 == 1;"
try 1 "1 == 1;"
try 1 "1 != 0;"
try 0 "1 != 1;"
try 1 "0 < 1;"
try 0 "1 < 1;"
try 0 "2 < 1;"
try 1 "0 <= 1;"
try 1 "1 <= 1;"
try 0 "2 <= 1;"
try 0 "0 > 1;"
try 0 "1 > 1;"
try 1 "2 > 1;"
try 0 "0 >= 1;"
try 1 "1 >= 1;"
try 1 "2 >= 1;"
try 3 "a = 3;a;"
try 14 "a = 2; b = (3+4); a*b;"
try 5 "a = 5;b = a;b;"
try 24 "ab3 = 3 * 4; c6d = ab3 * 2; e_f = c6d;"
try 15 "return_a = 16 / 4; return return_a + 11;"

echo OK

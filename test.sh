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

<< COMMENTOUT
try 41 "12 + 34 - 5; "
try 37 "7 + 5 * 6;"
try 12 "9 + 12 / 4;"
try 15 "5 * ( 9- 6);"
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
try 24 "ab3 = 3 * 4; c6d = ab3 * 2; e_f = c6d;"
try 15 "return_a = 16 / 4; return return_a + 11;"
try 47 "a = 23;if (11 == 11)a = 47; return a;"
try 2 "a = 3;if (11 == 11)a = 2;b = 4;if (11 == 13)a = 4;return a;"
try 45 "sum = 0; for (i = 0;i < 10;i = i + 1)sum = sum + i;return sum;"
try 10 "sum = 0; while(sum < 10)sum = sum + 1;return sum;"
try 50 "
ab = 10;
if (ab == 10) {
	c = 5;
	d = 10;
	ab = c * d;
}
return ab;
"
try 45 "
sum = 0;
for (i = 0; i < 10; i = i + 1) {
	sum = sum + i;
}
return sum;
"
try 45 "
sum = 0;
i = 0;
while(i < 10) {
	sum = sum + i;
	i = i + 1;
}
return sum;
"
COMMENTOUT
try 8 "
main() {
	ab = 3 * (2 + 4);
	cd = 12 / (9 - 3);
	if (ab == 18) {
		for (i = 0; i < 10; i = i + 1) {
			ab = ab + 2;
			cd = cd + 3;
		}
	}

	while (cd > 11) {
		cd = cd - 4;
	}

	return cd;
}"

echo OK
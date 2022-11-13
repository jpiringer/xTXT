#  Documentation

START = SYMBOL1 | SYMBOL2;
SYMBOL1 = abc | "abc";
SYMBOL2 = SYMBOL1 | START | "a" + START;

A{permutation} = a | b | c;
A{random} = a | b | c;
A{order} = a | b | c;
A{X==2} = a | b | c;


START = {COUNTER = 1} COUNT COUNT COUNT COUNT;
COUNT = COUNTER {COUNTER++};

| oder
+ concat (ohne whitespace)
\n, \t, \r, \\ escape

choice functions:
permutation: all are selected once randomly until all have been selected then †he cycle is repeated
random: standard
order: one after the other


operatoren for variables: ++, --, =, ==, <=, >=, ~=, +=, -=, *=, /=



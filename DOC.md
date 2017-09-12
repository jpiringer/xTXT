#  Documentation

START = SYMBOL1 | SYMBOL2;
SYMBOL1 = abc | "abc";
SYMBOL2 = SYMBOL1 | START | "a" @ START;

| oder
@ concat (ohne whitespace)
\n, \t, \r, \\ escape


#  version 1.2.0

## program mode
can be programmed in lua
with simple instructions to speak and show text
with grammar and LSystem
onKey

#  version 1.0.1

## markov
markov prefix length can now be 0
that will generate a string of letters with the occurance probability of the source text but without chaining the probabilities

## L-system
addded escape sequences for L-systems.
now can write rules like:
\s = space;
which means that spaces are substituted with the string "space".
possible escape sequences at the moment are: \s for space, \n for newline, \\ for \





//
//  Examples.cpp
//  txtgen - App
//
//  Created by joerg piringer on 05.10.17.
//

#include "Examples.hpp"

std::vector<std::pair<std::string,std::wstring>> grammarExamples() {
    return {
        
{"tutorial 1",
LR"(# tutorial 1
START = begin; # a START symbol is always needed
# don't forget the semicolon ; at the end of each line
)"
},
{"tutorial 2",
LR"(# tutorial 2
START = SECOND; # a START symbol is always needed
SECOND = all words written only in capital letters (and numbers) are substitued by their definition;
)"
},
{"tutorial 3",
LR"(# tutorial 3
START = EITHER | OR; # symbols separated by | are chosen randomly, try it a couple of times
EITHER = this;
OR = that;
)"
},
{"tutorial 4",
LR"(# tutorial 4
START = EITHER OR; # symbols separated by a space are joined
EITHER = this;
OR = that;
)"
},
{"tutorial 5",
LR"(# tutorial 5
START = FIRST+SECOND; # symbols separated by + are joined without a space
FIRST = ver;
SECOND = bindung;
)"
},
{"tutorial 6",
LR"(# tutorial 6
START = FIRST+"SECOND"; # characters in quotes " are not seen as symbols
FIRST = ver;
SECOND = bindung;
)"
},
{"tutorial 7",
LR"(# tutorial 7
START = first line\nsecond line; # to separate lines use the sequence \n (btw: n stands for newline)
)"
},
{"tutorial 8",
LR"(# tutorial 8
START = A A A A A A;
A = but | though| and | too| B;
B = i | you | he | she | START; # you can use symbols in symbol definitions
)"
},
{"tutorial 9",
LR"(# tutorial 9
START = infinite START; # but don't do this!!!
)"
},
{"counter",
LR"(# counter
START = {COUNTER = 0 END = 99} C;
C {COUNTER <= END} = {COUNTER++} COUNTER C;
C {COUNTER > END} = ende;
)"
},
{"example1",
LR"(START = A B C C C C C C C C;
A {permutate} = abc | def {COUNT = 1};
B {random} = A | B;
C {order} = eins | zwei | drei | vier;
)"
}
    };
}

std::vector<std::pair<std::string,std::wstring>> LSystemExamples() {
    return {
        {"alge",
            L"# algenwachstum\na = ab;\nb = a;"},
        
{"b-sprache",
LR"(# b-sprache
a = aba;
e = ebe;
i = ibi;
o = obo;
u = ubu;
y = yby;
ä = äbä;
ö = öbö;
ü = übü;
    )"
},
        {"buchstabieralphabet",
LR"(# österreichisches buchstabieralphabet
a = anton ;
b = berta ;
c = cäsar ;
d = dora ;
e = emil ;
f = friedrich ;
g = gustav ;
h = heinrich ;
i = ida ;
j = julius ;
k = konrad ;
l = ludwig ;
m = martha ;
n = nordpol ;
o = otto ;
p = paula ;
q = quelle ;
r = richard ;
s = samuel ;
t = theodor ;
u = ulrich ;
v = viktor ;
w = wilhelm ;
x = xaver ;
y = ypsilon ;
z = zeppelin ;
ä = ärger ;
ö = ökonom ;
ü = übermut ;
ß = scharfes s ;)"
        }
    };
}

std::vector<std::pair<std::string,std::wstring>> markovExamples() {
    return {
        {"alphabet", L"abcdefghijklmnopqrstuvwxyz"},
        {"genesis",
        L"Im Anfang war das Wort, und das Wort war bei Gott, und das Wort war Gott. Im Anfang war es bei Gott. Alles ist durch das Wort geworden und ohne das Wort wurde nichts, was geworden ist. In ihm war das Leben und das Leben war das Licht der Menschen. Und das Licht leuchtet in der Finsternis und die Finsternis hat es nicht erfasst. Es trat ein Mensch auf, der von Gott gesandt war; sein Name war Johannes. Er kam als Zeuge, um Zeugnis abzulegen für das Licht, damit alle durch ihn zum Glauben kommen. Er war nicht selbst das Licht, er sollte nur Zeugnis ablegen für das Licht. Das wahre Licht, das jeden Menschen erleuchtet, kam in die Welt. Er war in der Welt und die Welt ist durch ihn geworden, aber die Welt erkannte ihn nicht. Er kam in sein Eigentum, aber die Seinen nahmen ihn nicht auf. Allen aber, die ihn aufnahmen, gab er Macht, Kinder Gottes zu werden, allen, die an seinen Namen glauben, die nicht aus dem Blut, nicht aus dem Willen des Fleisches, nicht aus dem Willen des Mannes, sondern aus Gott geboren sind. Und das Wort ist Fleisch geworden und hat unter uns gewohnt und wir haben seine Herrlichkeit gesehen, die Herrlichkeit des einzigen Sohnes vom Vater, voll Gnade und Wahrheit. Johannes legte Zeugnis für ihn ab und rief: Dieser war es, über den ich gesagt habe: Er, der nach mir kommt, ist mir voraus, weil er vor mir war. Aus seiner Fülle haben wir alle empfangen, Gnade über Gnade. Denn das Gesetz wurde durch Mose gegeben, die Gnade und die Wahrheit kamen durch Jesus Christus. Niemand hat Gott je gesehen. Der Einzige, der Gott ist und am Herzen des Vaters ruht, er hat Kunde gebracht."
        }
        };
}

std::vector<std::pair<std::string,std::wstring>> programExamples() {
    return {
        {"hello, world!",
        LR"(print "hello, world!")"}
    };
}

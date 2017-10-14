//
//  Examples.cpp
//  txtgen - App
//
//  Created by joerg piringer on 05.10.17.
//

#include "Examples.hpp"

std::vector<std::pair<std::string,std::string>> grammarExamples() {
    return {
{"counter",
R"(START = {COUNTER = 0 END = 99} C;
C {COUNTER <= END} = {COUNTER++} COUNTER C;
C {COUNTER > END} = ende;
)"
},
{"example1",
R"(START = A B C C C C C C C C;
A {permutate} = abc | def {COUNT = 1};
B {random} = A | B;
C {order} = eins | zwei | drei | vier;
)"
}
    };
}

std::vector<std::pair<std::string,std::string>> LSystemExamples() {
    return {
{"b-sprache",
R"(a = aba;
e = ebe;
i = ibi;
o = obo;
u = ubu;
y = yby;
ä = äbä;
ö = öbö;
ü = übü;
    )"
}
    };
}

std::vector<std::pair<std::string,std::string>> markovExamples() {
    return {
        {"genesis",
        "Im Anfang war das Wort, und das Wort war bei Gott, und das Wort war Gott. Im Anfang war es bei Gott. Alles ist durch das Wort geworden und ohne das Wort wurde nichts, was geworden ist. In ihm war das Leben und das Leben war das Licht der Menschen. Und das Licht leuchtet in der Finsternis und die Finsternis hat es nicht erfasst. Es trat ein Mensch auf, der von Gott gesandt war; sein Name war Johannes. Er kam als Zeuge, um Zeugnis abzulegen für das Licht, damit alle durch ihn zum Glauben kommen. Er war nicht selbst das Licht, er sollte nur Zeugnis ablegen für das Licht. Das wahre Licht, das jeden Menschen erleuchtet, kam in die Welt. Er war in der Welt und die Welt ist durch ihn geworden, aber die Welt erkannte ihn nicht. Er kam in sein Eigentum, aber die Seinen nahmen ihn nicht auf. Allen aber, die ihn aufnahmen, gab er Macht, Kinder Gottes zu werden, allen, die an seinen Namen glauben, die nicht aus dem Blut, nicht aus dem Willen des Fleisches, nicht aus dem Willen des Mannes, sondern aus Gott geboren sind. Und das Wort ist Fleisch geworden und hat unter uns gewohnt und wir haben seine Herrlichkeit gesehen, die Herrlichkeit des einzigen Sohnes vom Vater, voll Gnade und Wahrheit. Johannes legte Zeugnis für ihn ab und rief: Dieser war es, über den ich gesagt habe: Er, der nach mir kommt, ist mir voraus, weil er vor mir war. Aus seiner Fülle haben wir alle empfangen, Gnade über Gnade. Denn das Gesetz wurde durch Mose gegeben, die Gnade und die Wahrheit kamen durch Jesus Christus. Niemand hat Gott je gesehen. Der Einzige, der Gott ist und am Herzen des Vaters ruht, er hat Kunde gebracht."
        }
        };
}

std::vector<std::pair<std::string,std::string>> programExamples() {
    return {
        {"hello, world!",
        R"(print "hello, world!")"}
    };
}

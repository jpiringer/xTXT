//
//  Examples.cpp
//  txtgen - App
//
//  Created by joerg piringer on 05.10.17.
//

#include "Examples.hpp"

std::vector<std::string> grammarExamples() {
    return {
R"(START = A B;
A = abc | def;
B = A | B;
)"        
    };
}

std::vector<std::string> LSystemExamples() {
    return {
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
        
    };
}

std::vector<std::string> markovExamples() {
    return {};
}

std::vector<std::string> programExamples() {
    return {};
}

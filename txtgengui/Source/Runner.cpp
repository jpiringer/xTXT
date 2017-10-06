//
//  Runner.cpp
//  txtgengui - App
//
//  Created by joerg piringer on 02.10.17.
//

#include "Runner.hpp"

#include "Parser.hpp"
#include "LSystem.h"
#include "Markov.hpp"
#include "utils.hpp"

#include "Examples.hpp"

jp::Runner::Runner(enum RunnerType rtype)
: runnerType(rtype) {
    
}

std::string jp::Runner::runAutomate() {
    std::wstring expression = fromUTF8(code);
    jp::Parser parser(expression);
    
    parser.parse();
    std::wstring result = parser.generate();
    
    if (parser.getErrorCount() > 0) {
        errorsHappened = true;
        result = parser.getErrors();
    }
    
    return toUTF8(result);
}

std::string jp::Runner::runLSystem() {
    auto lsystem = parseLSystem(fromUTF8(code));
    
    if (lsystem->getErrorCount() > 0) {
        errorsHappened = true;
        return toUTF8(lsystem->getErrors());
    }
    else {
        lsystem->setState(fromUTF8(text));
        
        lsystem->addRule('a', L"aba");
        lsystem->iterate();
        
        return toUTF8(lsystem->getState());
    }
}

std::string jp::Runner::runMarkov() {
    auto source = fromUTF8(code);
    MarkovTable markov(3);
    markov.addString(source);
    
    return toUTF8(markov.getString(source.length()));
}

std::string jp::Runner::runProgram() {
    auto source = fromUTF8(code);
    
    return toUTF8(source);
}

std::string jp::Runner::run() {
    switch (runnerType) {
        case Grammar:
            return runAutomate();
        case LSystem:
            return runLSystem();
        case Markov:
            return runMarkov();
        case Program:
            return runProgram();
        default:
            break;
    }
    return "Unknown Runner Type";
}

std::vector<std::string> jp::Runner::getExamples() {
    switch (runnerType) {
        case Grammar:
            return grammarExamples();
        case LSystem:
            return LSystemExamples();
        case Markov:
            return markovExamples();
        case Program:
            return programExamples();
        default:
            break;
    }
    return {};
}

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
#include "Program.hpp"
#include "NamShub.hpp"

#include "utils.hpp"

#include "Examples.hpp"

#include <iostream>

jp::Runner::Runner(enum RunnerType rtype)
: runnerType(rtype) {
    
}

double jp::Runner::getParameter(const std::string &parName) {
    if (parameters.find(parName) == parameters.end()) {
        return 0;
    }
    return parameters[parName];
}

std::string jp::Runner::getStringParameter(const std::string &parName) {
    if (stringParameters.find(parName) == stringParameters.end()) {
        return "";
    }
    return stringParameters[parName];
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
    MarkovTable markov(getParameter("prefixLen"));
    markov.addString(source);
    
    return toUTF8(markov.getString(getParameter("textLen")));
}

std::string jp::Runner::runProgram() {
    LuaProgram program;
    
    program.setCode(code);
    
    auto result = program.execute();
    
    if (program.getErrorCount() > 0) {
        errorsHappened = true;
        result += "\n";
        result += program.getErrors();
    }
    
    return result;
}

std::string jp::Runner::runNamShub() {
    NamShubExecutor namShub;
    
    return toUTF8(namShub.executeCommand(getStringParameter("command"), fromUTF8(text)));
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
        case NamShub:
            return runNamShub();
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

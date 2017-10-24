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

#include "TextTurtleGraphics.hpp"

#include <iostream>

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

// ====================================================================================
// AutomateRunner
// ====================================================================================

std::string jp::AutomateRunner::run() {
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

std::vector<std::pair<std::string,std::wstring>> jp::AutomateRunner::getExamples() {
    return grammarExamples();
}

DrawFunction jp::AutomateRunner::getDrawFunction() {
    return [](Graphics &g, int width, int height, const std::wstring &, double time) {
        
    };
}

bool jp::AutomateRunner::isAnimated() {
    return true;
}

// ====================================================================================
// LSystemRunner
// ====================================================================================

std::string jp::LSystemRunner::run() {
    auto lsystem = parseLSystem(fromUTF8(code));
    
    if (lsystem->getErrorCount() > 0) {
        errorsHappened = true;
        return toUTF8(lsystem->getErrors());
    }
    else {
        lsystem->setState(fromUTF8(text));
        
        lsystem->iterate();
        
        return toUTF8(lsystem->getState());
    }
}

std::vector<std::pair<std::string,std::wstring>> jp::LSystemRunner::getExamples() {
    return LSystemExamples();
}

DrawFunction jp::LSystemRunner::getDrawFunction() {
    return TextTurtleGraphics::getDrawFunction(this);
}

bool jp::LSystemRunner::isAnimated() {
    return false;
}

// ====================================================================================
// MarkovRunner
// ====================================================================================

std::string jp::MarkovRunner::run() {
    auto source = fromUTF8(code);
    MarkovTable markov(getParameter("prefixLen"));
    markov.addString(source);
    
    return toUTF8(markov.getString(getParameter("textLen")));
}

std::vector<std::pair<std::string,std::wstring>> jp::MarkovRunner::getExamples() {
    return markovExamples();
}

DrawFunction jp::MarkovRunner::getDrawFunction() {
    return [](Graphics &g, int width, int height, const std::wstring &, double time) {
        
    };
}

bool jp::MarkovRunner::isAnimated() {
    return true;
}

// ====================================================================================
// ProgramRunner
// ====================================================================================

std::string jp::ProgramRunner::run() {
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

std::vector<std::pair<std::string,std::wstring>> jp::ProgramRunner::getExamples() {
    return programExamples();
}

DrawFunction jp::ProgramRunner::getDrawFunction() {
    return [](Graphics &g, int width, int height, const std::wstring &, double time) {
        
    };
}

bool jp::ProgramRunner::isAnimated() {
    return true;
}

// ====================================================================================
// NamShubRunner
// ====================================================================================

std::string jp::NamShubRunner::run() {
    NamShubExecutor namShub;
    
    return toUTF8(namShub.executeCommand(getStringParameter("command"), fromUTF8(text)));
}

std::vector<std::pair<std::string,std::wstring>> jp::NamShubRunner::getExamples() {
    return {};
}

DrawFunction jp::NamShubRunner::getDrawFunction() {
    return [](Graphics &g, int width, int height, const std::wstring &, double time) {
        
    };
}

bool jp::NamShubRunner::isAnimated() {
    return true;
}

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

double jp::Runner::getParameter(const std::string &parName, float defValue) {
    if (parameters.find(parName) == parameters.end()) {
        return defValue;
    }
    return parameters[parName];
}

std::string jp::Runner::getStringParameter(const std::string &parName) {
    if (stringParameters.find(parName) == stringParameters.end()) {
        return "";
    }
    return stringParameters[parName];
}

void jp::Runner::changeOutput(const std::string &str) {
    
}

void jp::Runner::changeShowSize(float width, float height) {
    
}

// ====================================================================================
// AutomateRunner
// ====================================================================================

std::string jp::AutomateRunner::run(RunContext *runContext) {
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

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> jp::AutomateRunner::getExamples() {
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

jp::LSystemRunner::LSystemRunner() {
    textTurtleGraphics = std::make_shared<jp::TextTurtleGraphics>();
}

std::string jp::LSystemRunner::run(RunContext *runContext) {
    auto lsystem = LSystem::parseLSystem(fromUTF8(code));
    
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

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> jp::LSystemRunner::getExamples() {
    return LSystemExamples();
}

DrawFunction jp::LSystemRunner::getDrawFunction() {
    return [this](Graphics &g, int width, int height, const std::wstring &str, double time) {
		textTurtleGraphics->draw(this, g, width, height, str, time);
    };
}

bool jp::LSystemRunner::isAnimated() {
    return false;
}

void jp::LSystemRunner::exportFile(const std::string &fileName) {
    textTurtleGraphics->saveAsImage(this, fileName);
}

// ====================================================================================
// MarkovRunner
// ====================================================================================

std::string jp::MarkovRunner::run(RunContext *runContext) {
    auto source = fromUTF8(code);
    MarkovTable markov(getParameter("prefixLen"));
    markov.addString(source);
    
    return toUTF8(markov.getString(getParameter("textLen")));
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> jp::MarkovRunner::getExamples() {
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

std::string jp::ProgramRunner::run(RunContext *runContext) {
    if (program == nullptr) {
        program = std::make_shared<LuaProgram>();
    }
    program->setCode(code);
    
    auto result = program->execute(runContext);
    
    if (program->getErrorCount() > 0) {
        errorsHappened = true;
        result += "\n";
        result += program->getErrors();
    }
    
    return result;
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> jp::ProgramRunner::getExamples() {
    return programExamples();
}

DrawFunction jp::ProgramRunner::getDrawFunction() {
    return [](Graphics &g, int width, int height, const std::wstring &, double time) {
        TextWorld::sharedTextWorld().draw(g);
    };
}

bool jp::ProgramRunner::isAnimated() {
    return true;
}

void jp::ProgramRunner::exportFile(const std::string &fileName) {
    if (program == nullptr) {
        program = std::make_shared<LuaProgram>();
    }
    program->setCode(code);

    program->exportVideo(fileName);
}

void jp::ProgramRunner::keyPressed(int keyCode) {
    program->keyPressed(keyCode);
}

// ====================================================================================
// NamShubRunner
// ====================================================================================

std::string jp::NamShubRunner::run(RunContext *runContext) {
    NamShubExecutor namShub;
    
    return toUTF8(namShub.executeCommand(getStringParameter("command"), fromUTF8(text)));
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> jp::NamShubRunner::getExamples() {
    return {};
}

DrawFunction jp::NamShubRunner::getDrawFunction() {
    return [](Graphics &g, int width, int height, const std::wstring &, double time) {
        
    };
}

bool jp::NamShubRunner::isAnimated() {
    return true;
}

//
//  Examples.cpp
//  txtgen - App
//
//  Created by joerg piringer on 05.10.17.
//
#include "../JuceLibraryCode/JuceHeader.h"

#include "Examples.hpp"

#include <tuple>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>

#include "utils.hpp"
#include "Platform.hpp"

#if !TARGET_OS_MAC
#include "resource.h"
#endif

class Examples;

static std::shared_ptr<Examples> loadedLSystemExamples;
static std::shared_ptr<Examples> loadedGrammarExamples;
static std::shared_ptr<Examples> loadedMarkovExamples;
static std::shared_ptr<Examples> loadedProgramExamples;

class Examples {
    std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> contents;
    
    std::multimap<std::string, std::wstring> properties;
    std::wstring currentParsedPropertyContent;
    std::string currentParsedPropertyTitle;

protected:
    bool match(std::wstring propertyTitle, std::wstring &line) {
        if (line.find(propertyTitle+L":") == 0) {
            line = line.substr(propertyTitle.length()+1);
            return true;
        }
        return false;
    }
    
    void parseLine(std::wstring &line) {
        bool propertyComplete = false;
        bool exampleCompleted = false;
        
        //std::wcout << L"parsing: " << line << std::endl;
        if (match(L"TITLE", line)) {
            currentParsedPropertyContent = trim(line);
            currentParsedPropertyTitle = "title";
            propertyComplete = true;
        }
        else if (match(L"PAR", line)) {
            currentParsedPropertyContent = trim(line);
            currentParsedPropertyTitle = "parameter";
            propertyComplete = true;
        }
        else if (match(L"CODE", line)) {
            currentParsedPropertyContent = L"";
            currentParsedPropertyTitle = "code";
        }
        else if (line.find(L"$CODE") == 0) {
            if (currentParsedPropertyTitle == "code") {
                propertyComplete = true;
            }
            else {
                std::cout << "end tag mismatches property named \"" << currentParsedPropertyTitle << "\"" << std::endl;
            }
        }
        else if (line.find(L"$$") == 0) {
            exampleCompleted = true;
            if (!currentParsedPropertyTitle.empty()) { // if there's still an open property close it
                propertyComplete = true;
            }
        }
        else {
            currentParsedPropertyContent += line + L"\n";
        }
        
        if (propertyComplete) {
            //std::cout << "property " << currentParsedPropertyTitle << ": ";
            //std::wcout << currentParsedPropertyContent << std::endl;
            properties.emplace(currentParsedPropertyTitle, currentParsedPropertyContent);
            currentParsedPropertyContent = L"";
            currentParsedPropertyTitle = "";
        }
        if (exampleCompleted) {
            auto title = properties.find("title");
            auto code = properties.find("code");
            auto parameter = properties.equal_range("parameter");
            std::vector<std::wstring> parameters;
            
            for (auto p = parameter.first; p != parameter.second; ++p) {
                parameters.push_back(p->second);
            }

            contents.push_back(std::make_tuple(toUTF8(title->second), code->second, parameters));
            properties.clear();
        }
    }

    void parse(const std::wstring &str) {
        std::wistringstream stream(str);
        std::wstring line;
        while (std::getline(stream, line)) {
            parseLine(line);
        }
    }
    
    std::wstring loadFromFile(const std::string &filePath) {
        std::wifstream wif(filePath);
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        std::wstringstream wss;
        wss << wif.rdbuf();
        
        return wss.str();
    }
    
public:
    Examples(const std::string &fileName) {
        std::wstring str = loadFromFile(toDataPath(fileName));
        parse(str);
    }
    Examples(int _id) {
#if !TARGET_OS_MAC
        std::wstring str = loadResourceFileWithID(_id);
        parse(str);
#endif
    }
    
    decltype(contents) &getAll() {
        return contents;
    }
};

void initExamples() {
#if TARGET_OS_MAC
    loadedLSystemExamples = std::make_shared<Examples>("examples-lsystem.txt");
    loadedGrammarExamples = std::make_shared<Examples>("examples-grammar.txt");
    loadedMarkovExamples = std::make_shared<Examples>("examples-markov.txt");
    loadedProgramExamples = std::make_shared<Examples>("examples-program.txt");
#else
    loadedLSystemExamples = std::make_shared<Examples>(IDR_EXAMPLESLYSYSTEM);
    loadedGrammarExamples = std::make_shared<Examples>(IDR_EXAMPLESGRAMMAR);
    loadedMarkovExamples = std::make_shared<Examples>(IDR_EXAMPLESMARKOV);
    loadedProgramExamples = std::make_shared<Examples>(IDR_EXAMPLESPROGRAM);
#endif
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> grammarExamples() {
    return loadedGrammarExamples->getAll();
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> LSystemExamples() {
    return loadedLSystemExamples->getAll();
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> markovExamples() {
    return loadedMarkovExamples->getAll();
}

std::vector<std::tuple<std::string, std::wstring, std::vector<std::wstring>>> programExamples() {
    return loadedProgramExamples->getAll();
}

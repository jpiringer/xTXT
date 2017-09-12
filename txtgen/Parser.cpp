//
//  Parser.cpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Parser.hpp"
#include "Lexer.hpp"

#include <iostream>
#include <vector>

using namespace jp;

// =============================================================
// Rule
// =============================================================

Rule::Rule(const std::wstring &nm)
: name(nm) {
    
}

std::shared_ptr<RuleChoice> Rule::chooseRandomly() {
    return choices[rand()%choices.size()];
}

// =============================================================
// Parser
// =============================================================

Parser::Parser(const std::wstring &str)
: input(str) {
    
}

enum ParserState {
    ParserStateInit,             // Init
    ParserStateRuleDefinition,    // Rules choices are following
    ParserStateRuleHead,            // Rule Head expecting =
};

#define error(msg, lineNr) {_error(msg, lineNr); return;}
void Parser::_error(const std::wstring &msg, int lineNr) {
    std::wcout << "ERROR at line number " << (lineNr+1) << ": " << msg << std::endl;
}

void Parser::parse() {
    enum ParserState parserState = ParserStateInit;
    Lexer lexer(input);
    std::shared_ptr<Rule> currentRule = nullptr;
    std::shared_ptr<RuleChoice> currentChoice = nullptr;
    std::wstring errmsg = L"";
    
    for (auto lexem : lexer.getLexemes()) {
        switch (lexem->getType()) {
            case LexemSymbol:
                switch (parserState) {
                    case ParserStateInit:
                        currentRule = std::make_shared<Rule>(lexem->getContent());
                        parserState = ParserStateRuleHead;
                        break;
                    case ParserStateRuleDefinition:
                        if (currentRule == nullptr) {
                            error(L"no current rule!", lexem->getLineNumber());
                        }
                        else {
                            currentChoice->addLexem(lexem);
                        }
                        break;
                    default:
                        error(L"unexpected symbol \""+lexem->getContent()+L"\"", lexem->getLineNumber());
                        break;
                }
                break;
            case LexemDelimiter:
                switch (parserState) {
                    case ParserStateRuleDefinition:
                        if (lexem->getContent() == L";") {
                            currentRule->addChoice(currentChoice);
                            currentChoice = nullptr;
                            rules[currentRule->getName()] = currentRule;
                            parserState = ParserStateInit;
                            currentRule = nullptr;
                        }
                        else {
                            error(L"unexpected delimiter "+lexem->getContent(), lexem->getLineNumber());
                        }
                        break;
                    default:
                        error(L"unexpected delimiter "+lexem->getContent(), lexem->getLineNumber());
                        break;
                }
                break;
            case LexemOperator:
                switch (parserState) {
                    case ParserStateRuleHead:
                        if (lexem->getContent() == L"=") {
                            parserState = ParserStateRuleDefinition;
                            currentChoice = std::make_shared<RuleChoice>();
                        }
                        else {
                            error(L"unexpected operator "+lexem->getContent(), lexem->getLineNumber());
                        }
                        break;
                    case ParserStateRuleDefinition:
                        if (lexem->getContent() == L"|") {
                            currentRule->addChoice(currentChoice);
                            currentChoice = std::make_shared<RuleChoice>();
                        }
                        else {
                            error(L"unexpected operator "+lexem->getContent(), lexem->getLineNumber());
                        }
                        break;
                    default:
                        error(L"unexpected operator "+lexem->getContent(), lexem->getLineNumber());
                        break;
                }
                break;
            case LexemString:
                switch(parserState) {
                    case ParserStateRuleDefinition:
                        if (currentRule == nullptr) {
                            error(L"no current rule!", lexem->getLineNumber());
                        }
                        else {
                            currentChoice->addLexem(lexem);
                        }
                        break;
                    default:
                        error(L"unexpected string: \""+lexem->getContent()+L"\"", lexem->getLineNumber());
                        break;
                }
                break;

            default:
                break;
        }
        //std::wcout << lexem->toString() << std::endl;
    }
}

std::wstring Parser::expandRule(const std::wstring &ruleName) {
    std::shared_ptr<Rule> rule = rules[ruleName];
    std::shared_ptr<RuleChoice> chosen = rule->chooseRandomly();
    std::wstring result = L"";
    
    for (auto lexem : chosen->getLexems()) {
        if (result != L"") {
            result += L" ";
        }
        switch (lexem->getType()) {
            case LexemSymbol:
                result += expandRule(lexem->getContent());
                break;
            case LexemString:
                result += lexem->getContent();
                break;
            default:
                result += L"##ERRROR##";
                break;
        }
    }
    return result;
}

std::wstring Parser::generate() {
    if (rules[L"START"] == nullptr) {
        _error(L"no START symbol found", 1);
        return L"";
    }
    
    return expandRule(L"START");
}

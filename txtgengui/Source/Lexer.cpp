//
//  Lexer.cpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Lexer.hpp"
#include "Tokenizer.hpp"

using namespace jp;

bool isCapitalSymbol(const std::wstring &str) {
    std::wstring symbolLetters = CAPITAL_SYMBOL_LETTERS;
    
    for (size_t cpos = 0; cpos < str.size(); cpos++) {
        if (symbolLetters.find(str[cpos]) == std::wstring::npos) {
            return false;
        }
    }
    return true;
}

Lexer::Lexer(const std::wstring &str)
: input(str) {
    
}

std::vector<std::shared_ptr<Lexem>> Lexer::getLexemes() {
    std::vector<std::shared_ptr<Lexem>> lexemes;
    Tokenizer tokenizer(input);
    bool stringCapture = false;
    std::wstring stringCharacter = L"";
    std::wstring currentString = L"";
    
    for (auto token : tokenizer.getTokens()) {
        switch (token->getType()) {
            case TokenWhitespace:
                if (stringCapture) {
                    currentString += token->getContent();
                }
                break;
            case TokenDelimiter:
                if (token->getContent() == L"\"" || token->getContent() == L"\'") {
                    if (stringCapture) {
                        if (token->getContent() == stringCharacter) {
                            lexemes.push_back(std::make_shared<Lexem>(currentString, LexemString, token->getLineNumber()));
                            stringCapture = false;
                            stringCharacter = L"";
                        }
                        else {
                            currentString += token->getContent(); // if not the matching quote add to string
                        }
                    }
                    else {
                        stringCapture = true;
                        currentString = L"";
                        stringCharacter = token->getContent(); // in order to differentiate between " and '
                    }
                }
                else if (token->getContent() == L";") {
                    lexemes.push_back(std::make_shared<Lexem>(token->getContent(), LexemDelimiter, token->getLineNumber()));
                }
                else {
                    lexemes.push_back(std::make_shared<Lexem>(token->getContent(), LexemOperator, token->getLineNumber()));
                }
                break;
            case TokenOther:
                if (stringCapture) {
                    currentString += token->getContent();
                }
                else {
                    if (isCapitalSymbol(token->getContent())) {
                        lexemes.push_back(std::make_shared<Lexem>(token->getContent(), LexemSymbol, token->getLineNumber()));
                    }
                    else {
                        lexemes.push_back(std::make_shared<Lexem>(token->getContent(), LexemString, token->getLineNumber()));
                    }
                }
                break;
            default:
                break;
        }
    }
    
    return lexemes;
}

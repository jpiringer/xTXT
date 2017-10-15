//
//  tokenizer.cpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Tokenizer.hpp"

using namespace jp;

Tokenizer::Tokenizer(const std::wstring &str)
: input(str) {
}

std::vector<std::shared_ptr<Token>> Tokenizer::getTokens() {
    std::vector<std::shared_ptr<Token>> tokens;
    std::wstring whitespace = WHITESPACE;
    std::wstring delimiter = DELIMITER;
    int lineNr = 0;
    bool escape = false;
    bool comment = false;
    
    std::wstring collectedString = L"";
    enum TokenType currentType = TokenNone;
    for (size_t cpos = 0; cpos < input.size(); cpos++) {
        wchar_t c = input[cpos];
        
        if (comment) {
            if (c == '\n') {
                lineNr++;
                comment = false;
            }
        }
        else if (escape) {
            escape = false;
            switch (c) {
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case '\\':
                    c = '\\';
                    break;
                default:
                    break;
            }
            if (currentType != TokenOther) {
                currentType = TokenOther;
                collectedString = std::wstring(1, c);
            }
            else {
                collectedString += c;
            }
        }
        else if (c == L'#') {
            comment = true;
        }
        else if (whitespace.find(c) != std::wstring::npos) {
            if (currentType == TokenOther) {
                tokens.push_back(std::make_shared<Token>(collectedString, TokenOther, lineNr));
                collectedString = L"";
            }
            currentType = TokenWhitespace;
            tokens.push_back(std::make_shared<Token>(L" ", TokenWhitespace, lineNr));
            if (c == '\n') {
                lineNr++;
            }
        }
        else if (delimiter.find(c) != std::wstring::npos) {
            if (currentType == TokenOther) {
                tokens.push_back(std::make_shared<Token>(collectedString, TokenOther, lineNr));
                collectedString = L"";
            }
            currentType = TokenDelimiter;
            std::wstring str(1, c);
            tokens.push_back(std::make_shared<Token>(str, TokenDelimiter, lineNr));
        }
        else if (c == '\\') {
            escape = true;
        }
        else {
            if (currentType != TokenOther) {
                currentType = TokenOther;
                collectedString = std::wstring(1, c);
            }
            else {
                collectedString += c;
            }
        }
    }
    
    return tokens;
}

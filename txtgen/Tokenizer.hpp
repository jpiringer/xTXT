//
//  tokenizer.hpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <stdio.h>

#include <string>
#include <vector>

#define WHITESPACE L" \n\r\t"
#define DELIMITER L"=|\"\';{}[]"

namespace jp {
    
    enum TokenType {
        TokenWhitespace, TokenDelimiter, TokenOther, TokenNone
    };
    
    class Token {
        std::wstring content;
        enum TokenType type;
        int lineNumber = -1;
        
    public:
        Token(const Token &source) : content(source.content), type(source.type), lineNumber(source.lineNumber) {}
        Token(const std::wstring &c, enum TokenType t, int lnr) : content(c), type(t), lineNumber(lnr) {}
        
        const std::wstring &getContent() {return content;}
        enum TokenType getType() {return type;}
        std::wstring getTypeString() {
            switch (type) {
                case TokenWhitespace:
                    return L"T-WHITESPACE";
                    break;
                case TokenDelimiter:
                    return L"T-DELIMITER";
                    break;
                case TokenOther:
                    return L"T-OTHER";
                    break;
                case TokenNone:
                    return L"T-NONE";
                    break;
            }
            return L"T-ILLEGAL";
        }
        
        std::wstring toString() {return getTypeString()+L": "+getContent();}
        
        int getLineNumber() {return lineNumber;}
    };
    
    class Tokenizer {
        std::wstring input;
        
    public:
        Tokenizer(const std::wstring &str);
        
        std::vector<std::shared_ptr<Token>> getTokens();
    };

}

#endif /* tokenizer_hpp */

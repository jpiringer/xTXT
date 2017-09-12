//
//  Lexer.hpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <string>

#define CAPITAL_SYMBOL_LETTERS L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ0123456789_"

namespace jp {
    enum LexemType {
        LexemSymbol, LexemString, LexemNumber, LexemOperator, LexemDelimiter, LexemNone
    };
    
    class Lexem {
        std::wstring content;
        enum LexemType type;
        int lineNumber = -1;
        
    public:
        Lexem(const Lexem &source) : content(source.content), type(source.type), lineNumber(source.lineNumber) {}
        Lexem(const std::wstring &c, enum LexemType t, int lnr) : content(c), type(t), lineNumber(lnr) {}
        
        const std::wstring &getContent() {return content;}
        enum LexemType getType() {return type;}
        std::wstring getTypeString() {
            switch (type) {
                case LexemSymbol:
                    return L"L-SYMBOL";
                    break;
                case LexemString:
                    return L"L-STRING";
                    break;
                case LexemNumber:
                    return L"L-NUMBER";
                    break;
                case LexemOperator:
                    return L"L-OPERATOR";
                    break;
                case LexemDelimiter:
                    return L"L-DELIMITER";
                    break;
                case LexemNone:
                    return L"L-NONE";
                    break;
            }
            return L"L-ILLEGAL";
        }
        
        std::wstring toString() {return getTypeString()+L": "+getContent();}
        
        int getLineNumber() {return lineNumber;}
    };
    
    class Lexer {
        std::wstring input;
        
    public:
        Lexer(const std::wstring &str);
        
        std::vector<std::shared_ptr<Lexem>> getLexemes();
    };
}

#endif /* Lexer_hpp */

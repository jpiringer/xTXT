//
//  Parser.hpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace jp {
    class Lexem;
    
    class RuleChoice {
        std::vector<std::shared_ptr<Lexem>> lexems;
    public:
        void addLexem(std::shared_ptr<Lexem> lex) {lexems.push_back(lex);}
        
        bool isEmpty() {return lexems.size() == 0;}
        
        const std::vector<std::shared_ptr<Lexem>> getLexems() {return lexems;}
    };
    
    class Rule {
        std::wstring name;
        std::vector<std::shared_ptr<RuleChoice>> choices;
        
    public:
        Rule(const std::wstring &nm);
        void addChoice(std::shared_ptr<RuleChoice> choice) {choices.push_back(choice);}
        
        const std::wstring &getName() {return name;}
        
        std::shared_ptr<RuleChoice> chooseRandomly();
    };
    
    class Parser {
        std::wstring input;
        std::map<std::wstring, std::shared_ptr<Rule>> rules;

    protected:
        void _error(const std::wstring &msg, int lineNr);
        std::wstring expandRule(const std::wstring &ruleName);
        
    public:
        Parser(const std::wstring &str);
        
        void parse();
        
        std::wstring generate();
    };
}

#endif /* Parser_hpp */

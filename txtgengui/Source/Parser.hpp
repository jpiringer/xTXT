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

#include "Runner.hpp"

namespace jp {
    class Lexem;
    class Rule;
    class Parser;
    
    enum OperationType {
        OperationTypeEqual, OperationTypeNotEqual,
        OperationTypeLesser, OperationTypeGreater,
        OperationTypeLesserEqual, OperationTypeGreaterEqual,
        OperationTypeInc, OperationTypeDec,
        OperationTypeAssign, OperationTypeAssignPlus,
        OperationTypeAssignMinus, OperationTypeAssignMult,
        OperationTypeAssignDiv,
        OperationTypeIllegal};
    
    class RuleOperationStatement {
        std::vector<std::shared_ptr<Lexem>> operands;
        OperationType operationType;
        
        float getValue(Parser &parser, int lineNr, std::shared_ptr<Lexem> lex);
        void setValue(Parser &parser, int lineNr, std::shared_ptr<Lexem> lex, float v);
        
    public:
        RuleOperationStatement(OperationType t) : operationType(t) {}
        
        void addOperand(std::shared_ptr<Lexem> lex) {operands.push_back(lex);}
        std::shared_ptr<Lexem> execute(Parser &parser, int lineNr);
    };
    
    class RuleOperation {
        std::vector<std::shared_ptr<RuleOperationStatement>> statements;
    public:
        void addStatement(std::shared_ptr<RuleOperationStatement> st) {statements.push_back(st);}
        
        std::shared_ptr<Lexem> execute(Parser &parser, int lineNr);
    };
    
    class RuleChoiceElement {
    public:
        virtual ~RuleChoiceElement() {}
        virtual std::shared_ptr<Lexem> getLexem() {return nullptr;}
        virtual std::shared_ptr<RuleOperation> getOperation() {return nullptr;}
    };
    
    class RuleChoiceElementLexem : public RuleChoiceElement {
        std::shared_ptr<Lexem> lexem;
    public:
        RuleChoiceElementLexem(std::shared_ptr<Lexem> l) {lexem = l;}
        virtual ~RuleChoiceElementLexem() {}

        std::shared_ptr<Lexem> getLexem() override {return lexem;}
    };

    class RuleChoiceElementOperation : public RuleChoiceElement {
        std::shared_ptr<RuleOperation> operation;
    public:
        RuleChoiceElementOperation(std::shared_ptr<RuleOperation> o) {operation = o;}
        virtual ~RuleChoiceElementOperation() {}

        virtual std::shared_ptr<RuleOperation> getOperation() override {return operation;}
   };
    
    class RuleChoice {
        std::vector<std::shared_ptr<RuleChoiceElement>> elements;
    public:
        void addLexem(std::shared_ptr<Lexem> lex) {elements.push_back(std::make_shared<RuleChoiceElementLexem>(lex));}
        void addOperation(std::shared_ptr<RuleOperation> op) {elements.push_back(std::make_shared<RuleChoiceElementOperation>(op));}

        bool isEmpty() {return elements.size() == 0;}
        
        const std::vector<std::shared_ptr<RuleChoiceElement>> getElements() {return elements;}
        
        std::wstring getTraceryGrammarString();
    };
    
    class ChoiceFunction {
    protected:
        const Rule &rule;
    public:
        ChoiceFunction(const Rule &r) : rule(r) {}
        virtual ~ChoiceFunction() {}
        virtual std::shared_ptr<RuleChoice> choose() = 0;
    };

    class PermutationChoiceFunction : public ChoiceFunction {
        std::vector<std::shared_ptr<RuleChoice>> choicesLeft;
        
    public:
        PermutationChoiceFunction(const Rule &r) : ChoiceFunction(r) {}
        std::shared_ptr<RuleChoice> choose() override;
    };
    
    class RandomChoiceFunction : public ChoiceFunction {
    public:
        RandomChoiceFunction(const Rule &r) : ChoiceFunction(r) {}
        std::shared_ptr<RuleChoice> choose() override;
    };

    class OrderChoiceFunction : public ChoiceFunction {
        size_t pos = 0;
    public:
        OrderChoiceFunction(const Rule &r) : ChoiceFunction(r) {}
        std::shared_ptr<RuleChoice> choose() override;
    };
    
    class Rule {
        std::wstring name;
        std::vector<std::shared_ptr<RuleChoice>> choices;
        std::shared_ptr<ChoiceFunction> choiceFunction = nullptr;
        std::shared_ptr<RuleOperation> ruleOperation;
        
    public:
        Rule(const std::wstring &nm);
        void addChoice(std::shared_ptr<RuleChoice> choice) {choices.push_back(choice);}
        
        void setChoiceFunction(std::shared_ptr<ChoiceFunction> cf) {choiceFunction = cf;}
        
        void setOperation(std::shared_ptr<RuleOperation> op) {ruleOperation = op;}
        bool matches(Parser &parser, int lineNr);
        
        const std::vector<std::shared_ptr<RuleChoice>> &getChoices() const {return choices;}
        
        const std::wstring &getName() {return name;}
        std::wstring getTraceryGrammarString();

        std::shared_ptr<RuleChoice> choose();
    };
    
    class Parser {
        std::wstring input;
        std::multimap<std::wstring, std::shared_ptr<Rule>> rules;
        std::map<std::wstring, float> variables;

        int errorCount = 0;
        std::wstring errors;

    protected:
        std::wstring expandRule(const std::wstring &ruleName, int lineNr, int recursionDepth = 0);
        
    public:
        static std::wstring convertToTraceryGramnmar(const std::wstring &code, jp::RunnerType sourceType);
        
        Parser(const std::wstring &str);
        
        void parse();
        
        std::wstring generate();
        
        std::wstring getTraceryGrammarString();
        
        int getErrorCount() {return errorCount;}
        const std::wstring &getErrors() {return errors;}
        
        void setVariable(std::wstring name, float v);
        float getVariable(std::wstring name, int lineNr);
        bool isVariableDefined(std::wstring name);

        void _error(const std::wstring &msg, int lineNr);
    };
}

#endif /* Parser_hpp */

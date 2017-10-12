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
#include <utility>

#include <math.h>

using namespace jp;

#define TRUE_LEX L"true"
#define FALSE_LEX L"false"

// =============================================================
// RuleOperationStatement
// =============================================================

float RuleOperationStatement::getValue(Parser &parser, int lineNr, std::shared_ptr<Lexem> lex) {
    switch (lex->getType()) {
        case LexemNumber:
            return std::stof(lex->getContent());
        case LexemSymbol:
            return parser.getVariable(lex->getContent(), lineNr);
        default:
            parser._error(L"unknown operand type of \""+lex->getContent()+L"\"", lineNr);
            break;
    }
    return 0;
}

void RuleOperationStatement::setValue(Parser &parser, int lineNr, std::shared_ptr<Lexem> lex, float v) {
    switch (lex->getType()) {
        case LexemSymbol:
            parser.setVariable(lex->getContent(), v);
            break;
        default:
            parser._error(L"unknown operand type of \""+lex->getContent()+L"\"", lineNr);
            break;
    }
}

std::shared_ptr<Lexem> RuleOperationStatement::execute(Parser &parser, int lineNr) {
    std::shared_ptr<Lexem> ret = nullptr;
    
    switch (operationType) {
        case OperationTypeEqual:
            if (getValue(parser, lineNr, operands[0]) == getValue(parser, lineNr, operands[1])) {
                return std::make_shared<Lexem>(TRUE_LEX, LexemBool, lineNr);
            }
            else {
                return std::make_shared<Lexem>(FALSE_LEX, LexemBool, lineNr);
            }
            break;
        case OperationTypeNotEqual:
            if (getValue(parser, lineNr, operands[0]) != getValue(parser, lineNr, operands[1])) {
                return std::make_shared<Lexem>(TRUE_LEX, LexemBool, lineNr);
            }
            else {
                return std::make_shared<Lexem>(FALSE_LEX, LexemBool, lineNr);
            }
            break;
        case OperationTypeLesser:
            if (getValue(parser, lineNr, operands[0]) < getValue(parser, lineNr, operands[1])) {
                return std::make_shared<Lexem>(TRUE_LEX, LexemBool, lineNr);
            }
            else {
                return std::make_shared<Lexem>(FALSE_LEX, LexemBool, lineNr);
            }
            break;
        case OperationTypeGreater:
            if (getValue(parser, lineNr, operands[0]) > getValue(parser, lineNr, operands[1])) {
                return std::make_shared<Lexem>(TRUE_LEX, LexemBool, lineNr);
            }
            else {
                return std::make_shared<Lexem>(FALSE_LEX, LexemBool, lineNr);
            }
            break;
        case OperationTypeLesserEqual:
            if (getValue(parser, lineNr, operands[0]) <= getValue(parser, lineNr, operands[1])) {
                return std::make_shared<Lexem>(TRUE_LEX, LexemBool, lineNr);
            }
            else {
                return std::make_shared<Lexem>(FALSE_LEX, LexemBool, lineNr);
            }
            break;
        case OperationTypeGreaterEqual:
            if (getValue(parser, lineNr, operands[0]) >= getValue(parser, lineNr, operands[1])) {
                return std::make_shared<Lexem>(TRUE_LEX, LexemBool, lineNr);
            }
            else {
                return std::make_shared<Lexem>(FALSE_LEX, LexemBool, lineNr);
            }
            break;
        case OperationTypeInc:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[0])+1);
            break;
        case OperationTypeDec:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[0])-1);
            break;
        case OperationTypeAssign:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[1]));
            break;
        case OperationTypeAssignPlus:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[0])+getValue(parser, lineNr, operands[1]));
            break;
        case OperationTypeAssignMinus:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[0])-getValue(parser, lineNr, operands[1]));
            break;
        case OperationTypeAssignMult:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[0])*getValue(parser, lineNr, operands[1]));
            break;
        case OperationTypeAssignDiv:
            setValue(parser, lineNr, operands[0], getValue(parser, lineNr, operands[0])/getValue(parser, lineNr, operands[1]));
            break;

        default:
            break;
    }
    return ret;
}

// =============================================================
// RuleOperation
// =============================================================

std::shared_ptr<Lexem> RuleOperation::execute(Parser &parser, int lineNr) {
    std::shared_ptr<Lexem> ret = nullptr;
    
    for (auto statement : statements) {
        ret = statement->execute(parser, lineNr);
    }
    return ret;
}

// =============================================================
// ChoiceFunctions
// =============================================================

std::shared_ptr<RuleChoice> PermutationChoiceFunction::choose() {
    if (choicesLeft.empty()) {
        choicesLeft = rule.getChoices();
    }
    
    auto chosenIndex = rand() % choicesLeft.size();
    auto chosen = choicesLeft[chosenIndex];
    choicesLeft.erase(choicesLeft.begin()+chosenIndex);
    
    return chosen;
}

std::shared_ptr<RuleChoice> RandomChoiceFunction::choose() {
    return rule.getChoices()[rand() % rule.getChoices().size()];
}

std::shared_ptr<RuleChoice> OrderChoiceFunction::choose() {
    return rule.getChoices()[pos++ % rule.getChoices().size()];
}

// =============================================================
// Rule
// =============================================================

Rule::Rule(const std::wstring &nm)
: name(nm) {
    choiceFunction = std::make_shared<RandomChoiceFunction>(*this);
}

std::shared_ptr<RuleChoice> Rule::choose() {
    return choiceFunction->choose();
}

bool Rule::matches(Parser &parser, int lineNr) {
    if (ruleOperation == nullptr) {
        return true;
    }
    else {
        auto result = ruleOperation->execute(parser, lineNr);
        
        if (result->getType() == LexemBool) {
            return result->getContent() == TRUE_LEX;
        }
        else {
            parser._error(L"result of rule head operation must be a bool", lineNr);
            return false;
        }
    }
}

// =============================================================
// Parser
// =============================================================

Parser::Parser(const std::wstring &str)
: input(str) {
    
}

enum ParserState {
    ParserStateInit,                // Init
    ParserStateRuleDefinition,      // Rules choices are following
    ParserStateRuleHead,            // Rule Head expecting =
    ParserStateRuleHeadOption,      // Rule Head expecting }
    ParserStateRuleOperation        // operations that will be evaluated in between {}
};

#define error(msg, lineNr) {_error(msg, lineNr); return;}
void Parser::_error(const std::wstring &msg, int lineNr) {
    std::wstring errStr = L"ERROR at line number ";
    errStr += std::to_wstring(lineNr+1);
    errStr += L": ";
    errStr += msg;
    errStr += L"\n";
    
    std::wcout << errStr;
    errors += errStr;
    
    errorCount++;
}

std::vector<std::tuple<std::wstring, OperationType, int>> operators = { // third par is nr of operands
    {L"=", OperationTypeAssign, 2},
    {L"==", OperationTypeEqual, 2},
    {L"~=", OperationTypeNotEqual, 2},
    {L"<", OperationTypeLesser, 2},
    {L">", OperationTypeGreater, 2},
    {L">=", OperationTypeGreaterEqual, 2},
    {L"<=", OperationTypeLesserEqual, 2},
    {L"+=", OperationTypeAssignPlus, 2},
    {L"-=", OperationTypeAssignMinus, 2},
    {L"*=", OperationTypeAssignMult, 2},
    {L"/=", OperationTypeAssignDiv, 2},
    {L"++", OperationTypeInc, 1},
    {L"--", OperationTypeDec, 1}
};

OperationType getOperationTypeForLexem(std::shared_ptr<Lexem> op) {
    for (auto t : operators) {
        if (std::get<0>(t) == op->getContent()) {
            return std::get<1>(t);
        }
    }

    return OperationTypeIllegal;
}

void Parser::parse() {
    enum ParserState parserState = ParserStateInit;
    Lexer lexer(input);
    std::shared_ptr<Rule> currentRule = nullptr;
    std::shared_ptr<RuleChoice> currentChoice = nullptr;
    std::shared_ptr<RuleOperation> currentOperation = nullptr;
    std::wstring errmsg = L"";
    bool noWhitespace = false;
    std::shared_ptr<Lexem> operand1 = nullptr;
    std::shared_ptr<Lexem> operand2 = nullptr;
    std::shared_ptr<Lexem> op = nullptr;
    bool wantsSecondOperand = false;

    errorCount = 0;
    errors = L"";
    
    auto pushStatement = [&operand1, &operand2, &op, &currentOperation, &wantsSecondOperand](std::shared_ptr<RuleOperationStatement> statement) {
        currentOperation->addStatement(statement);
        operand1 = nullptr;
        operand2 = nullptr;
        op = nullptr;
        wantsSecondOperand = false;
    };
    
    auto isBinaryOperator = [](const std::wstring opStr) -> bool {
        for (auto o : operators) {
            if (std::get<0>(o) == opStr && std::get<2>(o) == 2) {
                return true;
            }
        }
        return false;
    };
    
    for (auto lexem : lexer.getLexemes()) {
        if (parserState == ParserStateRuleOperation || parserState == ParserStateRuleHeadOption) {
            switch (lexem->getType()) {
                case LexemBracket:
                    if (lexem->getContent() == L"}") {
                        if (parserState == ParserStateRuleHeadOption) {
                            parserState = ParserStateRuleHead;
                            currentRule->setOperation(currentOperation);
                        }
                        else {
                            parserState = ParserStateRuleDefinition;
                            currentChoice->addOperation(currentOperation);
                            noWhitespace = true; // don't add whitespace after operation
                        }
                        currentOperation = nullptr;
                    }
                    else {
                        error(L"unexpected bracket '"+lexem->getContent()+L"'", lexem->getLineNumber());
                    }
                    break;
                case LexemSymbol:
                case LexemNumber:
                    if (operand1 == nullptr) {
                        operand1 = lexem;
                    }
                    else {
                        if (operand2 == nullptr) {
                            operand2 = lexem;
                            if (wantsSecondOperand) {
                                auto statement = std::make_shared<RuleOperationStatement>(getOperationTypeForLexem(op));

                                statement->addOperand(operand1);
                                statement->addOperand(operand2);
                                pushStatement(statement);
                            }
                            else {
                                _error(L"too many operands", lexem->getLineNumber());
                            }
                        }
                        else {
                            _error(L"too many operands", lexem->getLineNumber());
                        }
                    }
                    break;
                case LexemString:
                    if (lexem->getContent() == L"permutate") {
                        currentRule->setChoiceFunction(std::make_shared<PermutationChoiceFunction>(*currentRule));
                    }
                    else if (lexem->getContent() == L"random") {
                        currentRule->setChoiceFunction(std::make_shared<RandomChoiceFunction>(*currentRule));
                    }
                    else if (lexem->getContent() == L"order") {
                        currentRule->setChoiceFunction(std::make_shared<OrderChoiceFunction>(*currentRule));
                    }
                    else {
                        error(L"unknown rule option \""+lexem->getContent()+L"\"", lexem->getLineNumber());
                    }
                    break;
                case LexemOperator:
                    if (isBinaryOperator(lexem->getContent())) {
                        op = lexem;
                        wantsSecondOperand = true;
                    }
                    else if (lexem->getContent() == L"++"){
                        auto statement = std::make_shared<RuleOperationStatement>(OperationTypeInc);
                        if (operand1 == nullptr || operand2 != nullptr) {
                            _error(L"wrong number of operands for ++", lexem->getLineNumber());
                        }
                        statement->addOperand(operand1);
                        pushStatement(statement);
                    }
                    else if (lexem->getContent() == L"--"){
                        auto statement = std::make_shared<RuleOperationStatement>(OperationTypeDec);
                        if (operand1 == nullptr || operand2 != nullptr) {
                            _error(L"wrong number of operands for ++", lexem->getLineNumber());
                        }
                        statement->addOperand(operand1);
                        pushStatement(statement);
                    }
                    else {
                        _error(L"unknown operator \""+lexem->getContent()+L"\"", lexem->getLineNumber());
                    }
                    break;
                default:
                    error(L"error parsing token \""+lexem->getContent()+L"\"", lexem->getLineNumber());
                    break;
            }
        }
        else {
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
                                if (!currentChoice->isEmpty() && !noWhitespace) {
                                    currentChoice->addLexem(std::make_shared<Lexem>(L" ", LexemString, -1));
                                }
                                currentChoice->addLexem(lexem);
                                noWhitespace = false;
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
                                rules.insert(std::make_pair(currentRule->getName(), currentRule));
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
                            else if (lexem->getContent() == L"+") {
                                noWhitespace = true;
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
                case LexemBracket:
                    switch (parserState) {
                        case ParserStateRuleHead:
                            if (lexem->getContent() == L"{") {
                                parserState = ParserStateRuleHeadOption;
                                currentOperation = std::make_shared<RuleOperation>();
                            }
                            break;
                        /*case ParserStateRuleHeadOption:
                            if (lexem->getContent() == L"}") {
                                parserState = ParserStateRuleHead;
                            }
                            break;*/
                        case ParserStateRuleDefinition:
                            if (lexem->getContent() == L"{") {
                                parserState = ParserStateRuleOperation;
                                currentOperation = std::make_shared<RuleOperation>();
                            }
                            break;
                        default:
                            error(L"unexpected bracket '"+lexem->getContent()+L"'", lexem->getLineNumber());
                            break;
                    }
                    break;
                case LexemNumber:
                case LexemString:
                    switch(parserState) {
                        case ParserStateRuleDefinition:
                            if (currentRule == nullptr) {
                                error(L"no current rule!", lexem->getLineNumber());
                            }
                            else {
                                if (!currentChoice->isEmpty() && !noWhitespace) {
                                    currentChoice->addLexem(std::make_shared<Lexem>(L" ", LexemString, -1));
                                }
                                currentChoice->addLexem(lexem);
                                noWhitespace = false;
                            }
                            break;
                        case ParserStateRuleHeadOption:

                        default:
                            error(L"unexpected string: \""+lexem->getContent()+L"\"", lexem->getLineNumber());
                            break;
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
}

std::wstring Parser::expandRule(const std::wstring &ruleName, int lineNr) {
    std::wstring result = L"";
    
    if (rules.find(ruleName) == rules.end()) {
        if (isVariableDefined(ruleName)) {
            float v = getVariable(ruleName, lineNr);
            
            if (v-trunc(v) == 0) {
                result = std::to_wstring((long)v);
            }
            else{
                result = std::to_wstring(v);
            }
        }
        else {
            _error(std::wstring(L"rule or variable \"")+ruleName+std::wstring(L"\" not found"), lineNr);
            return L"";
        }
    }
    else {
        auto ruleRange = rules.equal_range(ruleName);

        for (auto i = ruleRange.first; i != ruleRange.second; ++i) {
            auto rule = i->second;
            if (rule->matches(*this, lineNr)) {
                std::shared_ptr<RuleChoice> chosen = rule->choose();
                
                for (auto element : chosen->getElements()) {
                    std::shared_ptr<Lexem> lexem = nullptr;
                    
                    if (dynamic_cast<RuleChoiceElementLexem*>(element.get())) {
                        lexem = element->getLexem();
                    }
                    else { // operation
                        auto operation = element->getOperation();
                        
                        lexem = operation->execute(*this, lineNr);
                    }
                    
                    if (lexem != nullptr) {
                        switch (lexem->getType()) {
                            case LexemSymbol:
                                result += expandRule(lexem->getContent(), lexem->getLineNumber());
                                break;
                            case LexemNumber:
                            case LexemString:
                                result += lexem->getContent();
                                break;
                            default:
                                result += L"##ERRROR##";
                                break;
                        }
                    }
                }
                break;
            }
        }
    }
    return result;
}

bool Parser::isVariableDefined(std::wstring name) {
    return variables.find(name) != variables.end();
}

void Parser::setVariable(std::wstring name, float v) {
    variables[name] = v;
}

float Parser::getVariable(std::wstring name, int lineNr) {
    if (isVariableDefined(name)) {
        return variables[name];
    }
    else {
        _error(L"variable \""+name+L"\" undefined", lineNr);
    }
    return 0;
}

std::wstring Parser::generate() {
    if (getErrorCount() == 0) {
        if (rules.find(L"START") == rules.end()) {
            _error(L"no START symbol found", 0);
            return L"";
        }
        
        return expandRule(L"START", 0);
    }
    else {
        return L"";
    }
}

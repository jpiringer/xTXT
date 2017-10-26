/*
 *  LSystem.h
 *  abcdefg
 *
 *  Created by joerg on 12.10.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LSYSTEM_H
#define _LSYSTEM_H

#include <vector>
#include <memory>
#include <string>

class LSystem {
	class LSystemRule {
        std::wstring head;
		std::wstring tail;
	public:
		LSystemRule(const std::wstring &hd, const std::wstring &tl) {head = hd; tail = tl;}
		
		bool isMatch(const std::wstring &str) {return str.substr(0, head.length()) == head;}
        const std::wstring &getHead() {return head;}
		const std::wstring &getTail() {return tail;}
	};
	
	std::wstring state;
    std::vector<std::shared_ptr<LSystemRule>> rules;
    
    int errorCount = 0;
    std::wstring errors;
    
public:

    void _error(const std::wstring &msg, int lineNr) {
        std::wstring errStr = L"ERROR at line number ";
        errStr += std::to_wstring(lineNr+1);
        errStr += L": ";
        errStr += msg;
        errStr += L"\n";
        
        errors += errStr;
        
        errorCount++;
    }
    
    LSystem() {}
	
	void setState(const std::wstring &st) {state = st;}
	std::wstring &getState() {return state;}
	
    void addRule(const std::wstring &hd, const std::wstring &tl) {
        rules.push_back(std::make_shared<LSystemRule>(hd, tl));
    }
    
    std::wstring longestReplacement(const std::wstring &str, size_t &matchLength) {
        size_t length = 0;
        std::shared_ptr<LSystemRule> longestRule = nullptr;
        
        for(int i = 0; i < rules.size(); i++) {
            if (rules[i]->isMatch(str)) {
                size_t l = rules[i]->getHead().length();
                
                if (l > length) {
                    longestRule = rules[i];
                    length = l;
                }
            }
        }
        
        if (longestRule == nullptr) {
            matchLength = 0;
            return L"";
        }
        else {
            matchLength = length;
            return longestRule->getTail();
        }
    }
	
    void iterate() {
        std::wstring newstate = L"";
        
        for(int i = 0; i < state.size();) {
            size_t matchLength = 0;
            std::wstring replacement = longestReplacement(state.substr(i), matchLength);

            if (matchLength > 0) {
                newstate += replacement;
                i += matchLength;
            }
            else {
                newstate += state[i];
                i++;
            }
        }
        
        state = newstate;
    }
    
    int getErrorCount() {return errorCount;}
    const std::wstring &getErrors() {return errors;}

    static std::shared_ptr<LSystem> parseLSystem(const std::wstring &code);
};

#endif

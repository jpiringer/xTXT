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

template<typename Element, class Sequence> class LSystem {
	class LSystemRule {
		Element head;
		Sequence tail;
	public:
		LSystemRule(Element hd, Sequence tl) {head = hd; tail = tl;}
		
		bool isMatch(Element hd) {return hd == head;}
		Sequence &getTail() {return tail;}
	};
	
	Sequence state;
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
	
	void setState(Sequence st) {state = st;}
	Sequence &getState() {return state;}
	
    void addRule(Element hd, Sequence tl) {
        rules.push_back(std::make_shared<LSystemRule>(hd, tl));
    }
	
    Sequence &replace(Element &hd) {
        static Sequence seq;
        
        seq.clear();
        
        for(int i = 0; i < rules.size(); i++) {
            if(rules[i]->isMatch(hd))
                return rules[i]->getTail();
        }
        
        seq.push_back(hd);
        return seq;
    }
	
    void iterate() {
        Sequence newstate;
        
        for(int i = 0; i < state.size(); i++) {
            Sequence &replacement = replace(state[i]);
            
            for(int e = 0; e < replacement.size(); e++) {
                newstate.push_back(replacement[e]);
            }
        }
        
        state = newstate;
    }
    
    int getErrorCount() {return errorCount;}
    const std::wstring &getErrors() {return errors;}
};

std::shared_ptr<LSystem<wchar_t, std::wstring>> parseLSystem(const std::wstring &code);

#endif

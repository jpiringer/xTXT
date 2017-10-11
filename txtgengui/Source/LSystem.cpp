/*
 *  LSystem.cpp
 *  abcdefg
 *
 *  Created by joerg on 12.10.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "LSystem.h"

#include <string>

#define WHITESPACE L" \n\r\t"

#define error(msg, lineNr) {lsystem->_error(msg, lineNr); return lsystem;}

std::shared_ptr<LSystem<wchar_t, std::wstring>> parseLSystem(const std::wstring &code) {
    std::wstring whitespace = WHITESPACE;
    auto lsystem = std::make_shared<LSystem<wchar_t, std::wstring>>();
    bool lookingForHead = true;
    bool lookingForEqual = false;
    bool lookingForTailStart = false;
    bool lookingForSemicolon = false;
    wchar_t currentHead = '\0';
    std::wstring currentTail = L"";
    int lineNr = 0;
    
    for (size_t i = 0; i < code.size(); i++) {
        wchar_t c = code[i];
        
        if (c == '\n') {
            lineNr++;
        }
        
        if (lookingForHead) {
            if (whitespace.find(c) == std::wstring::npos) {
                currentHead = c;
                currentTail = L"";
                lookingForHead = false;
                lookingForEqual = true;
            }
        }
        else if (lookingForEqual) {
            if (whitespace.find(c) == std::wstring::npos) {
                if (c == '=') {
                    lookingForEqual = false;
                    lookingForTailStart = true;
                }
                else { // error
                    error(L"no '=' found", lineNr);
                }
            }
        }
        else if (lookingForTailStart) {
            if (whitespace.find(c) == std::wstring::npos) {
                lookingForTailStart = false;
                lookingForSemicolon = true;
                currentTail += c;
            }
        }
        else if (lookingForSemicolon) {
            if (c == ';') {
                lookingForSemicolon = false;
                lookingForHead = true;
                lsystem->addRule(currentHead, currentTail);
            }
            else {
                currentTail += c;
            }
        }
    }
    
    return lsystem;
}

void initLSystem()
{
	/*LSystem<char, string> test;
	
	test.setState("geschmurgel");
	test.addRule('e', "ebe");
	test.addRule('u', "ubu");
	
	test.iterate();
	printf("state1: %s\n", test.getState().c_str());
	test.iterate();
	printf("state2: %s\n", test.getState().c_str());
	test.iterate();
	printf("state3: %s\n", test.getState().c_str());*/
}

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

#include <iostream>

#define WHITESPACE L" \n\r\t"

#define error(msg, lineNr) {lsystem->_error(msg, lineNr); return lsystem;}

std::shared_ptr<LSystem> LSystem::parseLSystem(const std::wstring &code) {
    std::wstring whitespace = WHITESPACE;
    auto lsystem = std::make_shared<LSystem>();
    bool lookingForHead = true;
    bool lookingForEqual = false;
    bool lookingForTailStart = false;
    bool lookingForSemicolon = false;
    std::wstring currentHead = L"";
    std::wstring currentTail = L"";
    int lineNr = 0;
    bool comment = false;
    std::wstring commentContent = L"";
    bool lookingForEscape = false;
    
    for (size_t i = 0; i < code.size(); i++) {
        wchar_t c = code[i];
        auto escapeTrans = [&c, lookingForEscape, lineNr, lsystem] {
            if (lookingForEscape) {
                switch (c) {
                    case 'n':
                        c = '\n';
                        break;
                    case 's':
                        c = ' ';
                        break;
                    case '=':
                        c = '=';
                        break;
                    case '\\':
                        c = '\\';
                        break;
                    default: {
                        std::wstring cstr = L"";
                        cstr += c;
                        lsystem->_error(L"wrong escape sequence '\\"+cstr+L"'", lineNr);
                        break;
                    }
                }
            }
        };
        
        if (c == '\n') {
            lineNr++;
        }
        
        if (comment) {
            if (c == '\n') {
                //lsystem->parseComment(commentContent);
                comment = false;
            }
            else {
                commentContent += c;
            }
        }
        else if (c == '#') {
            commentContent = L"";
            comment = true;
        }
        else if (lookingForHead) {
            if (whitespace.find(c) == std::wstring::npos) {
                if (c == '\\' && !lookingForEscape) {
                    lookingForEscape = true;
                }
                else if (c == '=') {
                    lookingForEqual = false;
                    lookingForTailStart = true;
                }
                else {
                    escapeTrans();
                    currentHead += c;
                    currentTail = L"";
                    lookingForEscape = false;
                }
            }
            else {
                if (currentHead.length() > 0) {
                    lookingForHead = false;
                    lookingForEqual = true;
                    lookingForEscape = false;
                }
            }
        }
        else if (lookingForEqual) {
            if (whitespace.find(c) == std::wstring::npos) {
                if (c == '=') {
                    lookingForEqual = false;
                    lookingForTailStart = true;
                }
                else { // error
                    error(L"'=' expected", lineNr);
                }
            }
        }
        else if (lookingForTailStart) {
            if (whitespace.find(c) == std::wstring::npos) {
                if (c == '\\' && !lookingForEscape) {
                    lookingForEscape = true;
                }
                else {
                    lookingForTailStart = false;
                    lookingForSemicolon = true;
                    escapeTrans();
                    currentTail += c;
                }
            }
        }
        else if (lookingForSemicolon) {
            if (c == '\\' && !lookingForEscape) {
                lookingForEscape = true;
            }
            else {
                if (c == ';') {
                    lookingForSemicolon = false;
                    lookingForHead = true;
                    lsystem->addRule(currentHead, currentTail);
                    currentHead = L"";
                }
                else {
                    escapeTrans();
                    currentTail += c;
                }
            }
        }
    }
    
    return lsystem;
}

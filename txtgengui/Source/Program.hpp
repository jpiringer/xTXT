//
//  Program.hpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef Program_hpp
#define Program_hpp

#include <string>

namespace jp {
    enum Justification {
        JustificationLeft,
        JustificationRight,
        JustificationCenter
    };
};

class LuaProgram {
    static LuaProgram *theProgram;
    
    std::string code;
    
    int errorCount = 0;
    std::string errors;
    
    float colorR, colorG, colorB, colorA;
    float backgroundR, backgroundG, backgroundB, backgroundA;
    float size;
    enum jp::Justification justification;
    
protected:
    void _error(const std::string &msg) {
        errors += msg;
        
        errorCount++;
    }
    
public:
    LuaProgram() {}
    
    int getErrorCount() {return errorCount;}
    const std::string &getErrors() {return errors;}
    
    void setCode(const std::string &c) {code = c;}
    std::string execute();
    
    void setColor(float r, float g, float b, float a) {colorR = r; colorG = g; colorB = b; colorA = a;}
    void setBackground(float r, float g, float b, float a) {backgroundR = r; backgroundG = g; backgroundB = b; backgroundA = a;}
    void setJustification(enum jp::Justification j) {justification = j;}
    void setSize(float s) {size = s;}
    
    static std::wstring convertToProgram(const std::wstring &str);
    
    static LuaProgram *sharedProgram() {return theProgram;}
};
#endif /* Program_hpp */

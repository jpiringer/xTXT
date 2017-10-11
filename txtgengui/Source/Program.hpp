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

class LuaProgram {
    std::string code;
    
    int errorCount = 0;
    std::string errors;
    
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
};
#endif /* Program_hpp */

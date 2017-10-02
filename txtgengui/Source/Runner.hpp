//
//  Runner.hpp
//  txtgengui - App
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef Runner_hpp
#define Runner_hpp

#include <string>

namespace jp {
    enum RunnerType {
        Automate = 0
    };
    
    class Runner {
        enum RunnerType runnerType;
        std::string code;
        
        bool errorsHappened = false;
        
        std::string runAutomate();
        
    public:
        Runner(enum RunnerType rtype = Automate);
        
        void setCode(const std::string &_code) {code = _code;}
        
        std::string getSampleCode();
        
        std::string run();
        
        bool hasErrors() {return errorsHappened;}
    };
};

#endif /* Runner_hpp */

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
        Grammar = 0,
        LSystem,
        Markov,
        Program
    };
    
    class Runner {
        enum RunnerType runnerType;
        std::string code;
        std::string text;

        bool errorsHappened = false;
        
        std::string runAutomate();
        std::string runLSystem();

    public:
        Runner(enum RunnerType rtype = Grammar);
        
        void setCode(const std::string &_code) {code = _code;}
        void setText(const std::string &_text) {text = _text;}
        void setType(RunnerType t) {runnerType = t;}
        RunnerType getType() {return runnerType;}
                
        std::string run();
        
        bool hasErrors() {return errorsHappened;}
        
        std::vector<std::string> getExamples();
    };
};

#endif /* Runner_hpp */

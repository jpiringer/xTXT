//
//  Runner.hpp
//  txtgengui - App
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef Runner_hpp
#define Runner_hpp

#include <string>
#include <map>
#include <vector>

namespace jp {
    enum RunnerType {
        Grammar = 0,
        LSystem,
        Markov,
        Program,
        NamShub
    };
    
    class Runner {
        enum RunnerType runnerType = Markov;
        std::string code;
        std::string text;

        bool errorsHappened = false;
        
        std::string runAutomate();
        std::string runLSystem();
        std::string runMarkov();
        std::string runProgram();
        std::string runNamShub();

        std::map<std::string, double> parameters;
        std::map<std::string, std::string> stringParameters;

    public:
        Runner(enum RunnerType rtype = Markov);
        
        void setCode(const std::string &_code) {code = _code;}
        void setText(const std::string &_text) {text = _text;}
        void setType(RunnerType t) {runnerType = t;}
        RunnerType getType() {return runnerType;}
                
        std::string run();
        
        bool hasErrors() {return errorsHappened;}
        void resetErrors() {errorsHappened = false;}
        
        void setParameter(const std::string &parName, double value) {parameters[parName] = value;}
        double getParameter(const std::string &parName);
        void setStringParameter(const std::string &parName, std::string value) {stringParameters[parName] = value;}
        std::string getStringParameter(const std::string &parName);
        
        std::vector<std::pair<std::string,std::wstring>> getExamples();
    };
};

#endif /* Runner_hpp */

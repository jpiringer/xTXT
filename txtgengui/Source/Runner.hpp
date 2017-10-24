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

#include "ShowWindow.hpp"

namespace jp {
    enum RunnerType {
        Grammar = 0,
        LSystem,
        Markov,
        Program,
        NamShub
    };
    
    class Runner {
    protected:
        std::string code;
        std::string text;

        bool errorsHappened = false;

        std::map<std::string, double> parameters;
        std::map<std::string, std::string> stringParameters;

    public:
        virtual ~Runner() {}

        void setCode(const std::string &_code) {code = _code;}
        void setText(const std::string &_text) {text = _text;}
        
        virtual RunnerType getType() = 0;
                
        virtual std::string run() = 0;
        
        bool hasErrors() {return errorsHappened;}
        void resetErrors() {errorsHappened = false;}
        
        void setParameter(const std::string &parName, double value) {parameters[parName] = value;}
        double getParameter(const std::string &parName);
        void setStringParameter(const std::string &parName, std::string value) {stringParameters[parName] = value;}
        std::string getStringParameter(const std::string &parName);
        
        virtual std::vector<std::pair<std::string,std::wstring>> getExamples() = 0;
        
        virtual DrawFunction getDrawFunction() = 0;
        virtual bool isAnimated() = 0;
    };
    
    class AutomateRunner : public Runner {
    public:
        virtual ~AutomateRunner() {}
        
        virtual RunnerType getType() override {return Grammar;}
        
        virtual std::string run() override;

        virtual std::vector<std::pair<std::string,std::wstring>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;
    };

    class LSystemRunner : public Runner {
    public:
        virtual ~LSystemRunner() {}
        
        virtual RunnerType getType() override {return LSystem;}
        
        virtual std::string run() override;
        
        virtual std::vector<std::pair<std::string,std::wstring>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;
    };
    
    class MarkovRunner : public Runner {
    public:
        virtual ~MarkovRunner() {}
        
        virtual RunnerType getType() override {return Markov;}
        
        virtual std::string run() override;
        
        virtual std::vector<std::pair<std::string,std::wstring>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;
    };

    class ProgramRunner : public Runner {
    public:
        virtual ~ProgramRunner() {}
        
        virtual RunnerType getType() override {return Program;}
        
        virtual std::string run() override;
        
        virtual std::vector<std::pair<std::string,std::wstring>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;
    };

    class NamShubRunner : public Runner {
    public:
        virtual ~NamShubRunner() {}
        
        virtual RunnerType getType() override {return NamShub;}
        
        virtual std::string run() override;
        
        virtual std::vector<std::pair<std::string,std::wstring>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;
    };

};

#endif /* Runner_hpp */

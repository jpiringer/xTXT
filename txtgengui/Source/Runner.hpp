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

#include "TextTurtleGraphics.hpp"

#include "LSystem.h"
//#include "Program.hpp"

class LSystem;
class LuaProgram;

namespace jp {
    enum RunnerType {
        Grammar = 0,
        LSystem,
        Markov,
        Program,
        NamShub
    };
    
    class TextTurtleGraphics;
   
    class RunContext {
    public:
        virtual ~RunContext() {}
        virtual void changeOutput(const std::string &str) = 0;
        virtual void setErrors(const std::string &str) = 0;
        virtual void changeShowSize(float width, float height) = 0;
        virtual void speak(std::string str) = 0;
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
                
        virtual std::string run(RunContext *runContext) = 0;
        
        bool hasErrors() {return errorsHappened;}
        void resetErrors() {errorsHappened = false;}
        
        void changeOutput(const std::string &str);
        void changeShowSize(float width, float height);
        
        void setParameter(const std::string &parName, double value) {parameters[parName] = value;}
        double getParameter(const std::string &parName, float defValue = 0);
        void setStringParameter(const std::string &parName, std::string value) {stringParameters[parName] = value;}
        std::string getStringParameter(const std::string &parName);
        
        virtual std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> getExamples() = 0;
        
        virtual DrawFunction getDrawFunction() = 0;
        virtual bool isAnimated() = 0;
        
        virtual void exportFile(const std::string &fileName) = 0;
        virtual std::string exportsToFileType() {return "";}
        
        virtual void keyPressed(int keyCode) {}
    };
    
    class AutomateRunner : public Runner {
    public:
        virtual ~AutomateRunner() {}
        
        virtual RunnerType getType() override {return Grammar;}
        
        virtual std::string run(RunContext *runContext) override;

        virtual std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;
        
        virtual void exportFile(const std::string &fileName) override {};
    };

    class LSystemRunner : public Runner {
        std::shared_ptr<TextTurtleGraphics> textTurtleGraphics;
        std::shared_ptr<::LSystem> lsystem;
        
    public:
        LSystemRunner();
        
        virtual ~LSystemRunner() {}
        
        virtual RunnerType getType() override {return LSystem;}
        
        virtual std::string run(RunContext *runContext) override;
        
        virtual std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;

        virtual void exportFile(const std::string &fileName) override;
        virtual std::string exportsToFileType() override {return "*.png";}
    };
    
    class MarkovRunner : public Runner {
    public:
        virtual ~MarkovRunner() {}
        
        virtual RunnerType getType() override {return Markov;}
        
        virtual std::string run(RunContext *runContext) override;
        
        virtual std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;

        virtual void exportFile(const std::string &fileName) override {};
    };

    class ProgramRunner : public Runner {
        std::shared_ptr<LuaProgram> program;

    public:
        virtual ~ProgramRunner() {}
        
        virtual RunnerType getType() override {return Program;}
        
        virtual std::string run(RunContext *runContext) override;
        
        virtual std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;

        virtual void exportFile(const std::string &fileName) override;
        virtual std::string exportsToFileType() override {return "*.mov";}
        
        virtual void keyPressed(int keyCode) override;
    };

    class NamShubRunner : public Runner {
    public:
        virtual ~NamShubRunner() {}
        
        virtual RunnerType getType() override {return NamShub;}
        
        virtual std::string run(RunContext *runContext) override;
        
        virtual std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> getExamples() override;
        
        virtual DrawFunction getDrawFunction() override;
        virtual bool isAnimated() override;

        virtual void exportFile(const std::string &fileName) override {};
    };

};

#endif /* Runner_hpp */

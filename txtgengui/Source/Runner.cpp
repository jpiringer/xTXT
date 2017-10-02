//
//  Runner.cpp
//  txtgengui - App
//
//  Created by joerg piringer on 02.10.17.
//

#include "Runner.hpp"

#include "Parser.hpp"
#include "utils.hpp"

std::string automateSampleCode = R"(START = A B;
A = abc | def;
B = A | B;
)";

jp::Runner::Runner(enum RunnerType rtype)
: runnerType(rtype) {
    
}

std::string jp::Runner::getSampleCode() {
    switch (runnerType) {
        case Automate:
            return automateSampleCode;
        default:
            break;
    }
    
    return "Unknown Runner Type";
}

std::string jp::Runner::runAutomate() {
    std::wstring expression = fromUTF8(code);
    jp::Parser parser(expression);
    
    parser.parse();
    std::wstring result = parser.generate();
    
    if (parser.getErrorCount() > 0) {
        errorsHappened = true;
        result = parser.getErrors();
    }
    
    return toUTF8(result);
}

std::string jp::Runner::run() {
    switch (runnerType) {
        case Automate:
            return runAutomate();
        default:
            break;
    }
    return "Unknown Runner Type";
}

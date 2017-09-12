//
//  main.cpp
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>

#include "Parser.hpp"

#include "examples.h"

int main(int argc, const char * argv[]) {
    std::wstring expression = example1(); 
    jp::Parser parser(expression);
    
    parser.parse();
    
    std::wcout << parser.generate() << std::endl;
    
    return 0;
}

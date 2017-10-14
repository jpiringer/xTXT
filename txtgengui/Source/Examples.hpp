//
//  Examples.hpp
//  txtgen - App
//
//  Created by joerg piringer on 05.10.17.
//

#ifndef Examples_hpp
#define Examples_hpp

#include <stdio.h>

#include <string>
#include <vector>

std::vector<std::pair<std::string,std::string>> grammarExamples();
std::vector<std::pair<std::string,std::string>> LSystemExamples();
std::vector<std::pair<std::string,std::string>> markovExamples();
std::vector<std::pair<std::string,std::string>> programExamples();

#endif /* Examples_hpp */

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

void initExamples();

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> grammarExamples();
std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> LSystemExamples();
std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> markovExamples();
std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> programExamples();

#endif /* Examples_hpp */

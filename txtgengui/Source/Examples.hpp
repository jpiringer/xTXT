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

std::vector<std::pair<std::string,std::wstring>> grammarExamples();
std::vector<std::pair<std::string,std::wstring>> LSystemExamples();
std::vector<std::pair<std::string,std::wstring>> markovExamples();
std::vector<std::pair<std::string,std::wstring>> programExamples();

#endif /* Examples_hpp */

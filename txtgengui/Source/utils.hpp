//
//  utils.hpp
//  txtgengui - App
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef utils_hpp
#define utils_hpp

#include <string>

std::string toUTF8(const std::wstring &str);
std::wstring fromUTF8(const std::string &str);

#endif /* utils_hpp */

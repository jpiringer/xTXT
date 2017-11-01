//
//  utils.cpp
//  txtgengui - App
//
//  Created by joerg piringer on 02.10.17.
//

#include "utils.hpp"

#include <algorithm>
#include <codecvt>
#include <locale>
#include <string>

std::string convertToString(const std::wstring &wstr) {
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
    std::string u8str = conv1.to_bytes(wstr);
    
    return u8str;
}

std::wstring convertToWString(const std::string &str) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(str);
    
    return wide;
}

std::string toUTF8(const std::wstring &str) {
    return convertToString(str);
}

std::wstring fromUTF8(const std::string &str) {
    return convertToWString(str);
}

std::wstring trim(const std::wstring &s) {
	std::locale loc;
    auto wsfront=std::find_if_not(s.begin(),s.end(),[loc](wchar_t c){return std::isspace(c, loc);});
    return std::wstring(wsfront,std::find_if_not(s.rbegin(),std::wstring::const_reverse_iterator(wsfront),[loc](wchar_t c){return std::isspace(c, loc);}).base());
}

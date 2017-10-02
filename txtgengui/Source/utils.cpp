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

std::string convertToString(const std::wstring &wstr)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
    std::string u8str = conv1.to_bytes(wstr);
    
    return u8str;
}

std::wstring convertToWString(const std::string &str)
{
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

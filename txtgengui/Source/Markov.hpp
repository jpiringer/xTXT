//
//  Markov.hpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef Markov_hpp
#define Markov_hpp

#include <unordered_map>
#include <string>
#include <memory>

class MarkovProbTable {
    std::unordered_map<wchar_t, int> mappings;
    int allchars;
    
public:
    MarkovProbTable() {allchars = 0;}
    
    void addChar(wchar_t c);
    wchar_t generateChar();
};

class MarkovTable {
    std::unordered_map<std::wstring, std::shared_ptr<MarkovProbTable>> chains;
    size_t prefixLen;
    std::wstring source;
    
protected:
    void addPrefixWithChar(std::wstring prefix, wchar_t c);
    std::wstring generateSeed();
    
public:
    MarkovTable(size_t _prefixLen);
    ~MarkovTable();
    
    void addString(const std::wstring &str);
    std::wstring getString(size_t len);
};

#endif /* Markov_hpp */

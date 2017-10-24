//
//  Markov.cpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Markov.hpp"

void MarkovProbTable::addChar(wchar_t c) {
    if (mappings.find(c) == mappings.end()) {
        mappings[c] = 1;
    }
    else {
        mappings[c]++;
    }
    allchars++;
}

wchar_t MarkovProbTable::generateChar() {
    int val = rand() % allchars;
    int sum = 0;
    
    for (std::unordered_map<wchar_t, int>::iterator it = mappings.begin(); it != mappings.end(); it++) {
        if (val >= sum && val < sum+(*it).second) {
            return (*it).first;
        }
        sum += (*it).second;
    }
    return L' ';
}

// ====================== MARKOV

MarkovTable::MarkovTable(size_t _prefixLen)
: prefixLen(_prefixLen) {
}

MarkovTable::~MarkovTable() {
}

void MarkovTable::addPrefixWithChar(std::wstring prefix, wchar_t c) {
    if (chains.find(prefix) == chains.end()) {
        chains[prefix] = std::make_shared<MarkovProbTable>();
    }
    chains[prefix]->addChar(c);
}

void MarkovTable::addString(const std::wstring &str) {
    source = str;
    if (prefixLen > 0) {
        for (size_t pos = 0; pos < str.length(); pos++) {
            std::wstring sub = str.substr(pos, prefixLen+1);
            
            if (sub.length() == prefixLen+1) {
                addPrefixWithChar(sub.substr(0,prefixLen), sub[prefixLen]);
            }
        }
    }
}

std::wstring MarkovTable::generateSeed() {
    if (chains.size() > 0) {
        int seedNr = rand() % chains.size();
        int k = 0;
        
        for (auto chain : chains) {
            if (k == seedNr) {
                return chain.first;
            }
            k++;
        }
        
        return (*(chains.begin())).first;
    }
    else
        return L"";
}

std::wstring MarkovTable::getString(size_t len) {
    std::wstring result = L"";
    
    if (prefixLen > 0) {
        std::wstring seed = generateSeed();
        
        for (int i = 0; i < len; i++) {
            auto probtable = chains[seed];
            
            while (probtable == nullptr) {
                seed = generateSeed();
                probtable = chains[seed];
            }
            wchar_t c = probtable->generateChar();
            
            result += c;
            
            seed += c;
            seed = seed.substr(1);
        }
    }
    else {
        for (int i = 0; i < len; i++) {
            int charPos = rand() % source.length();

            result += source[charPos];
        }
    }
    
    return result;
}

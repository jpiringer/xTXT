﻿//
//  NamShub.cpp
//  txtgencmd
//
//  Created by joerg piringer on 07.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "NamShub.hpp"

#include <vector>
#include <functional>
#include <locale>
#include <algorithm>

//#include <hunspell/hunspell.hxx>

#include "utils.hpp"
#include "Platform.hpp"

#define VOWELS L"aeiouAEIOUäöüÄÖÜ"
#define CONSONANTS L"bcdfghjklmnpqrstvwxzBCDFGHJKLMNPQRSTVXWZß"
#define PLOSIVES L"cbdkptCBDKPT"
#define FRICATIVES L"fsvFSV"
#define LOOPABLE (VOWELS L"fhlmnrsvFHLMNRSV")
#define UNLOOPABLE L"bcdgjkpqtxzBCDGJKPQTXZ"
#define ALPHA (VOWELS CONSONANTS)
#define WHITESPACE L" \t\r\n"
#define NEWLINE L"\n"
#define STRETCHABLE L"aefhilmnorsuyäöüAEFHILMNORSUYÄÖÜ"
#define NORMALCHARS L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ^°!\"$%&/()\\=?+*#'´`-_.:,;€@<>äöüÄÖÜß \t\r\t"

static std::wstring vowels = std::wstring(VOWELS);
static std::wstring consonants = std::wstring(CONSONANTS);
static std::wstring plosives = std::wstring(PLOSIVES);
static std::wstring fricatives = std::wstring(FRICATIVES);
static std::wstring loopable = std::wstring(LOOPABLE);
static std::wstring unloopable = std::wstring(UNLOOPABLE);
static std::wstring alpha = std::wstring(ALPHA);
static std::wstring whitespace = std::wstring(WHITESPACE);
static std::wstring newline = std::wstring(NEWLINE);
static std::wstring stretchable = std::wstring(STRETCHABLE);
static std::wstring normalChars = std::wstring(NORMALCHARS);

std::wstring consOnly(const std::wstring &s) {
    std::wstring str(L"");
    
    for(int x = 0; x < s.length(); x++)
    {
        if(consonants.find(s[x]) != std::string::npos)
            str += s[x];
    }
    return str;
}

std::wstring vowelsOnly(const std::wstring &s) {
    std::wstring str(L"");
    
    for(int x = 0; x < s.length(); x++)
    {
        if(vowels.find(s[x]) != std::string::npos)
            str += s[x];
    }
    return str;
}

std::wstring fricativesOnly(const std::wstring &s) {
    std::wstring str(L"");
    
    for(int x = 0; x < s.length(); x++)
    {
        if(fricatives.find(s[x]) != std::string::npos)
            str += s[x];
    }
    return str;
}

std::wstring plosivesOnly(const std::wstring &s) {
    std::wstring str(L"");
    
    for(int x = 0; x < s.length(); x++)
    {
        if(plosives.find(s[x]) != std::string::npos)
            str += s[x];
    }
    return str;
}

wchar_t transformSortChar(wchar_t c) {
	switch (c) {
	case L'ä':
		c = 'a';
		break;
	case L'ö':
		c = 'o';
		break;
	case L'ü':
		c = 'u';
		break;
	case L'ß':
		c = 's';
		break;
	case L'Ä':
		c = 'A';
		break;
	case L'Ö':
		c = 'O';
		break;
	case L'Ü':
		c = 'U';
		break;
	}
	return c;
}

int sort_compare(const void *arg1, const void *arg2) {
	static std::locale locale("");
    wchar_t c1 = *(wchar_t*)arg1 , c2 = *(wchar_t*)arg2;
    
	c1 = transformSortChar(c1);
	c2 = transformSortChar(c2);
    
    c1 = std::tolower(c1, locale);
    c2 = std::tolower(c2, locale);
    
    if(c1 == c2)
        return 0;
    if(c1 < c2)
        return -1;
    return 1;
}

std::wstring rip(const std::wstring &s) {
    std::wstring str(L"");
    size_t cnt;
    size_t sz = s.length();
    wchar_t c, c2;
    
    for(cnt = 0; cnt < sz;) {
        c = (wchar_t)(s[cnt]);
        if((c != '\n') && (c != '\r') && (c != '\r'))
        {
            switch(std::rand()%7)
            {
                case 1 :
                    str += c;
                case 2:
                    c2 = s[rand()%sz];
                    if((c2 != '\n') && (c2 != '\r') && (c2 != '\r'))
                        str += c2;
                default :
                    str += c;
                    cnt++;
            }
        }
        else
        {
            str += c;
            cnt++;
        }
    }
    
    return str;
}

template <class S> S shuffle(S s) {
    size_t len = s.length();
    S str;
    S used(s);
    int rnd = 0;
    int x;
    
    for(x = 0; x < len; x++) {
        rnd = std::rand()%(used.length());
        
        str += used[rnd];
        
        used = used.erase(rnd,1); //used.Left(rnd)+used.Right(used.Len()-rnd-1);
    }
    
    return str;
}

template <class S> S &partRnd(S in, size_t maxlen) {
    static S results;
    S str;
    size_t index = 0;
    
    results = L"";
    
    for(index = 0; index < in.length(); ) {
        size_t l = std::rand() % maxlen + 1;
        
        results += in.substr(index, l) + L" ";
        index += l;
    }
    
    if(index < in.length())
        results += in.substr(index);
    
    return results;
}

template <class S> S &stretch(S in) {
    static S result;
    
    result = L"";
    
    for (size_t i = 0; i < in.length(); i++) {
        if(stretchable.find(in[i]) != std::string::npos) {
            result += in[i];
        }
        result += in[i];
    }
    
    return result;
}

template <class S> S &condense(S in) {
    static S result;
    
    result = L"";
    
    bool wasWhitespace = false;
    for (size_t i = 0; i < in.length(); i++) {
        if(whitespace.find(in[i]) != std::string::npos) {
            if (!wasWhitespace) {
                result += in[i];
            }
            wasWhitespace = true;
        }
        else {
            result += in[i];
            wasWhitespace = false;
        }
    }
    
    return result;
}

std::wstring split(const std::wstring &s) {
    std::wstring str = L"";
    for (size_t i = 0; i < s.length(); i++) {
        str += s[i];
        str += L" ";
    }
    return str;
}

template <class S> S permutations(const S &in) {
    S ret;
    S str = in;
    
    std::sort(str.begin(),str.end());
    
    do {
        ret += str;
        ret += L" ";
    }
    while(std::next_permutation(str.begin(),str.end()));
    
    return ret;
}

NamShubExecutor::NamShubExecutor() {
    /*std::string languageCode = "German";

    std::string affFile = languageCode+std::string(".aff");
    std::string dicFile = languageCode+std::string(".dic");

    hunspell = std::make_shared<Hunspell>(toDataPath(affFile).c_str(), toDataPath(dicFile).c_str());*/
}

std::wstring NamShubExecutor::suggest(const std::wstring &s) {
    std::wstring result = L"";
    /*char **slst = nullptr;
    int n = hunspell->suggest(&slst, toUTF8(s).c_str());

    
    for (int i = 0; i < n; i++) {
        auto sg = std::string(slst[i]);
        result += fromUTF8(sg);
        if (i < n-1) {
            result += L" ";
        }
    }
    
    hunspell->free_list(&slst, n);*/
    
    return result;
}

std::wstring NamShubExecutor::executeCommand(const std::string &command, std::wstring _str) {
    std::vector<std::pair<std::string, std::function<std::wstring(std::wstring)>>> commands = {
        {"dup", [](const std::wstring &str){
            return str+str;
        }},
        {"reverse", [](const std::wstring &str) {
            auto s = str;
            
            std::reverse(s.begin(), s.end());
            return s;
        }},
        {"sort", [](const std::wstring &str) {
            auto s = str;
            std::qsort((void *)s.c_str(), s.length(), sizeof(wchar_t), sort_compare);
            
            return s;
        }},
        {"rip", [](const std::wstring &str) {
            return rip(str);
        }},
        {"shuffle", [](const std::wstring &str) {
            return shuffle(str);
        }},
        {"part", [](const std::wstring &str) {
            return partRnd(str, 5);
        }},
        {"split", [](const std::wstring &str) {
            return split(str);
        }},
        {"condense", [](const std::wstring &str) {
            return condense(str);}},
        {"stretch", [](const std::wstring &str) {
            return stretch(str);}},
        {"vowels only", [](const std::wstring &str) {
            return vowelsOnly(str);}},
        {"cons only", [](const std::wstring &str) {
            return consOnly(str);}},
        {"fricatives only", [](const std::wstring &str) {
            return fricativesOnly(str);}},
        {"plosives only", [](const std::wstring &str) {
            return plosivesOnly(str);}},
        //{"suggest", [this](const std::wstring &str) {
        //    return suggest(str);}},
        {"permutate", [](const std::wstring &str) {
            if (str.length() <= 8) {
                return permutations(str);
            }
            else {
                return str;
            }
        }}
    };
                        
    for (auto p : commands) {
        if (p.first == command) {
            return p.second(_str);
        }
    }
    
    return _str;
}
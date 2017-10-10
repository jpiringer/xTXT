//
//  NamShub.hpp
//  txtgencmd
//
//  Created by joerg piringer on 07.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef NamShub_hpp
#define NamShub_hpp

#include <string>

class NamShubExecutor {
    
public:
    NamShubExecutor() {}
    
    std::wstring executeCommand(const std::string &command, std::wstring _str);
};

#endif /* NamShub_hpp */

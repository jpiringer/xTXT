//
//  txtgenlib.cpp
//  txtgenlib
//
//  Created by joerg piringer on 02.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include <iostream>
#include "txtgenlib.hpp"
#include "txtgenlibPriv.hpp"

void txtgenlib::HelloWorld(const char * s)
{
    txtgenlibPriv *theObj = new txtgenlibPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void txtgenlibPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};


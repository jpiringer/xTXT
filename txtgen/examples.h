//
//  examples.h
//  txtgen
//
//  Created by joerg piringer on 12.09.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#ifndef examples_h
#define examples_h

inline std::wstring example1() {
    return LR"(
    START = TEST1 | TEST2;
    TEST1 = abc START | def TEST1 | a b c d e TEST2 | z;
    TEST2 = xyz TEST2 | äöü TEST1 | 'string mit white' START;
    
    
    )";
}


#endif /* examples_h */

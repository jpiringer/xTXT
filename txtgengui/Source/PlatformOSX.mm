//
//  Platform.cpp
//  txtgen - App
//
//  Created by joerg piringer on 10.10.17.
//

#include "Platform.hpp"

#import <Foundation/Foundation.h>

#define CString2NSString(s) [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding]
#define CString2NSStringEncoding(s, _encoding) [NSString stringWithCString:s.c_str() encoding:(((_encoding) == MacRoman) ? NSMacOSRomanStringEncoding : NSUTF8StringEncoding)]
#define NSString2CString(s) std::string([s UTF8String])

std::string toDataPath(const std::string &p) {
    NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:CString2NSString(p)];
    
    return NSString2CString(path);
}

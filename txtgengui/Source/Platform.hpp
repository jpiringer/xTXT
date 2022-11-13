//
//  Platform.hpp
//  txtgen - App
//
//  Created by joerg piringer on 10.10.17.
//

#ifndef Platform_hpp
#define Platform_hpp

#include <string>

#if JUCE_MAC
std::string toDataPath(const std::string &p);
#else
std::wstring loadResourceFileWithID(int _id);
#endif


#endif /* Platform_hpp */

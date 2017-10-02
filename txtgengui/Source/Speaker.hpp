//
//  Speaker.hpp
//  txtgen - App
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef Speaker_hpp
#define Speaker_hpp

#include <string>

#include <memory>

namespace jp {
    class Speaker {
        
    public:
        Speaker() {}
        virtual ~Speaker() {}
        
        virtual void speak(const std::string &str) = 0;
    };
}

std::shared_ptr<jp::Speaker> createSpeakerInstance();

#endif /* Speaker_hpp */

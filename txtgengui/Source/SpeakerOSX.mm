//
//  Speaker.cpp
//  txtgen - App
//
//  Created by joerg piringer on 02.10.17.
//

#include "Speaker.hpp"

#import <AppKit/AppKit.h>

namespace jp {
    class SpeakerOSX : public Speaker {
        NSSpeechSynthesizer *speechSynthesizer = nil;
        
    public:
        SpeakerOSX() {
            speechSynthesizer = [[NSSpeechSynthesizer alloc] init];
        }
        
        ~SpeakerOSX() {
            speechSynthesizer = nil;
        }

        void speak(const std::string &str) override {
            [speechSynthesizer startSpeakingString:[NSString stringWithCString:str.c_str() encoding:NSUTF8StringEncoding]];
        }
        
        void stop() override {
            [speechSynthesizer stopSpeaking];
        }
    };
}
        
std::shared_ptr<jp::Speaker> createSpeakerInstance() {
    return std::make_shared<jp::SpeakerOSX>();
}

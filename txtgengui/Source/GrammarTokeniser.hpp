//
//  AutomateTokenizer.hpp
//  txtgen - App
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef AutomateTokenizer_hpp
#define AutomateTokenizer_hpp

#include "../JuceLibraryCode/JuceHeader.h"

class GrammarTokeniser : public juce::CodeTokeniser {
    
    
public:
    int readNextToken(juce::CodeDocument::Iterator &source) override;
    juce::CodeEditorComponent::ColourScheme getDefaultColourScheme() override;
    
    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_keyword,
        tokenType_operator,
        tokenType_identifier,
        tokenType_integer,
        tokenType_float,
        tokenType_string,
        tokenType_bracket,
        tokenType_punctuation,
        tokenType_preprocessor
    };
};

#endif /* AutomateTokenizer_hpp */

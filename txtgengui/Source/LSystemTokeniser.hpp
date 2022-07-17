//
//  LSystemTokeniserTokenizer.hpp
//  txtgen - App
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef LSystemTokeniserTokenizer_hpp
#define LSystemTokeniserTokenizer_hpp

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

class LSystemTokeniser : public CodeTokeniser {
    
    
public:
    int readNextToken(CodeDocument::Iterator &source) override;
    CodeEditorComponent::ColourScheme getDefaultColourScheme() override;
    
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

#endif /* LSystemTokeniserTokenizer_hpp */

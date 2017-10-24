//
//  TextTurtleGraphics.hpp
//  xTXT - App
//
//  Created by joerg piringer on 24.10.17.
//

#ifndef TextTurtleGraphics_hpp
#define TextTurtleGraphics_hpp

#include "ShowWindow.hpp"

#include "Runner.hpp"

#include <math.h>

class TextTurtleGraphics {
    jp::Runner *runner;
    
    std::shared_ptr<Font> displayFont;
    float fontSize = 15.f;
    float angle = 90.f/180.0f*M_PI;
    
    void processCharacter(Graphics &g, wchar_t c);
    void initState(Graphics &g, int width, int height);
    
public:
    TextTurtleGraphics(jp::Runner *r);
    
    static DrawFunction getDrawFunction(jp::Runner *r);
    
};
#endif /* TextTurtleGraphics_hpp */

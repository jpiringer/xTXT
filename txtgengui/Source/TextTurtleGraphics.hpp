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

#define _USE_MATH_DEFINES
#include <math.h>

namespace jp {
    class Runner;
    
    class GraphicsDisplay {
    public:
        virtual ~GraphicsDisplay() {}
        
        virtual void saveAsImage(jp::Runner *runner, const std::string &fileName) = 0;
        
        virtual void draw(jp::Runner *runner, Graphics &g, int width, int height, const std::wstring &str, double time) = 0;
    };
    
    class TextTurtleGraphics : public GraphicsDisplay {
        std::shared_ptr<Font> displayFont;
        std::vector<std::tuple<float, float, float, float>> stack;
        float fontSize;
        float standardAngle = 90.f/180.0f*M_PI;
        float randomAngleDeviation = 0;
        float minx, miny, maxx, maxy;
        
        float posx = 0;
        float posy = 0;
        float angle = 0;
        
        float offsx;
        float offsy;
        float scale;
        
        std::wstring content;
        
    protected:
        void processCharacter(Graphics &g, wchar_t c, bool draw = true);
        void initState(jp::Runner *r, Graphics &g, int width, int height, float _offsx = 0, float _offsy = 0, float _scale = 1);
        void checkLimits();
        
        void pushStack();
        void popStack();
        
    public:
        TextTurtleGraphics();
        
        void saveAsImage(jp::Runner *runner, const std::string &fileName) override;
        
        void draw(jp::Runner *runner, Graphics &g, int width, int height, const std::wstring &str, double time) override;
        
    };
}
#endif /* TextTurtleGraphics_hpp */

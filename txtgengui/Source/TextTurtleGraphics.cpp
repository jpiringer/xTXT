//
//  TextTurtleGraphics.cpp
//  xTXT - App
//
//  Created by joerg piringer on 24.10.17.
//

#include "TextTurtleGraphics.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

#define FONT_SIZE_INC 5.f
#define FONT_SIZE_MIN 5.f

jp::TextTurtleGraphics::TextTurtleGraphics() {
}

void jp::TextTurtleGraphics::saveAsImage(jp::Runner *runner, const std::string &fileName) {
    float imageScale = 10;
    float drawingWidth = (maxx - minx)*imageScale;
    float drawingHeight = (maxy - miny)*imageScale;
    Image image(Image::RGB, drawingWidth, drawingHeight, true);
    Graphics g(image);
    PNGImageFormat imageFormat;
    File file(fileName);
    FileOutputStream outputStream(file);
    
    // clear with white
    g.setColour(Colours::white);
    g.fillRect(0.f, 0.f, drawingWidth, drawingHeight);
    
    draw(runner, g, drawingWidth, drawingHeight, content, 0);
    
    imageFormat.writeImageToStream(image, outputStream);
}

void jp::TextTurtleGraphics::checkLimits() {
    minx = std::min(posx-fontSize, minx);
    miny = std::min(posy-fontSize, miny);
    maxx = std::max(posx+fontSize, maxx);
    maxy = std::max(posy+fontSize, maxy);
}

void jp::TextTurtleGraphics::initState(jp::Runner *runner, Graphics &g, int width, int height, float _offsx, float _offsy, float _scale) {
    standardAngle = runner->getParameter("angle")/180.0f*M_PI;
    g.setColour(Colours::black);
    
    fontSize = 15.f;
    displayFont = std::make_shared<Font>(Font::getDefaultMonospacedFontName(), fontSize, Font::plain);
    g.setFont(*displayFont);

    offsx = _offsx;
    offsy = _offsy;
    scale = _scale;
    
    minx = HUGE_VALF;
    miny = HUGE_VALF;
    maxx = -HUGE_VALF;
    maxy = -HUGE_VALF;
    
    posx = width/2;
    posy = height/2;
    angle = -M_PI/2;

    checkLimits();
}

void jp::TextTurtleGraphics::processCharacter(Graphics &g, wchar_t c, bool draw) {
    switch (c) {
        case '+':
            angle += standardAngle;
            break;
        case '-':
            angle -= standardAngle;
            break;
        case '<':
            fontSize -= FONT_SIZE_INC;
            if (fontSize < FONT_SIZE_MIN) {
                fontSize = FONT_SIZE_MIN;
            }
            break;
        case '>':
            fontSize += FONT_SIZE_INC;
            break;
        case '|':
            angle += M_PI; // turn 180°
            break;
        default: {
            if (draw) {
                const Graphics::ScopedSaveState state(g);

                wchar_t s[2] = {c, '\0'};
                String s2(s);
                
                displayFont->setHeight(fontSize);
                g.setFont(*displayFont);
                g.addTransform(AffineTransform::scale(scale));
                g.addTransform(AffineTransform::translation(posx+offsx, posy+offsy));
                g.addTransform(AffineTransform::rotation(angle+M_PI/2));
                g.drawSingleLineText(s2, 0, 0, Justification::horizontallyCentred);
            }
            posx += cos(angle) * fontSize;
            posy += sin(angle) * fontSize;
            checkLimits();
            break;
        }
    }
}

void jp::TextTurtleGraphics::draw(jp::Runner *runner, Graphics &g, int width, int height, const std::wstring &str, double time) {
    {
        const Graphics::ScopedSaveState state(g);
        
        initState(runner, g, width, height);
        for (size_t i = 0; i < str.length(); i++) {
            processCharacter(g, str[i], false);
        }
    }
    
    float drawingWidth = maxx - minx;
    float drawingHeight = maxy - miny;
    
    if (drawingWidth < 10.0f) {
        drawingWidth = width;
    }
    if (drawingHeight < 10.0f) {
        drawingHeight = height;
    }
    
    float scale = std::min((float)width / drawingWidth, (float)height / drawingHeight);
    
    {
        const Graphics::ScopedSaveState state(g);
        
        initState(runner, g, width, height, -minx+(width-drawingWidth*scale)/2/scale, -miny+(height-drawingHeight*scale)/2/scale, scale);
        for (size_t i = 0; i < str.length(); i++) {
            processCharacter(g, str[i], true);
        }
    }
    
    content = str;
}

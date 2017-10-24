//
//  TextTurtleGraphics.cpp
//  xTXT - App
//
//  Created by joerg piringer on 24.10.17.
//

#include "TextTurtleGraphics.hpp"

TextTurtleGraphics::TextTurtleGraphics(jp::Runner *r)
: runner(r) {
    displayFont = std::make_shared<Font>(Font::getDefaultMonospacedFontName(), fontSize, Font::plain);
}

void TextTurtleGraphics::initState(Graphics &g, int width, int height) {
    angle = runner->getParameter("angle")/180.0f*M_PI;
    g.setColour(Colours::black);
    g.setFont(*displayFont);

    g.addTransform(AffineTransform::translation(width/2, height/2));
}

void TextTurtleGraphics::processCharacter(Graphics &g, wchar_t c) {
    switch (c) {
        case '+':
            g.addTransform(AffineTransform::rotation(angle));
            break;
        case '-':
            g.addTransform(AffineTransform::rotation(-angle));
            break;
        default: {
            wchar_t s[2] = {c, '\0'};
            String s2(s);
            g.drawSingleLineText(s2, 0, 0, Justification::horizontallyCentred);
            g.addTransform(AffineTransform::translation(0, -fontSize));
            break;
        }
    }
}

DrawFunction TextTurtleGraphics::getDrawFunction(jp::Runner *r) {
    auto textTurtle = std::make_shared<TextTurtleGraphics>(r);

    return [textTurtle](Graphics &g, int width, int height, const std::wstring &str, double time) {
        const Graphics::ScopedSaveState state(g);

        textTurtle->initState(g, width, height);
        for (size_t i = 0; i < str.length(); i++) {
            textTurtle->processCharacter(g, str[i]);
        }
    };
}

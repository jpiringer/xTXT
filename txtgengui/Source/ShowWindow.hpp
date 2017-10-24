//
//  ShowWindow.hpp
//  xTXT - App
//
//  Created by joerg piringer on 24.10.17.
//

#ifndef ShowWindow_hpp
#define ShowWindow_hpp

#include "../JuceLibraryCode/JuceHeader.h"

#include <functional>

typedef std::function<void(Graphics &, int, int, const std::wstring &, double)> DrawFunction;

class ShowComponent : public Component {
    double lastRenderStartTime, averageTimeMs, averageActualFPS;
    Font displayFont;
    
    DrawFunction drawFunction;
    
    std::wstring content;
    
    double animationStartTime = 0;

public:
    ShowComponent(const String& name);
    
    void paint(Graphics &g) override;
    void drawShow(Graphics &g);
    
    void setDrawFunction(DrawFunction df) {drawFunction = df;}
    
    void setContent(const std::wstring &str) {content = str;}
    
    void startAnimation();
    void stopAnimation();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowComponent)
};

class ShowWindow : public DocumentWindow, private Timer {
    void timerCallback() override;
    
    ScopedPointer<ShowComponent> contentComponent;
    
    bool animated = false;

public:
    ShowWindow();
    
    ~ShowWindow();
    
    void closeButtonPressed() override;
    
    void startAnimation();
    void stopAnimation();
    
    void setDrawFunction(DrawFunction df) {contentComponent->setDrawFunction(df);}
    void update(std::wstring str);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowWindow)
};

#endif /* ShowWindow_hpp */
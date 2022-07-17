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

#define FPS 30.f

using namespace juce;

typedef std::function<void(Graphics &, int, int, const std::wstring &, double)> DrawFunction;

namespace jp {
    class Runner;
}

class ShowComponent :
public Component,
public ApplicationCommandTarget,
public KeyListener
{
    double lastRenderStartTime, averageTimeMs, averageActualFPS;
    Font displayFont;
    
    DrawFunction drawFunction;
    
    std::wstring content;
    
    double animationStartTime = 0;
    
    jp::Runner *runner;

public:
    ShowComponent(const String& name);
    ~ShowComponent();
    
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    ApplicationCommandTarget *getNextCommandTarget() override;
    bool perform(const InvocationInfo& info) override;
    
    void paint(Graphics &g) override;
    void drawShow(Graphics &g);
    
    void setDrawFunction(DrawFunction df) {drawFunction = df;}
    
    void setContent(const std::wstring &str) {content = str;}
    
    void startAnimation();
    void stopAnimation();
    
    void exportImage();
    void setRunner(jp::Runner *_runner) {runner = _runner;}
    
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowComponent)
};

class ShowWindow : public DocumentWindow, private Timer {
    void timerCallback() override;
    
    std::unique_ptr<ShowComponent> contentComponent;
    
    bool animated = false;

public:
    ShowWindow();
    
    ~ShowWindow();
    
    void closeButtonPressed() override;
    
    void startAnimation();
    void stopAnimation();
    
    void setDrawFunction(DrawFunction df) {contentComponent->setDrawFunction(df);}
    void update(jp::Runner *_runner, std::wstring str);
    
    void exportImage();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowWindow)
};

#endif /* ShowWindow_hpp */

//
//  ShowWindow.cpp
//  xTXT - App
//
//  Created by joerg piringer on 24.10.17.
//

#include "ShowWindow.hpp"

#define SHOW_FPS 1

ShowComponent::ShowComponent(const String& name)
: Component (name),
lastRenderStartTime (0),
averageTimeMs (0),
averageActualFPS (0)
{
    displayFont = Font(Font::getDefaultMonospacedFontName(), 12.0f, Font::bold);
    setSize(600, 400);
    
    drawFunction = [](Graphics &g, int w, int h, const std::wstring &str, double time) {};
}

void ShowComponent::startAnimation() {
    animationStartTime = Time::getMillisecondCounterHiRes();
}

void ShowComponent::stopAnimation() {
    
}

void ShowComponent::paint(Graphics &g) {
    double frameStartTime = Time::getMillisecondCounterHiRes();

    {
        const Graphics::ScopedSaveState state(g);
        
        drawFunction(g, getWidth(), getHeight(), content, frameStartTime - animationStartTime);
    }
    
#if SHOW_FPS
    double now = Time::getMillisecondCounterHiRes();
    double filtering = 0.08;
    
    const double elapsedMs = now - frameStartTime;
    averageTimeMs += (elapsedMs - averageTimeMs) * filtering;
    
    const double sinceLastRender = now - lastRenderStartTime;
    lastRenderStartTime = now;
    
    const double effectiveFPS = 1000.0 / averageTimeMs;
    const double actualFPS = sinceLastRender > 0 ? (1000.0 / sinceLastRender) : 0;
    averageActualFPS += (actualFPS - averageActualFPS) * filtering;
    
    GlyphArrangement ga;
    ga.addFittedText(displayFont,
                     "Elapsed Time: " + String(frameStartTime - animationStartTime, 10)
                     + " ms\n" +
                     "Time: " + String (averageTimeMs, 2)
                     + " ms\nEffective FPS: " + String (effectiveFPS, 1)
                     + "\nActual FPS: " + String (averageActualFPS, 1),
                     0, 10.0f, getWidth() - 10.0f, (float) getHeight(), Justification::topRight, 3);
    
    g.setColour(Colours::white.withAlpha (0.5f));
    g.fillRect(ga.getBoundingBox (0, ga.getNumGlyphs(), true).getSmallestIntegerContainer().expanded (4));
    
    g.setColour(Colours::black);
    ga.draw(g);
#endif
}

// =======================================================================================
// ShowWindow
// =======================================================================================

ShowWindow::ShowWindow()
: DocumentWindow ("Show", Colours::white, DocumentWindow::allButtons) {
    setContentOwned(contentComponent = new ShowComponent("Show"), true);
}

ShowWindow::~ShowWindow() {
}

void ShowWindow::closeButtonPressed() {
    delete this;
}

void ShowWindow::timerCallback() {
    contentComponent->repaint();
}

void ShowWindow::startAnimation() {
    startTimerHz(60);
    animated = true;
    contentComponent->startAnimation();
}

void ShowWindow::stopAnimation() {
    stopTimer();
    contentComponent->stopAnimation();
    animated = false;
}

void ShowWindow::update(std::wstring str) {
    contentComponent->setContent(str);
    if (!animated) {
        contentComponent->repaint();
    }
}

//
//  MainWindow.h
//  txtgen
//
//  Created by joerg piringer on 02.10.17.
//

#ifndef MainWindow_h
#define MainWindow_h

#include "../JuceLibraryCode/JuceHeader.h"

class MainContentComponent;

//==============================================================================
/*
 This class implements the desktop window that contains an instance of
 our MainContentComponent class.
 */
class MainWindow : public DocumentWindow, private AsyncUpdater {
public:
    MainWindow (String name);
    
    ~MainWindow();
    
    void closeButtonPressed() override;
        
    void handleAsyncUpdate() override;

    static ApplicationCommandManager &getApplicationCommandManager();

    /* Note: Be careful if you override any DocumentWindow methods - the base
     class uses a lot of them, so by overriding you might break its functionality.
     It's best to do all your work in your content component instead, but if
     you really have to override any DocumentWindow methods, make sure your
     subclass also calls the superclass's method.
     */
    
private:
    ScopedPointer<MainContentComponent> contentComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};


#endif /* MainWindow_h */

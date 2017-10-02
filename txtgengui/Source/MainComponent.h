/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Runner.hpp"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public MenuBarModel, private FilenameComponentListener,                        public ButtonListener, public ApplicationCommandTarget
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    enum CommandIDs
    {
        runCmd            = 0x2100,
    };

private:
    void filenameComponentChanged (FilenameComponent*) override;
    
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int menuIndex, const String& /*menuName*/) override;
    void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    void getAllCommands (Array<CommandID>& commands) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    bool perform (const InvocationInfo& info) override;
    
    void buttonClicked (Button* button) override;
    
    void run();
    
    ScopedPointer<MenuBarComponent> menuBar;

    // this is the document that the editor component is showing
    CodeDocument codeDocument;
    
    // this is a tokeniser to apply the C++ syntax highlighting
    CPlusPlusCodeTokeniser cppTokeniser;
    
    // components
    ScopedPointer<CodeEditorComponent> editor = nullptr;
    ScopedPointer<TextEditor> results = nullptr;
    ScopedPointer<TextButton> runButton = nullptr;
    FilenameComponent fileChooser;
    
    jp::Runner runner;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

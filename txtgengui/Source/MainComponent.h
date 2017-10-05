/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Runner.hpp"
#include "Speaker.hpp"
#include "AutomateTokeniser.hpp"

#include <memory>

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
    enum CommandIDs {
        runCmd = 0x2100,
        saveCmd = 0x2200,
        openCmd = 0x2300,
        newCmd = 0x2400,
        speakCmd = 0x2500,
    };

private:
    void filenameComponentChanged (FilenameComponent*) override;
    void loadFileNow(File file);
    void loadFile(File file);
    void saveFile(const File &file);
    
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int menuIndex, const String& /*menuName*/) override;
    void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    void getAllCommands (Array<CommandID>& commands) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    bool perform (const InvocationInfo& info) override;
    
    void buttonClicked (Button* button) override;
    
    // commands
    void run();
    void openFile();
    void saveFile();
    void newFile();
    void speak();
    
    ScopedPointer<MenuBarComponent> menuBar;

    // this is the document that the editor component is showing
    CodeDocument codeDocument;
    
    // this is a tokeniser to apply the C++ syntax highlighting
    AutomateTokeniser tokeniser;
    
    TextButton* addRunTypeButton(TextButton *newComp) {
        runTypeButtons.add(newComp);
        runTypeGroup.addAndMakeVisible(newComp);
        return newComp;
    }
    
    // components
    ScopedPointer<CodeEditorComponent> editor = nullptr;
    ScopedPointer<TextEditor> results = nullptr;
    ScopedPointer<TextButton> runButton = nullptr;
    ScopedPointer<TextButton> speakButton = nullptr;
    FilenameComponent filenameComponent;
    
    jp::Runner runner;
    std::shared_ptr<jp::Speaker> speaker;
    
    std::vector<std::pair<std::string, jp::RunnerType>> runTypeNames = {
        {"Grammar", jp::Grammar},
        {"L-System", jp::LSystem},
        {"Markov", jp::Markov}
    };
    Component runTypeGroup;
    OwnedArray<TextButton> runTypeButtons;
    
    void setCurrentRunnerType(jp::RunnerType rt);
    jp::RunnerType getCurrentRunnerType() {return runner.getType();}
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

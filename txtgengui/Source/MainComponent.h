/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Runner.hpp"
#include "Speaker.hpp"
#include "GrammarTokeniser.hpp"
#include "LSystemTokeniser.hpp"

#include <memory>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public MenuBarModel,
    private FilenameComponentListener,
    public ButtonListener, public ApplicationCommandTarget
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
        undoCmd = 0x3100,
        redoCmd = 0x3200,
        settingsCmd = 0x1100,
        settingsLargeFontCmd = 0x1105
    };

private:
    void restoreSettings();
    void filenameComponentChanged (FilenameComponent*) override;
    void loadFileNow(File file);
    void loadFile(File file);
    void saveFile(const File &file);
    void showSettings();
    
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
    
    std::shared_ptr<CodeTokeniser> tokeniser;

    std::shared_ptr<TextButton> addRunTypeButton(std::shared_ptr<TextButton> newComp) {
        runTypeButtons.push_back(newComp);
        runTypeGroup.addAndMakeVisible(*newComp);
        return newComp;
    }
    
    // settings
    SafePointer<DialogWindow> dialogWindow;
    
    // components
    std::shared_ptr<CodeEditorComponent> editor = nullptr;
    std::shared_ptr<TextEditor> results = nullptr;
    std::shared_ptr<TextButton> runButton = nullptr;
    std::shared_ptr<TextButton> speakButton = nullptr;
    std::shared_ptr<TextButton> stopSpeakButton = nullptr;
    FilenameComponent filenameComponent;
    
    // specific parameters
    std::shared_ptr<Label> markovPrefixLabel;
    std::shared_ptr<Label> markovTextLabel;
    std::shared_ptr<Slider> markovPrefixLen = nullptr;
    std::shared_ptr<Slider> markovTextLen = nullptr;
    std::vector<std::shared_ptr<TextButton>> methodButtons;

    std::vector<std::shared_ptr<Component>> parameterComponents;
    void makeParametersVisible();
    void addParameterComponent(std::shared_ptr<Component> c);
    void changeCodeEditor();

    jp::Runner runner;
    std::shared_ptr<jp::Speaker> speaker;
    
    std::vector<std::pair<std::string, jp::RunnerType>> runTypeNames = {
        {"Markov", jp::Markov},
        {"Methods", jp::NamShub},
        {"L-System", jp::LSystem},
        {"Grammar", jp::Grammar},
        {"Program", jp::Program}
    };
    Component runTypeGroup;
    std::vector<std::shared_ptr<TextButton>> runTypeButtons;
    
    void setCurrentRunnerType(jp::RunnerType rt);
    jp::RunnerType getCurrentRunnerType() {return runner.getType();}
    
    UndoManager undoManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "Common.h"

#include "../JuceLibraryCode/JuceHeader.h"

#include "Runner.hpp"
#include "Speaker.hpp"
#include "GrammarTokeniser.hpp"
#include "LSystemTokeniser.hpp"

#include "ShowWindow.hpp"

#include <memory>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent :
    public Component,
    public MenuBarModel,
    private FilenameComponentListener,
    public Button::Listener,
    public Slider::Listener,
    public ComboBox::Listener,
    public ApplicationCommandTarget,
    public jp::RunContext
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    bool hasUnsavedChanges();
    
    //==============================================================================
    enum CommandIDs {
        runCmd = 0x2100,
        saveCmd = 0x2200,
        exportCmd = 0x2210,
        openCmd = 0x2300,
        newCmd = 0x2400,
        speakCmd = 0x2500,
        showCmd = 0x2505,
        undoCmd = 0x3100,
        redoCmd = 0x3200,
        convertToProgramCmd = 0x4100,
        convertToTraceryGrammarCmd = 0x4200,
        settingsCmd = 0x1100,
        settingsLargeFontCmd = 0x1105,
        aboutCmd = 0x10101,
        websiteCmd = 0x10102,
        docGrammarCmd = 0x10103,
        docProgramCmd = 0x10104
   };

private:
    void restoreSettings();
    void filenameComponentChanged(FilenameComponent *) override;
    void loadFileNow(File file);
    void loadFile(File file);
    void saveFile(const File &file);
    void exportFile(const File &file);
    void showSettings();
    
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex(int menuIndex, const String& /*menuName*/) override;
    void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    void getAllCommands (Array<CommandID>& commands) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    bool perform (const InvocationInfo& info) override;
    
    // run context
    virtual void changeOutput(const std::string &str) override;
    virtual void setErrors(const std::string &str) override;
    virtual void changeShowSize(float width, float height) override;
    virtual void speak(std::string str) override;
    
    // listeners
    void buttonClicked (Button* button) override;
    void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;
    void sliderValueChanged(Slider *slider) override;
    
    // commands
    void run();
    void openFile();
    void saveFile();
    void exportText();
    void newFile();
    void speak();
    void convertToProgram();
    void convertToTraceryGrammar();
    void showAbout();
    void showWebsite();
    void showDocGrammar();
    void showDocProgram();
    void show(int width = -1, int height = -1);
    void setShowSize(int width, int height);

    std::unique_ptr<MenuBarComponent> menuBar;
    std::shared_ptr<PopupMenu> extraAppleMenuItems = nullptr;

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
    SafePointer<ShowWindow> showWindow;

    // components
    std::shared_ptr<CodeEditorComponent> editor = nullptr;
    std::shared_ptr<TextEditor> results = nullptr;
    std::shared_ptr<TextButton> runButton = nullptr;
    std::shared_ptr<TextButton> speakButton = nullptr;
    std::shared_ptr<TextButton> stopSpeakButton = nullptr;
    std::shared_ptr<TextButton> showButton = nullptr;
    FilenameComponent filenameComponent;

    bool ignoreExampleComboBoxNotification = false;
    std::shared_ptr<ComboBox> examplesComboBox;
    
    // specific parameters
    std::shared_ptr<Label> markovPrefixLabel;
    std::shared_ptr<Label> markovTextLabel;
    std::shared_ptr<Slider> markovPrefixLen = nullptr;
    std::shared_ptr<Slider> markovTextLen = nullptr;
    std::vector<std::shared_ptr<TextButton>> methodButtons;
    
    std::shared_ptr<Label> lsystemAngleLabel;
    std::shared_ptr<Slider> lsystemAngle = nullptr;
    std::shared_ptr<Label> lsystemAngleDeviationLabel;
    std::shared_ptr<Slider> lsystemAngleDeviation = nullptr;

    std::vector<std::shared_ptr<Component>> parameterComponents;
    void makeParametersVisible();
    void addParameterComponent(std::shared_ptr<Component> c);
    void changeCodeEditor();
    void parseParameters(const std::vector<std::wstring> parameters);

    std::shared_ptr<jp::Runner> runner;
    std::shared_ptr<jp::Speaker> speaker;
    
    std::unique_ptr<FileChooser> fileChooser;
    
    std::vector<std::pair<std::string, jp::RunnerType>> runTypeNames = {
        {"Markov", jp::Markov},
        {"Methods", jp::NamShub},
        {"L-System", jp::LSystem},
        {"Grammar", jp::Grammar}
#if PROGRAMMING_ENABLED
        ,{"Program", jp::Program}
#endif
    };
    Component runTypeGroup;
    std::vector<std::shared_ptr<TextButton>> runTypeButtons;
    
    void setCurrentRunnerType(jp::RunnerType rt);
    jp::RunnerType getCurrentRunnerType() {if (runner == nullptr) return jp::Markov; else return runner->getType();}
    void chooseExample(int exampleNr);
    
    UndoManager undoManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

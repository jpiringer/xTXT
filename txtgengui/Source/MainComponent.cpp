/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "MainWindow.h"
#include "Runner.hpp"
#include "Program.hpp"
#include "Parser.hpp"

#include "UndoableActions.hpp"

#include <iostream>
#include <fstream>

#include "utils.hpp"

#define COMPONENT_ID(i) static String i = #i

COMPONENT_ID(LARGE_FONT_COMPONENT_ID);

#define FONT_SIZE_SMALL 20
#define FONT_SIZE_LARGE 40
#define FONT_SIZE standardFontSize

#define LARGE_FONT_SETTINGS "LARGE_FONT"

#if JUCE_MAC
#define NATIVE_MENU 1
#else
#define NATIVE_MENU 0
#endif

static int standardFontSize = FONT_SIZE_SMALL;

inline Colour getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour uiColour, Colour fallback = Colour (0xff4d4d4d))
{
    if (auto* v4 = dynamic_cast<LookAndFeel_V4*> (&LookAndFeel::getDefaultLookAndFeel()))
        return v4->getCurrentColourScheme().getUIColour (uiColour);
    
    return fallback;
}

void MainContentComponent::addParameterComponent(std::shared_ptr<Component> c) {
    parameterComponents.push_back(c);
    addChildComponent(*c);
}

void MainContentComponent::restoreSettings() {
    auto settings = MainWindow::getApplicationProperties().getUserSettings();
    
    bool largeFont = settings->getBoolValue(LARGE_FONT_SETTINGS, false);
    standardFontSize = largeFont ? FONT_SIZE_LARGE : FONT_SIZE_SMALL;
}

//==============================================================================
MainContentComponent::MainContentComponent()
: filenameComponent("File", File(), true, false, false,
"*.*", String(),
               "Choose a file to open it in the editor") {
    setOpaque (true);
    
    restoreSettings();
    
#if NATIVE_MENU
    extraAppleMenuItems = std::make_shared<PopupMenu>();
    extraAppleMenuItems->addItem(aboutCmd, "About xTXT...");
    MenuBarModel::setMacMainMenu(this, extraAppleMenuItems.get());
#else
    addAndMakeVisible(menuBar = new MenuBarComponent(this));
#endif

    runButton = std::make_shared<TextButton>("Run");
    runButton->addListener(this);
    addAndMakeVisible(*runButton);

    showButton = std::make_shared<TextButton>("Show");
    showButton->addListener(this);
    addAndMakeVisible(*showButton);
    speakButton = std::make_shared<TextButton>("Speak");
    speakButton->addListener(this);
    addAndMakeVisible(*speakButton);
    stopSpeakButton = std::make_shared<TextButton>("Stop");
    stopSpeakButton->addListener(this);
    addAndMakeVisible(*stopSpeakButton);
    
    examplesComboBox = std::make_shared<ComboBox>("Examples");
    examplesComboBox->setEditableText(false);
    addAndMakeVisible(*examplesComboBox);
    examplesComboBox->addListener(this);
    
    // specific parameters
    markovPrefixLabel = std::make_shared<Label>();
    markovPrefixLabel->setText("Prefix:", dontSendNotification);
    markovTextLabel = std::make_shared<Label>();
    markovTextLabel->setText("Text Length:", dontSendNotification);
    markovPrefixLen = std::make_shared<Slider>();
    markovPrefixLen->setSliderStyle(Slider::LinearHorizontal);
    markovPrefixLen->setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
    markovPrefixLen->setRange(0, 10, 1);
    markovPrefixLen->setValue(3);
    markovTextLen = std::make_shared<Slider>();
    markovTextLen->setSliderStyle(Slider::LinearHorizontal);
    markovTextLen->setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
    markovTextLen->setRange(1, 10000, 1);
    markovTextLen->setValue(1000);
    
    addParameterComponent(markovPrefixLabel);
    addParameterComponent(markovTextLabel);
    addParameterComponent(markovPrefixLen);
    addParameterComponent(markovTextLen);
    
    lsystemAngleLabel = std::make_shared<Label>();
    lsystemAngleLabel->setText("Angle:", dontSendNotification);
    lsystemAngle = std::make_shared<Slider>();
    lsystemAngle->setSliderStyle(Slider::LinearHorizontal);
    lsystemAngle->setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
    lsystemAngle->setRange(0, 360, 1);
    lsystemAngle->setValue(60);
    lsystemAngle->addListener(this);
    
    lsystemAngleDeviationLabel = std::make_shared<Label>();
    lsystemAngleDeviationLabel->setText("Deviation:", dontSendNotification);
    lsystemAngleDeviation = std::make_shared<Slider>();
    lsystemAngleDeviation->setSliderStyle(Slider::LinearHorizontal);
    lsystemAngleDeviation->setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
    lsystemAngleDeviation->setRange(0, 1, 0.001);
    lsystemAngleDeviation->setValue(0);
    lsystemAngleDeviation->addListener(this);
    
    addParameterComponent(lsystemAngleLabel);
    addParameterComponent(lsystemAngle);
    addParameterComponent(lsystemAngleDeviationLabel);
    addParameterComponent(lsystemAngleDeviation);

    std::vector<std::string> methods = {
        "dup", "reverse", "sort", "rip", "shuffle",
        "part", "split", "word lines", "condense",
        "stretch", "vowels only", "cons only",
        "fricatives only", "plosives only", //"suggest",
        "permutate"
#if PROGRAMMING_ENABLED
        ,"convert to program"
#endif
    };
    
    for (auto s : methods) {
        std::shared_ptr<TextButton> b = std::make_shared<TextButton>(s);
        
        methodButtons.push_back(b);
        addParameterComponent(b);
        b->addListener(this);
    }

    // Create a file chooser control to load files into it..
    addAndMakeVisible(filenameComponent);
    filenameComponent.addListener(this);
    
    Font font(Font::getDefaultMonospacedFontName(), FONT_SIZE, Font::plain);
    results = std::make_shared<TextEditor>();
    addAndMakeVisible(*results);
    results->setFont(font);
    results->setMultiLine(true);
    results->setReturnKeyStartsNewLine(true);
    results->setColour(TextEditor::backgroundColourId, Colour(0xff, 0xff, 0xff));
    results->setColour(TextEditor::textColourId, Colour(0, 0, 0));

    getLookAndFeel().setUsingNativeAlertWindows(true);
   
    setSize(800, 700);

    speaker = createSpeakerInstance();
    
    for (int i = 0; i < runTypeNames.size(); ++i) {
        auto tb = addRunTypeButton(std::make_shared<TextButton>(runTypeNames[i].first));
        
        tb->setClickingTogglesState(true);
        tb->setRadioGroupId(123456);
        tb->setColour(TextButton::textColourOffId, Colours::black);
        tb->setColour(TextButton::textColourOnId, Colours::black);
        tb->setColour(TextButton::buttonColourId, Colours::white);
        tb->setColour(TextButton::buttonOnColourId, Colours::blueviolet.brighter());
        
        tb->addListener(this);

        tb->setBounds(5+i * 80, 6, 80, 24);
        tb->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0)
                              | ((i != runTypeNames.size()-1) ? Button::ConnectedOnRight : 0));
        
        if (runTypeNames[i].second == getCurrentRunnerType()) {
            tb->setToggleState(true, dontSendNotification);
        }
    }
    addAndMakeVisible(runTypeGroup);
    
    editor->loadContent("");
    setCurrentRunnerType(getCurrentRunnerType());
    
    setApplicationCommandManagerToWatch(&MainWindow::getApplicationCommandManager());
}

MainContentComponent::~MainContentComponent() {
    dialogWindow->exitModalState(0);
    dialogWindow.deleteAndZero();
    
    showWindow.deleteAndZero();
    
    PopupMenu::dismissAllActiveMenus();

    filenameComponent.removeListener(this);
#if NATIVE_MENU
    MenuBarModel::setMacMainMenu(nullptr);
#endif
}

void MainContentComponent::paint (Graphics& g) {
    g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground,
                                       Colours::white));
}

void MainContentComponent::makeParametersVisible() {
    for (auto c : parameterComponents) {
        c->setVisible(false);
    }
    
    switch (getCurrentRunnerType()) {
        case jp::Markov:
            markovPrefixLen->setVisible(true);
            markovTextLen->setVisible(true);
            markovPrefixLabel->setVisible(true);
            markovTextLabel->setVisible(true);
            break;
        case jp::LSystem:
            lsystemAngleLabel->setVisible(true);
            lsystemAngle->setVisible(true);
            lsystemAngleDeviationLabel->setVisible(true);
            lsystemAngleDeviation->setVisible(true);
            break;
        case jp::NamShub:
            for (auto b : methodButtons) {
                b->setVisible(true);
            }
        default:
            break;
    }
}

void MainContentComponent::changeCodeEditor() {
    if (editor != nullptr) {
        removeChildComponent(&*editor);
        editor = nullptr;
    }
    tokeniser = nullptr;
    switch (getCurrentRunnerType()) {
        case jp::Markov:
        case jp::NamShub:
            editor = std::make_shared<CodeEditorComponent>(codeDocument, nullptr);
            break;
        case jp::LSystem:
            tokeniser = std::make_shared<LSystemTokeniser>();
            editor = std::make_shared<CodeEditorComponent>(codeDocument, &*tokeniser);
            break;
        case jp::Grammar:
            tokeniser = std::make_shared<GrammarTokeniser>();
            editor = std::make_shared<CodeEditorComponent>(codeDocument, &*tokeniser);
            break;
        case jp::Program:
            tokeniser = std::make_shared<LuaTokeniser>();
            editor = std::make_shared<CodeEditorComponent>(codeDocument, &*tokeniser);
            break;
    }
    // Create the editor..
    addAndMakeVisible(*editor);
    Font font(Font::getDefaultMonospacedFontName(), FONT_SIZE, Font::plain);
    editor->setFont(font);
    editor->setColour(CodeEditorComponent::backgroundColourId, Colour(0xff, 0xff, 0xff));
    editor->setColour(CodeEditorComponent::lineNumberBackgroundId, Colour(0, 0, 0));
    editor->setColour(CodeEditorComponent::defaultTextColourId, Colour(0, 0, 0));
}

void MainContentComponent::resized() {
    bool showCode = true;
    Rectangle<int> r(getLocalBounds());
    
    // menu
#if !NATIVE_MENU
    menuBar->setBounds(r.removeFromTop(LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
#endif

    // top
    filenameComponent.setBounds(r.removeFromTop(25));
    Rectangle<int> runTypeArea(r.removeFromTop(30));
    int runTypeAreaWidth = runTypeArea.getWidth();
    runTypeGroup.setBounds(runTypeArea.removeFromLeft(runTypeAreaWidth/3*2));
    
    if (getCurrentRunnerType() == jp::NamShub) {
        examplesComboBox->setVisible(false);
    }
    else {
        examplesComboBox->setVisible(true);
        examplesComboBox->setBounds(runTypeArea.removeFromRight(runTypeAreaWidth/3));
    }

    makeParametersVisible();
    changeCodeEditor();
    
    switch (getCurrentRunnerType()) {
        case jp::Markov: {
            Rectangle<int> markovArea(r.removeFromTop(25));
            int partition = markovArea.getWidth()/8;
            markovPrefixLabel->setBounds(markovArea.removeFromLeft(partition));
            markovPrefixLen->setBounds(markovArea.removeFromLeft(partition*3));
            markovTextLabel->setBounds(markovArea.removeFromLeft(partition));
            markovTextLen->setBounds(markovArea.removeFromLeft(partition*3));
            break;
        }
        case jp::LSystem: {
            Rectangle<int> lsystemArea(r.removeFromTop(25));
            int partition = lsystemArea.getWidth()/8;
            lsystemAngleLabel->setBounds(lsystemArea.removeFromLeft(partition));
            lsystemAngle->setBounds(lsystemArea.removeFromLeft(partition*3));
            lsystemAngleDeviationLabel->setBounds(lsystemArea.removeFromLeft(partition));
            lsystemAngleDeviation->setBounds(lsystemArea.removeFromLeft(partition*3));
            break;
        }
        case jp::NamShub: {
            int counter = 0;
            Rectangle<int> methodLineArea;
            for (auto b : methodButtons) {
                if (counter % 5 == 0) {
                    methodLineArea = r.removeFromTop(25);
                }
                b->setBounds(methodLineArea.removeFromLeft(r.getWidth()/5));
                counter++;
            }
            showCode = false;
            break;
        }
        default:
            break;
    }
    
    // speak
    Rectangle<int> speakArea(r.removeFromBottom(25));
    int bw = speakArea.getWidth()/3;
    showButton->setBounds(speakArea.removeFromLeft(bw));
    speakButton->setBounds(speakArea.removeFromLeft(bw));
    stopSpeakButton->setBounds(speakArea);
    
    // results & run
    if (showCode) {
        results->setBounds(r.removeFromBottom(r.getHeight()/2));
        runButton->setBounds(r.removeFromBottom(25));
        editor->setVisible(true);
        editor->setBounds(r.withTrimmedTop(8));
    }
    else {
        results->setBounds(r.withTrimmedTop(8));
        editor->setVisible(false);
    }
}

void MainContentComponent::loadFileNow(File file) {
    editor->loadContent(file.loadFileAsString());
    filenameComponent.setCurrentFile(file, true);
}

void MainContentComponent::loadFile(File file) {
    if (hasUnsavedChanges()) {
        AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                     "Unsaved Changes",
                                     "You have unsaved changes. Do you want to discard them?",
                                     "Cancel",
                                     "Discard",
                                     0,
                                     ModalCallbackFunction::create([this, file](int result) {
            if (result == 0) { // ok
                loadFileNow(file);
            }
        }));
    }
    else {
        loadFileNow(file);
    }
}

void MainContentComponent::saveFile(const File &file) {
    file.replaceWithText(editor->getDocument().getAllContent());
    editor->getDocument().setSavePoint();
    filenameComponent.setCurrentFile(file, true);
}

void MainContentComponent::exportFile(const File &file) {
    file.replaceWithText(results->getText());
    filenameComponent.setCurrentFile(file, true);
}

void MainContentComponent::filenameComponentChanged(FilenameComponent *) {
    File file = filenameComponent.getCurrentFile();
    loadFile(file);
}

StringArray MainContentComponent::getMenuBarNames() {
    //const char *const names[] = { "File", "Edit", "Help", nullptr };

#if PROGRAMMING_ENABLED
    return { "File", "Edit", "Program", "Help"};
#else
    return { "File", "Edit", "Help"};
#endif
}

PopupMenu MainContentComponent::getMenuForIndex(int menuIndex, const String &menuName) {
    ApplicationCommandManager *commandManager = &MainWindow::getApplicationCommandManager();
    
    PopupMenu menu;
    
    if (menuIndex == 0) { // File
        menu.addCommandItem(commandManager, MainContentComponent::newCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, MainContentComponent::openCmd);
        menu.addCommandItem(commandManager, MainContentComponent::saveCmd);
        menu.addCommandItem(commandManager, MainContentComponent::exportCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, MainContentComponent::runCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, MainContentComponent::settingsCmd);
    }
    else if (menuIndex == 1) { // Edit
        menu.addCommandItem(commandManager, MainContentComponent::undoCmd);
        menu.addCommandItem(commandManager, MainContentComponent::redoCmd);
    }
#if PROGRAMMING_ENABLED
    else if (menuIndex == 2) { // Program
        menu.addCommandItem(commandManager, MainContentComponent::convertToProgramCmd);
        menu.addCommandItem(commandManager, MainContentComponent::convertToTraceryGrammarCmd);
    }
    else if (menuIndex == 3) { // Help
#else
    else if (menuIndex == 2) { // Help
#endif
#if !NATIVE_MENU
        menu.addCommandItem(commandManager, MainContentComponent::aboutCmd);
#endif
        menu.addCommandItem(commandManager, MainContentComponent::websiteCmd);
        //menu.addCommandItem(commandManager, MainContentComponent::docGrammarCmd);
        //menu.addCommandItem(commandManager, MainContentComponent::docProgramCmd);
    }
    
    return menu;
}

void MainContentComponent::menuItemSelected (int menuItemID, int topLevelMenuIndex) {
    // most of our menu items are invoked automatically as commands, but we can handle the
    // other special cases here..
    
    if (topLevelMenuIndex == -1 && menuItemID == aboutCmd) {
        showAbout();
    }
}

void MainContentComponent::getAllCommands(Array<CommandID>& commands) {
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        MainContentComponent::runCmd,
        MainContentComponent::saveCmd,
        MainContentComponent::exportCmd,
        MainContentComponent::openCmd,
        MainContentComponent::newCmd,
        MainContentComponent::speakCmd,
        MainContentComponent::showCmd,
        MainContentComponent::undoCmd,
        MainContentComponent::redoCmd,
        MainContentComponent::settingsCmd,
        MainContentComponent::aboutCmd,
        MainContentComponent::websiteCmd,
        MainContentComponent::docGrammarCmd,
        MainContentComponent::docProgramCmd,
        MainContentComponent::convertToProgramCmd,
        MainContentComponent::convertToTraceryGrammarCmd
    };
    
    commands.addArray(ids, numElementsInArray(ids));
}

void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) {
    const String generalCategory("General");
    const String codeCategory("Code");
    const String programCategory("Program");
    const String editCategory("Edit");
    const String appleCategory("Apple");
    const String helpCategory("Help");

    switch (commandID) {
        case MainContentComponent::runCmd:
            result.setInfo("Run", "Run the code", codeCategory, 0);
            result.addDefaultKeypress('r', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::saveCmd:
            result.setInfo("Save Program...", "Save the lsystem/grammar/program code", codeCategory, 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            switch (getCurrentRunnerType()) {
                case jp::Markov:
                case jp::NamShub:
                    result.setActive(false);
                    break;
                default:
                    result.setActive(true);
                    break;
            }
            break;
        case MainContentComponent::exportCmd:
            result.setInfo("Export Text...", "Export image or text or video", codeCategory, 0);
            result.addDefaultKeypress('e', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::openCmd:
            result.setInfo("Open...", "Open code file", codeCategory, 0);
            result.addDefaultKeypress('o', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::newCmd:
            result.setInfo("New", "New code file", codeCategory, 0);
            result.addDefaultKeypress('n', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::speakCmd:
            result.setInfo("Speak", "Speak results", codeCategory, 0);
            result.addDefaultKeypress('t', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::showCmd:
            result.setInfo("Show", "Show results", codeCategory, 0);
            result.addDefaultKeypress('d', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::undoCmd:
            result.setInfo("Undo", "Undo result changes", editCategory, 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::redoCmd:
            result.setInfo("Redo", "Redo result changes", editCategory, 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier|ModifierKeys::shiftModifier);
            break;
        case MainContentComponent::convertToProgramCmd:
            result.setInfo("Convert To Program", "Convert To Program", programCategory, 0);
            break;
        case MainContentComponent::convertToTraceryGrammarCmd:
            result.setInfo("Convert To Tracery Grammar", "Convert To Tracery Grammar", programCategory, 0);
            break;
        case MainContentComponent::settingsCmd:
            result.setInfo("Settings...", "Show Settings", generalCategory, 0);
            result.addDefaultKeypress(',', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::aboutCmd:
            result.setInfo("About xTXT...", "Show About", appleCategory, 0);
            break;
        case MainContentComponent::websiteCmd:
            result.setInfo("More info...", "Show joerg piringer's website", helpCategory, 0);
            break;
        case MainContentComponent::docGrammarCmd:
            result.setInfo("Show Grammar Document...", "Show a help document for the grammar mode", helpCategory, 0);
            break;
        case MainContentComponent::docProgramCmd:
            result.setInfo("Show Program Document...", "Show a help document for the program mode", helpCategory, 0);
            break;
    }
}

ApplicationCommandTarget *MainContentComponent::getNextCommandTarget() {
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

bool MainContentComponent::perform(const InvocationInfo& info) {
    switch (info.commandID) {
        case MainContentComponent::runCmd:
            run();
            break;
        case MainContentComponent::saveCmd:
            saveFile();
            break;
        case MainContentComponent::exportCmd:
            if (showWindow == nullptr) {
                exportText();
            }
            else {
                showWindow->exportImage();
            }
            break;
        case MainContentComponent::openCmd:
            openFile();
            break;
        case MainContentComponent::newCmd:
            newFile();
            break;
        case MainContentComponent::speakCmd:
            speak();
            break;
        case MainContentComponent::showCmd:
            show();
            break;
        case MainContentComponent::undoCmd:
            undoManager.undo();
            break;
        case MainContentComponent::redoCmd:
            undoManager.redo();
            break;
        case MainContentComponent::convertToProgramCmd:
            convertToProgram();
            break;
        case MainContentComponent::convertToTraceryGrammarCmd:
            convertToTraceryGrammar();
            break;
        case MainContentComponent::settingsCmd:
            showSettings();
            break;
        case MainContentComponent::aboutCmd:
            showAbout();
            break;
        case MainContentComponent::websiteCmd:
            showWebsite();
            break;
        case MainContentComponent::docGrammarCmd:
            showDocGrammar();
            break;
        case MainContentComponent::docProgramCmd:
            showDocProgram();
            break;
        default:
            return false;
    }
    return true;
}

void MainContentComponent::openFile() {
    fileChooser = std::make_unique<FileChooser>("Open File", File(), "*.xtxt");
    
    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
     
    fileChooser->launchAsync(folderChooserFlags, [this] (const FileChooser& chooser) {
        auto file = chooser.getResult();
        
        if (file.getFullPathName() != "") {
            loadFile(file);
        }
    });
}

void MainContentComponent::saveFile() {
    fileChooser = std::make_unique<FileChooser>("Save LSystem/Grammar/Program File", File(), "*.xtxt");
        
    auto folderChooserFlags = FileBrowserComponent::saveMode;
     
    fileChooser->launchAsync(folderChooserFlags, [this] (const FileChooser& chooser) {
        auto file = chooser.getResult();
        
        if (file.getFullPathName() != "") {
            saveFile(file);
        }
    });
}
    
void MainContentComponent::exportText() {
    fileChooser = std::make_unique<FileChooser>("Export Text", File(), "*.txt");
        
    auto folderChooserFlags = FileBrowserComponent::saveMode;
     
    fileChooser->launchAsync(folderChooserFlags, [this] (const FileChooser& chooser) {
        auto file = chooser.getResult();
        
        if (file.getFullPathName() != "") {
            exportFile(file);
        }
    });
}

void MainContentComponent::newFile() {
    if (hasUnsavedChanges()) {
        AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                     "Unsaved Changes",
                                     "You have unsaved changes. Do you want to discard them?",
                                     "Cancel",
                                     "Discard",
                                     0,
                                     ModalCallbackFunction::create([this](int result) {
            if (result == 0) { // ok
                editor->loadContent("");
            }
        }));
    }
    else {
        editor->loadContent("");
    }
}

#define SETTINGS_WIDTH 300
#define SETTINGS_HEIGHT 200
void MainContentComponent::showSettings() {
    DialogWindow::LaunchOptions options;
    
    ToggleButton *toggle = new ToggleButton("Large font");
    toggle->setComponentID(LARGE_FONT_COMPONENT_ID);
    toggle->setToggleState(standardFontSize == FONT_SIZE_LARGE, dontSendNotification);
    toggle->setColour(ToggleButton::textColourId, Colours::black);
    toggle->setColour(ToggleButton::tickColourId, Colours::black);
    toggle->setColour(ToggleButton::tickDisabledColourId, Colours::darkgrey);
    toggle->addListener(this);
    options.content.setOwned(toggle);
    
    Rectangle<int> area(0, 0, SETTINGS_WIDTH, SETTINGS_HEIGHT);
    
    options.content->setSize(area.getWidth(), area.getHeight());
    
    options.dialogTitle                   = "Settings";
    options.dialogBackgroundColour        = Colour(0xff, 0xff, 0xff);
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = true;
    options.resizable                     = false;
    
    dialogWindow = options.launchAsync();
    
    if (dialogWindow != nullptr) {
        dialogWindow->centreWithSize(SETTINGS_WIDTH, SETTINGS_HEIGHT);
    }
}

void MainContentComponent::changeOutput(const std::string &str) {
    MessageManager::callAsync([this, str] {
        results->setColour(TextEditor::textColourId, Colour(0, 0, 0));

        results->setText(str);
    });
}

void MainContentComponent::setErrors(const std::string &str) {
    MessageManager::callAsync([this, str] {
        results->setColour(TextEditor::textColourId, Colour(0xff, 0, 0));
        
        results->setText(str);
    });
}

void MainContentComponent::changeShowSize(float width, float height) {
    MessageManager::callAsync([this, width, height] {
        setShowSize(width, height);
    });
}

void MainContentComponent::speak(std::string str) {
    MessageManager::callAsync([this, str] {
        speaker->speak(str);
    });
}

void MainContentComponent::run() {
    auto content = editor->getDocument().getAllContent().toStdString();
    std::string text;
    Range<int> selection = results->getHighlightedRegion();
    
    if (selection.isEmpty()) {
        text = results->getText().toStdString();
    }
    else {
        text = results->getHighlightedText().toStdString();
    }
    
    switch (getCurrentRunnerType()) {
        case jp::Markov:
            runner->setParameter("prefixLen", markovPrefixLen->getValue());
            runner->setParameter("textLen", markovTextLen->getValue());
            break;
        case jp::LSystem:
            runner->setParameter("angle", lsystemAngle->getValue());
            runner->setParameter("angleDeviation", lsystemAngleDeviation->getValue());
        default:
            break;
    }

    runner->resetErrors();
    runner->setCode(content);
    runner->setText(text);

    auto runnerResults = runner->run(this);
    if (runner->hasErrors()) {
        results->setColour(TextEditor::textColourId, Colour(0xff, 0, 0));
    }
    else {
        results->setColour(TextEditor::textColourId, Colour(0, 0, 0));
    }
    
    // undo management
    auto oldText = results->getText().toStdString();
    undoManager.beginNewTransaction();
    undoManager.perform(new UndoableModifyResultsAction(results, oldText));

    if (selection.isEmpty()) {
        results->setText(runnerResults);
    }
    else {
        auto firstFragment = results->getText().substring(0, selection.getStart());
        auto secondFragment = results->getText().substring(selection.getStart()+selection.getLength());
        results->setText(firstFragment+runnerResults+secondFragment);
        results->setHighlightedRegion(Range<int>(selection.getStart(), (int)selection.getStart()+(int)runnerResults.length()));
    }
    
    if (showWindow != nullptr) {
        showWindow->update(runner.get(), fromUTF8(results->getText().toStdString()));
    }
}

void MainContentComponent::speak() {
    auto content = results->getText();
    
    speaker->speak(content.toStdString());
}
    
void MainContentComponent::convertToProgram() {

    switch (runner->getType()) {
        case jp::NamShub: {
            auto content = fromUTF8(results->getText().toStdString());
            results->setText(toUTF8(LuaProgram::convertToProgram(L"", content, jp::NamShub)));
        }
        break;
        case jp::Grammar: {
            auto code = fromUTF8(editor->getDocument().getAllContent().toStdString());
            results->setText(toUTF8(LuaProgram::convertToProgram(code, L"", jp::Grammar)));
        }
        break;
        case jp::LSystem: {
            auto content = fromUTF8(results->getText().toStdString());
            auto code = fromUTF8(editor->getDocument().getAllContent().toStdString());
            results->setText(toUTF8(LuaProgram::convertToProgram(code, content, jp::LSystem)));
        }
        break;
        default:
            break;
    }
}
    
void MainContentComponent::convertToTraceryGrammar() {
    switch (runner->getType()) {
        case jp::Grammar: {
            auto code = fromUTF8(editor->getDocument().getAllContent().toStdString());
            results->setText(toUTF8(jp::Parser::convertToTraceryGramnmar(code, jp::Grammar)));
        }
        break;
        case jp::LSystem: {
            /*auto content = fromUTF8(results->getText().toStdString());
            auto code = fromUTF8(editor->getDocument().getAllContent().toStdString());
            results->setText(toUTF8(LuaProgram::convertToProgram(code, content, jp::LSystem)));*/
        }
        break;
        default:
            break;
    }
}

inline Colour getRandomColour (float brightness) {
    return Colour::fromHSV (Random::getSystemRandom().nextFloat(), 0.5f, brightness, 1.0f);
}

inline Colour getRandomBrightColour()   { return getRandomColour (0.8f); }

void MainContentComponent::setShowSize(int width, int height) {
    if (showWindow == nullptr) {
        show(width, height);
    }
    else {
        const bool native = true;
        Rectangle<int> area(0, 0, width, height);
        
        RectanglePlacement placement((native ? RectanglePlacement::xLeft
                                      : RectanglePlacement::xRight)
                                     | RectanglePlacement::yTop
                                     | RectanglePlacement::doNotResize);
        
        if (Desktop::getInstance().getDisplays().getPrimaryDisplay() != nullptr) {
            Rectangle<int> result(placement.appliedTo(area, Desktop::getInstance().getDisplays()
                                                      .getPrimaryDisplay()->userArea.reduced(20)));
            showWindow->setBounds(result);
        }
    }
}

void MainContentComponent::show(int width, int height) {
    const bool native = true;
    auto code = editor->getDocument().getAllContent().toStdString();

    if (showWindow != nullptr) {
        showWindow->stopAnimation();
        showWindow.deleteAndZero();
    }
    
    showWindow = new ShowWindow();
    
    if (width < 0 || height < 0) {
        setShowSize(600, 400);
    }
    else {
        setShowSize(width, height);
    }
    
    showWindow->setResizable(true, !native);
    showWindow->setUsingNativeTitleBar(native);
    showWindow->setVisible(true);
    
    runner->setParameter("angle", lsystemAngle->getValue());
    runner->setParameter("angleDeviation", lsystemAngleDeviation->getValue());
    showWindow->setDrawFunction(runner->getDrawFunction());
    runner->setCode(code);
    showWindow->update(runner.get(), fromUTF8(results->getText().toStdString()));

    if (runner->isAnimated()) {
        showWindow->startAnimation();
    }
}

void MainContentComponent::setCurrentRunnerType(jp::RunnerType rt) {
	if (showWindow != nullptr) {
		deleteAndZero(showWindow);
	}

    switch (rt) {
        case jp::Grammar:
            runner = std::make_shared<jp::AutomateRunner>();
            break;
        case jp::LSystem:
            runner = std::make_shared<jp::LSystemRunner>();
            break;
        case jp::Markov:
            runner = std::make_shared<jp::MarkovRunner>();
            break;
        case jp::Program:
            runner = std::make_shared<jp::ProgramRunner>();
            break;
        case jp::NamShub:
            runner = std::make_shared<jp::NamShubRunner>();
            break;
            
        default:
            break;
    }
    auto examples = runner->getExamples();
    
    int exampleID = 1;
    examplesComboBox->clear();
    for (auto tuple : examples) {
        examplesComboBox->addItem(std::get<0>(tuple), exampleID++);
    }
    ignoreExampleComboBoxNotification = true;
    examplesComboBox->setSelectedId(1);
    
    chooseExample(0);

    resized();
}

void MainContentComponent::parseParameters(const std::vector<std::wstring> parameters) {
    for (std::wstring parameter : parameters) {
        std::wstring trimmed = trim(parameter);
        
        size_t colonPos = trimmed.find_first_of(':');
        
        if (colonPos != std::string::npos) {
            std::wstring head = trimmed.substr(0,colonPos);
            head = trim(head);
            
            std::wstring tail = trimmed.substr(colonPos+1);
            tail = trim(tail);
            
            if (head == L"axiom") {
                std::wcout << "axiom: " << tail << std::endl;
                results->setText(toUTF8(tail));
            }
            else if (head == L"angle") {
                std::wcout << "angle: " << tail << std::endl;
                float angle = std::atof(toUTF8(tail).c_str());
                lsystemAngle->setValue(angle);
            }
        }
    }
}

void MainContentComponent::chooseExample(int exampleNr) {
    auto examples = runner->getExamples();

    if (examples.size() > 0) {
        if (hasUnsavedChanges()) {
            AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                         "Unsaved Changes",
                                         "You have unsaved changes. Do you want to discard them?",
                                         "Cancel",
                                         "Discard",
                                         0,
                                         ModalCallbackFunction::create([this, exampleNr, examples](int result) {
                if (result == 0) { // ok
                    editor->loadContent(toUTF8(std::get<1>(examples[exampleNr])));
                    auto parameters = std::get<2>(examples[exampleNr]);
                    parseParameters(parameters);
                }
            }));
        }
        else {
            auto code = toUTF8(std::get<1>(examples[exampleNr]));
            editor->loadContent(code);
            if (runner != nullptr) {
                runner->setCode(code);
            }

            auto parameters = std::get<2>(examples[exampleNr]);
            parseParameters(parameters);
        }
    }
}

void MainContentComponent::showAbout() {
    AlertWindow::AlertIconType icon = AlertWindow::InfoIcon;
    
    AlertWindow::showMessageBoxAsync (icon,
                                      std::string("xTXT ")+ VERSION_STRING.toStdString() + std::string(" by joerg piringer"),
                                      std::string("xTXT was started by joerg piringer in 2017.\nfor more info & source code look at https://joerg.piringer.net/xTXT\n\n")+std::string(BinaryData::CHANGES_md),
                                      "OK");
}

void MainContentComponent::showWebsite() {
    Process::openDocument("https://joerg.piringer.net/xTXT", "");
}
    
void MainContentComponent::showDocGrammar() {
    Process::openDocument("https://joerg.piringer.net/xTXT", "");
}
    
void MainContentComponent::showDocProgram() {
    Process::openDocument("https://joerg.piringer.net/xTXT", "");
}

void MainContentComponent::comboBoxChanged(ComboBox *comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged->getSelectedId() > 0 && !ignoreExampleComboBoxNotification) {
        chooseExample(comboBoxThatHasChanged->getSelectedId()-1);
    }
    ignoreExampleComboBoxNotification = false;
}

void MainContentComponent::sliderValueChanged(Slider *slider) {
    if (slider == lsystemAngle.get()) {
        runner->setParameter("angle", lsystemAngle->getValue());
        if (showWindow != nullptr) {
            showWindow->update(runner.get(), fromUTF8(results->getText().toStdString()));
        }
    }
    else if (slider == lsystemAngleDeviation.get()) {
        runner->setParameter("angleDeviation", lsystemAngleDeviation->getValue());
        if (showWindow != nullptr) {
            showWindow->update(runner.get(), fromUTF8(results->getText().toStdString()));
        }
    }
}

void MainContentComponent::buttonClicked(Button *button) {
    if (button == runButton.get()) {
        run();
    }
    else if (button == showButton.get()) {
        show();
    }
    else if (button == speakButton.get()) {
        speak();
    }
    else if (button == stopSpeakButton.get()) {
        speaker->stop();
    }
    else if (button->getComponentID() == LARGE_FONT_COMPONENT_ID) {
        ToggleButton *tb = (ToggleButton *)button;
        
        if (tb->getToggleState() == true) {
            standardFontSize = FONT_SIZE_LARGE;
        }
        else {
            standardFontSize = FONT_SIZE_SMALL;
        }
        
        Font font(Font::getDefaultMonospacedFontName(), FONT_SIZE, Font::plain);
        results->setFont(font);
        
        auto settings = MainWindow::getApplicationProperties().getUserSettings();
        
        settings->setValue(LARGE_FONT_SETTINGS, tb->getToggleState());
    }
    else {
        if (button->getToggleState()) {
            for (int i = 0; i < runTypeButtons.size(); i++) {
                if (&*runTypeButtons[i] == button) {
                    setCurrentRunnerType(runTypeNames[i].second);
                    return;
                }
            }
        }
        if (getCurrentRunnerType() == jp::NamShub) {
            auto command = button->getButtonText().toStdString();
            
            runner->setStringParameter("command", command);
            run();
        }
    }
}

bool MainContentComponent::hasUnsavedChanges() {
    return editor->getDocument().hasChangedSinceSavePoint();
}

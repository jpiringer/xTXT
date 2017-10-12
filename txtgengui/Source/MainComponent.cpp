/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "MainWindow.h"
#include "Runner.hpp"

#include "UndoableActions.hpp"

#include <iostream>
#include <fstream>

#define COMPONENT_ID(i) static String i = #i

COMPONENT_ID(LARGE_FONT_COMPONENT_ID);


#define FONT_SIZE_SMALL 20
#define FONT_SIZE_LARGE 40
#define FONT_SIZE standardFontSize

#define LARGE_FONT_SETTINGS "LARGE_FONT"

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
    
    addAndMakeVisible (menuBar = new MenuBarComponent (this));
    
#if JUCE_MAC
    menuBar->setModel (nullptr);
    MenuBarModel::setMacMainMenu (this);
#endif

    runButton = std::make_shared<TextButton>("Run");
    runButton->addListener(this);
    addAndMakeVisible(*runButton);

    speakButton = std::make_shared<TextButton>("Speak");
    speakButton->addListener(this);
    addAndMakeVisible(*speakButton);
    stopSpeakButton = std::make_shared<TextButton>("Stop");
    stopSpeakButton->addListener(this);
    addAndMakeVisible(*stopSpeakButton);
    
    // specific parameters
    markovPrefixLabel = std::make_shared<Label>();
    markovPrefixLabel->setText("Prefix:", dontSendNotification);
    markovTextLabel = std::make_shared<Label>();
    markovTextLabel->setText("Text Length:", dontSendNotification);
    markovPrefixLen = std::make_shared<Slider>();
    markovPrefixLen->setSliderStyle(Slider::LinearHorizontal);
    markovPrefixLen->setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
    markovPrefixLen->setRange(1, 10, 1);
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
    
    std::vector<std::string> methods = {
        "dup", "reverse", "sort", "rip", "shuffle",
        "part", "split", "condense",
        "stretch", "vowels only", "cons only",
        "fricatives only", "plosives only", //"suggest",
        "permutate"};
    
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
    results->setColour(TextEditor::backgroundColourId, Colour(0xff, 0xff, 0xff));
    results->setColour(TextEditor::textColourId, Colour(0, 0, 0));

    getLookAndFeel().setUsingNativeAlertWindows(true);

    setSize(800, 1000);
    
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
    
    setCurrentRunnerType(getCurrentRunnerType());
}

MainContentComponent::~MainContentComponent() {
    dialogWindow->exitModalState(0);
    dialogWindow.deleteAndZero();
    
    filenameComponent.removeListener(this);
#if JUCE_MAC
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

    // top
    filenameComponent.setBounds(r.removeFromTop(25));
    runTypeGroup.setBounds(r.removeFromTop(30));
    
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
    speakButton->setBounds(speakArea.removeFromLeft(speakArea.getWidth()/2));
    stopSpeakButton->setBounds(speakArea);
    
    // results & run
    if (showCode) {
        results->setBounds(r.removeFromBottom(400));
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
    if (editor->getDocument().hasChangedSinceSavePoint()) {
        AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                     "Unsaved Changes",
                                     "You have unsaved changes. Do you want to discard them?",
                                     "Cancel",
                                     "Discard",
                                     0,
                                     ModalCallbackFunction::create([this, file](int result) {
            if (result == 1) { // ok
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

void MainContentComponent::filenameComponentChanged(FilenameComponent *) {
    File file = filenameComponent.getCurrentFile();
    loadFile(file);
}

StringArray MainContentComponent::getMenuBarNames() {
    const char *const names[] = { "File", "Edit", nullptr };
    
    return StringArray(names);
}

PopupMenu MainContentComponent::getMenuForIndex (int menuIndex, const String& menuName) {
    ApplicationCommandManager *commandManager = &MainWindow::getApplicationCommandManager();
    
    PopupMenu menu;
    
    if (menuIndex == 0) { // File
        menu.addCommandItem(commandManager, MainContentComponent::newCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, MainContentComponent::openCmd);
        menu.addCommandItem(commandManager, MainContentComponent::saveCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, MainContentComponent::runCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, MainContentComponent::settingsCmd);
   }
    else if (menuIndex == 1) { // Edit
        menu.addCommandItem(commandManager, MainContentComponent::undoCmd);
        menu.addCommandItem(commandManager, MainContentComponent::redoCmd);
    }
    /*else if (menuIndex == 1)
    {
        menu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV1);
        menu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV2);
        menu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV3);
        
        PopupMenu v4SubMenu;
        v4SubMenu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV4Dark);
        v4SubMenu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV4Midnight);
        v4SubMenu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV4Grey);
        v4SubMenu.addCommandItem (commandManager, MainAppWindow::useLookAndFeelV4Light);
        
        menu.addSubMenu ("Use LookAndFeel_V4", v4SubMenu);
        
        menu.addSeparator();
        menu.addCommandItem (commandManager, MainAppWindow::useNativeTitleBar);
        
#if JUCE_MAC
        menu.addItem (6000, "Use Native Menu Bar");
#endif
        
#if ! JUCE_LINUX
        menu.addCommandItem (commandManager, MainAppWindow::goToKioskMode);
#endif
        
        if (MainAppWindow* mainWindow = MainAppWindow::getMainAppWindow())
        {
            StringArray engines (mainWindow->getRenderingEngines());
            
            if (engines.size() > 1)
            {
                menu.addSeparator();
                
                for (int i = 0; i < engines.size(); ++i)
                    menu.addCommandItem (commandManager, MainAppWindow::renderingEngineOne + i);
            }
        }
    }
    else if (menuIndex == 2)
    {
        if (TabbedComponent* tabs = findParentComponentOfClass<TabbedComponent>())
        {
            menu.addItem (3000, "Tabs at Top",    true, tabs->getOrientation() == TabbedButtonBar::TabsAtTop);
            menu.addItem (3001, "Tabs at Bottom", true, tabs->getOrientation() == TabbedButtonBar::TabsAtBottom);
            menu.addItem (3002, "Tabs on Left",   true, tabs->getOrientation() == TabbedButtonBar::TabsAtLeft);
            menu.addItem (3003, "Tabs on Right",  true, tabs->getOrientation() == TabbedButtonBar::TabsAtRight);
        }
    }
    else if (menuIndex == 3)
    {
        return getDummyPopupMenu();
    }*/
    
    return menu;
}

void MainContentComponent::menuItemSelected (int menuItemID, int topLevelMenuIndex) {
    // most of our menu items are invoked automatically as commands, but we can handle the
    // other special cases here..
    
    if (menuItemID == 6000)
    {
#if JUCE_MAC
        if (MenuBarModel::getMacMainMenu() != nullptr)
        {
            MenuBarModel::setMacMainMenu (nullptr);
            menuBar->setModel (this);
        }
        else
        {
            menuBar->setModel (nullptr);
            MenuBarModel::setMacMainMenu (this);
        }
#endif
    }
    else if (menuItemID >= 3000 && menuItemID <= 3003)
    {
        if (TabbedComponent* tabs = findParentComponentOfClass<TabbedComponent>())
        {
            TabbedButtonBar::Orientation o = TabbedButtonBar::TabsAtTop;
            
            if (menuItemID == 3001) o = TabbedButtonBar::TabsAtBottom;
            if (menuItemID == 3002) o = TabbedButtonBar::TabsAtLeft;
            if (menuItemID == 3003) o = TabbedButtonBar::TabsAtRight;
            
            tabs->setOrientation (o);
        }
    }
    else if (menuItemID >= 12298 && menuItemID <= 12305)
    {
        //sendChangeMessage();
    }
}

void MainContentComponent::getAllCommands(Array<CommandID>& commands) {
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        MainContentComponent::runCmd,
        MainContentComponent::saveCmd,
        MainContentComponent::openCmd,
        MainContentComponent::newCmd,
        MainContentComponent::speakCmd,
        MainContentComponent::undoCmd,
        MainContentComponent::redoCmd,
        MainContentComponent::settingsCmd
    };
    
    commands.addArray (ids, numElementsInArray(ids));
}

void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) {
    const String generalCategory("General");
    const String codeCategory("Code");
    const String editCategory("Edit");

    switch (commandID) {
        case MainContentComponent::runCmd:
            result.setInfo("Run", "Run the code", codeCategory, 0);
            result.addDefaultKeypress('r', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::saveCmd:
            result.setInfo("Save", "Save the code", codeCategory, 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::openCmd:
            result.setInfo("Open", "Open code file", codeCategory, 0);
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
        case MainContentComponent::undoCmd:
            result.setInfo("Undo", "Undo result changes", editCategory, 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier);
            break;
        case MainContentComponent::redoCmd:
            result.setInfo("Redo", "Redo result changes", editCategory, 0);
            result.addDefaultKeypress('z', ModifierKeys::commandModifier|ModifierKeys::shiftModifier);
            break;
        case MainContentComponent::settingsCmd:
            result.setInfo("Settings...", "Show Settings", generalCategory, 0);
            result.addDefaultKeypress(',', ModifierKeys::commandModifier);
            break;
    }
}

ApplicationCommandTarget *MainContentComponent::getNextCommandTarget() {
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

bool MainContentComponent::perform(const InvocationInfo& info) {
    //if (auto *mainWindow = MainWindow::getMainAppWindow()) {
        switch (info.commandID) {
            case MainContentComponent::runCmd:
                run();
                break;
            case MainContentComponent::saveCmd:
                saveFile();
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
            case MainContentComponent::undoCmd:
                undoManager.undo();
                break;
            case MainContentComponent::redoCmd:
                undoManager.redo();
                break;
            case MainContentComponent::settingsCmd:
                showSettings();
                break;
            default:
                return false;
        }
    //}
    return true;
}

void MainContentComponent::openFile() {
    FileChooser fileChooser("Open File");
    
    if (fileChooser.browseForFileToOpen()) {
        loadFile(fileChooser.getResult());
    }
}

void MainContentComponent::saveFile() {
    FileChooser fileChooser("Save File");

    if (fileChooser.browseForFileToSave(true)) {
        saveFile(fileChooser.getResult());
    }
}

void MainContentComponent::newFile() {
    if (editor->getDocument().hasChangedSinceSavePoint()) {
        AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                     "Unsaved Changes",
                                     "You have unsaved changes. Do you want to discard them?",
                                     "Cancel",
                                     "Discard",
                                     0,
                                     ModalCallbackFunction::create([this](int result) {
            if (result == 1) { // ok
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
    
    runner.resetErrors();
    runner.setCode(content);
    runner.setText(text);
    
    switch (getCurrentRunnerType()) {
        case jp::Markov:
            runner.setParameter("prefixLen", markovPrefixLen->getValue());
            runner.setParameter("textLen", markovTextLen->getValue());
            break;
        default:
            break;
    }

    auto runnerResults = runner.run();
    if (runner.hasErrors()) {
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
}

void MainContentComponent::speak() {
    auto content = results->getText();
    
    speaker->speak(content.toStdString());
}

void MainContentComponent::setCurrentRunnerType(jp::RunnerType rt) {
    runner.setType(rt);
    //if (!editor->getDocument().hasChangedSinceSavePoint()) {
        auto examples = runner.getExamples();
        
        if (examples.size() > 0) {
            editor->loadContent(examples[0]);
        }
    resized();
    //}
}

void MainContentComponent::buttonClicked(Button *button) {
    if (button == &*runButton) {
        run();
    }
    else if (button == &*speakButton) {
        speak();
    }
    else if (button == &*stopSpeakButton) {
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
        
        for (int i = 0; i < runTypeButtons.size(); i++) {
            if (&*runTypeButtons[i] == button) {
                setCurrentRunnerType(runTypeNames[i].second);
                return;
            }
        }
        if (getCurrentRunnerType() == jp::NamShub) {
            auto command = button->getButtonText().toStdString();
            
            runner.setStringParameter("command", command);
            run();
        }
    }
}

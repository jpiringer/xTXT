/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "MainWindow.h"
#include "Runner.hpp"

#include <iostream>
#include <fstream>

inline Colour getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour uiColour, Colour fallback = Colour (0xff4d4d4d))
{
    if (auto* v4 = dynamic_cast<LookAndFeel_V4*> (&LookAndFeel::getDefaultLookAndFeel()))
        return v4->getCurrentColourScheme().getUIColour (uiColour);
    
    return fallback;
}

//==============================================================================
MainContentComponent::MainContentComponent()
: filenameComponent("File", File(), true, false, false,
"*.*", String(),
               "Choose a file to open it in the editor") {
    setOpaque (true);
    
    addAndMakeVisible (menuBar = new MenuBarComponent (this));
    
#if JUCE_MAC
    menuBar->setModel (nullptr);
    MenuBarModel::setMacMainMenu (this);
#endif

    runButton = new TextButton("Run");
    runButton->addListener(this);
    addAndMakeVisible (runButton);

    speakButton = new TextButton("Speak");
    speakButton->addListener(this);
    addAndMakeVisible(speakButton);
    
    // Create the editor..
    addAndMakeVisible (editor = new CodeEditorComponent(codeDocument, &tokeniser));
    Font font(Font::getDefaultMonospacedFontName(), 20, Font::plain);
    editor->setFont(font);
    
    editor->setColour(CodeEditorComponent::backgroundColourId, Colour(0xff, 0xff, 0xff));
    editor->setColour(CodeEditorComponent::lineNumberBackgroundId, Colour(0, 0, 0));

    // Create a file chooser control to load files into it..
    addAndMakeVisible(filenameComponent);
    filenameComponent.addListener(this);
    
    addAndMakeVisible(results = new TextEditor());
    results->setFont(font);
    results->setMultiLine(true);
    results->setColour(TextEditor::backgroundColourId, Colour(0xff, 0xff, 0xff));
    results->setColour(TextEditor::textColourId, Colour(0, 0, 0));

    getLookAndFeel().setUsingNativeAlertWindows(true);

    setSize(600, 800);
    
    speaker = createSpeakerInstance();
    
    for (int i = 0; i < runTypeNames.size(); ++i) {
        TextButton *tb = addRunTypeButton(new TextButton(runTypeNames[i].first));
        
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
    filenameComponent.removeListener (this);
#if JUCE_MAC
    MenuBarModel::setMacMainMenu (nullptr);
#endif
}

void MainContentComponent::paint (Graphics& g) {
    g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground,
                                       Colours::white));
}

void MainContentComponent::resized() {
    Rectangle<int> r(getLocalBounds());

    filenameComponent.setBounds(r.removeFromTop(25));
    runTypeGroup.setBounds(r.removeFromTop(30));
    
    speakButton->setBounds(r.removeFromBottom(25));
    results->setBounds(r.removeFromBottom(200));
    runButton->setBounds(r.removeFromBottom(25));
    editor->setBounds(r.withTrimmedTop(8));
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
    const char* const names[] = { "File", nullptr };
    
    return StringArray (names);
}

PopupMenu MainContentComponent::getMenuForIndex (int menuIndex, const String& menuName) {
    ApplicationCommandManager *commandManager = &MainWindow::getApplicationCommandManager();
    
    PopupMenu menu;
    
    if (menuIndex == 0) {
        menu.addCommandItem (commandManager, MainContentComponent::newCmd);
        menu.addSeparator();
        menu.addCommandItem (commandManager, MainContentComponent::openCmd);
        menu.addCommandItem (commandManager, MainContentComponent::saveCmd);
        menu.addSeparator();
        menu.addCommandItem (commandManager, MainContentComponent::runCmd);
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
        MainContentComponent::speakCmd
    };
    
    commands.addArray (ids, numElementsInArray (ids));
}

void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) {
    const String generalCategory("General");
    const String codeCategory("Code");
    
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

void MainContentComponent::run() {
    auto content = editor->getDocument().getAllContent();
    auto text = results->getText();
    runner.setCode(content.toStdString());
    runner.setText(text.toStdString());

    auto runnerResults = runner.run();
    if (runner.hasErrors()) {
        results->setColour(TextEditor::textColourId, Colour(0xff, 0, 0));
    }
    else {
        results->setColour(TextEditor::textColourId, Colour(0, 0, 0));
    }
    results->setText(runnerResults);
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
    //}
}

void MainContentComponent::buttonClicked(Button *button) {
    if (button == runButton) {
        run();
    }
    else if (button == speakButton) {
        speak();
    }
    else {
        for (int i = 0; i < runTypeButtons.size(); i++) {
            if (runTypeButtons[i] == button) {
                setCurrentRunnerType(runTypeNames[i].second);
            }
        }
    }
}

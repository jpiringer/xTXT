/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#include "Runner.hpp"

inline Colour getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour uiColour, Colour fallback = Colour (0xff4d4d4d))
{
    if (auto* v4 = dynamic_cast<LookAndFeel_V4*> (&LookAndFeel::getDefaultLookAndFeel()))
        return v4->getCurrentColourScheme().getUIColour (uiColour);
    
    return fallback;
}

//==============================================================================
MainContentComponent::MainContentComponent()
: fileChooser ("File", File(), true, false, false,
"*.*", String(),
               "Choose a file to open it in the editor") {
    setOpaque (true);
    
    addAndMakeVisible (menuBar = new MenuBarComponent (this));
    
#if JUCE_MAC
    menuBar->setModel (nullptr);
    MenuBarModel::setMacMainMenu (this);
#endif

    runButton = new TextButton("Run");
    //runButton->setTriggeredOnMouseDown(true);
    runButton->addListener(this);
    addAndMakeVisible (runButton);

    // Create the editor..
    addAndMakeVisible (editor = new CodeEditorComponent (codeDocument, &cppTokeniser));
    Font font(Font::getDefaultMonospacedFontName(), 20, Font::plain);
    editor->setFont(font);
    
    editor->loadContent(runner.getSampleCode());
    
    editor->setColour(CodeEditorComponent::backgroundColourId, Colour(0xff, 0xff, 0xff));
    editor->setColour(CodeEditorComponent::lineNumberBackgroundId, Colour(0, 0, 0));

    // Create a file chooser control to load files into it..
    addAndMakeVisible (fileChooser);
    fileChooser.addListener (this);
    
    addAndMakeVisible(results = new TextEditor());
    results->setFont(font);
    results->setMultiLine(true);
    results->setColour(TextEditor::backgroundColourId, Colour(0xff, 0xff, 0xff));

    setSize(600, 800);
}

MainContentComponent::~MainContentComponent() {
    fileChooser.removeListener (this);
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

    fileChooser.setBounds(r.removeFromTop(25));
    results->setBounds(r.removeFromBottom(200));
    runButton->setBounds(r.removeFromBottom(25));
    editor->setBounds(r.withTrimmedTop(8));
}

void MainContentComponent::filenameComponentChanged (FilenameComponent*) {
    editor->loadContent (fileChooser.getCurrentFile().loadFileAsString());
}

StringArray MainContentComponent::getMenuBarNames() {
    const char* const names[] = { "txtgen", "Look-and-feel", "Tabs", "Misc", nullptr };
    
    return StringArray (names);
}

PopupMenu MainContentComponent::getMenuForIndex (int menuIndex, const String& menuName) {
    //ApplicationCommandManager* commandManager = &MainAppWindow::getApplicationCommandManager();
    
    PopupMenu menu;
    
    /*if (menuIndex == 0)
    {
        menu.addCommandItem (commandManager, MainAppWindow::showPreviousDemo);
        menu.addCommandItem (commandManager, MainAppWindow::showNextDemo);
        menu.addSeparator();
        menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit);
    }
    else if (menuIndex == 1)
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
        MainContentComponent::runCmd
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
            default:
                return false;
        }
    //}
    return true;
}

void MainContentComponent::run() {
    auto content = editor->getDocument().getAllContent();
    runner.setCode(content.toStdString());
    
    auto runnerResults = runner.run();
    if (runner.hasErrors()) {
        results->setColour(TextEditor::textColourId, Colour(0xff, 0, 0));
    }
    else {
        results->setColour(TextEditor::textColourId, Colour(0, 0, 0));
    }
    results->setText(runnerResults);
}

void MainContentComponent::buttonClicked(Button* button) {
    if (button == runButton) {
        run();
    }
}

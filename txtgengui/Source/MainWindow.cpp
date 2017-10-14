/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "MainWindow.h"
#include "MainComponent.h"

ScopedPointer<ApplicationCommandManager> MainWindow::applicationCommandManager = nullptr;
ScopedPointer<ApplicationProperties> MainWindow::applicationProperties = nullptr;
PropertiesFile::Options MainWindow::applicationPropertiesStorageOptions;

MainWindow::MainWindow (String name)  : DocumentWindow (name, 
                                            Desktop::getInstance().getDefaultLookAndFeel()
                                            .findColour (ResizableWindow::backgroundColourId),
                                            DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(contentComponent = new MainContentComponent(), true);
    
    // this lets the command manager use keypresses that arrive in our window to send out commands
    addKeyListener (getApplicationCommandManager().getKeyMappings());
    
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, true);
    
    triggerAsyncUpdate();
}

MainWindow::~MainWindow() {
    applicationCommandManager = nullptr;
    applicationProperties = nullptr;
}

void MainWindow::closeButtonPressed() {
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

ApplicationCommandManager &MainWindow::getApplicationCommandManager() {
    if (applicationCommandManager == nullptr)
        applicationCommandManager = new ApplicationCommandManager();
    
    return *applicationCommandManager;
}

ApplicationProperties &MainWindow::getApplicationProperties() {
    if (applicationProperties == nullptr) {
        applicationProperties = new ApplicationProperties();
        applicationPropertiesStorageOptions.applicationName = "txtgen";
        applicationPropertiesStorageOptions.filenameSuffix = "settings";
        applicationPropertiesStorageOptions.osxLibrarySubFolder = "Application Support/txtgen/";
        applicationProperties->setStorageParameters(applicationPropertiesStorageOptions);
    }
    
    return *applicationProperties;
    
}

void MainWindow::handleAsyncUpdate() {
    // This registers all of our commands with the command manager but has to be done after the window has
    // been created so we can find the number of rendering engines available
    auto& commandManager = MainWindow::getApplicationCommandManager();
    
    commandManager.registerAllCommandsForTarget(contentComponent);
    commandManager.registerAllCommandsForTarget(JUCEApplication::getInstance());
}

bool MainWindow::hasUnsavedChanges() {
    return contentComponent->hasUnsavedChanges();
}

bool MainWindow::canQuit() {
    if (hasUnsavedChanges()) {
        if (AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon,
                                            "Unsaved Changes",
                                            "You have unsaved changes. Do you want to discard them and quit?",
                                            "Quit without Saving",
                                            "No",
                                            "Cancel",
                                            0) == 1) {
            return true;
        }
        return false;
    }
    else {
        return true;
    }
}

//==============================================================================
class xTXTApplication  : public JUCEApplication {
public:
    //==============================================================================
    xTXTApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override {
        mainWindow = new MainWindow (getApplicationName());
    }

    void shutdown() override {
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override {
        if (mainWindow->canQuit()) {
            quit();
        }
    }

    void anotherInstanceStarted (const String& commandLine) override {
    }
    
    void unhandledException(const std::exception *e, const String &sourceFilename, int lineNumber) override {
        String msg = "";
        
        if (e != nullptr) {
            msg += e->what();
            msg += "\n";
        }
        msg += "in file: \"";
        msg += sourceFilename;
        msg += "\" at line number: ";
        msg += lineNumber;
        
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Exception", msg);
    }

private:
    ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (xTXTApplication)

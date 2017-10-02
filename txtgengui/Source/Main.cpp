/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

static ScopedPointer<ApplicationCommandManager> applicationCommandManager;

//==============================================================================
class txtgenguiApplication  : public JUCEApplication
{
public:
    //==============================================================================
    txtgenguiApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow = new MainWindow (getApplicationName());
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow    : public DocumentWindow, private AsyncUpdater {
    public:
        MainWindow (String name)  : DocumentWindow (name,
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
        
        ~MainWindow() {
            applicationCommandManager = nullptr;
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
        ApplicationCommandManager &getApplicationCommandManager()
        {
            if (applicationCommandManager == nullptr)
                applicationCommandManager = new ApplicationCommandManager();
            
            return *applicationCommandManager;
        }
        
        void handleAsyncUpdate() override {
            // This registers all of our commands with the command manager but has to be done after the window has
            // been created so we can find the number of rendering engines available
            auto& commandManager = MainWindow::getApplicationCommandManager();
            
            commandManager.registerAllCommandsForTarget(contentComponent);
            commandManager.registerAllCommandsForTarget(JUCEApplication::getInstance());
        }

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

private:
    ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (txtgenguiApplication)

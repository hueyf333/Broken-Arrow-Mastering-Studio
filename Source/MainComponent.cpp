#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : projectSerializer (audioEngine),
      menuBar (this),
      transportBar (audioEngine),
      timelineComponent (audioEngine),
      mixerComponent (audioEngine)
{
    setSize (1200, 800);
    
    // Set up command manager
    commandManager.registerAllCommandsForTarget (this);
    
    // Add menu bar
    addAndMakeVisible (menuBar);
    
    // Add transport bar
    addAndMakeVisible (transportBar);
    
    // Add timeline
    addAndMakeVisible (timelineComponent);
    
    // Add mixer
    addAndMakeVisible (mixerComponent);
    
    // Initialize audio engine
    audioEngine.initialiseAudioDevice();
    
    // Add some default tracks
    audioEngine.addTrack ("Track 1");
    audioEngine.addTrack ("Track 2");
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    // Menu bar
    menuBar.setBounds (area.removeFromTop (25));
    
    // Transport bar
    transportBar.setBounds (area.removeFromTop (60));
    
    // Split remaining area between timeline and mixer
    if (showMixer && showTimeline)
    {
        mixerComponent.setBounds (area.removeFromBottom (250));
        timelineComponent.setBounds (area);
    }
    else if (showTimeline)
    {
        timelineComponent.setBounds (area);
        mixerComponent.setBounds (0, 0, 0, 0);
    }
    else if (showMixer)
    {
        mixerComponent.setBounds (area);
        timelineComponent.setBounds (0, 0, 0, 0);
    }
}

//==============================================================================
// Menu Bar

juce::StringArray MainComponent::getMenuBarNames()
{
    return { "File", "Edit", "View", "Plugins", "Help" };
}

juce::PopupMenu MainComponent::getMenuForIndex (int topLevelMenuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;
    
    if (topLevelMenuIndex == 0)  // File
    {
        menu.addCommandItem (&commandManager, newProject);
        menu.addCommandItem (&commandManager, openProject);
        menu.addSeparator();
        menu.addCommandItem (&commandManager, saveProject);
        menu.addCommandItem (&commandManager, saveProjectAs);
        menu.addSeparator();
        menu.addCommandItem (&commandManager, importAudio);
        menu.addCommandItem (&commandManager, exportMixdown);
        menu.addSeparator();
        menu.addCommandItem (&commandManager, quitApp);
    }
    else if (topLevelMenuIndex == 1)  // Edit
    {
        menu.addCommandItem (&commandManager, undoAction);
        menu.addCommandItem (&commandManager, redoAction);
    }
    else if (topLevelMenuIndex == 2)  // View
    {
        menu.addCommandItem (&commandManager, toggleMixer);
        menu.addCommandItem (&commandManager, toggleTimeline);
    }
    else if (topLevelMenuIndex == 3)  // Plugins
    {
        menu.addCommandItem (&commandManager, scanPlugins);
    }
    else if (topLevelMenuIndex == 4)  // Help
    {
        menu.addCommandItem (&commandManager, showAbout);
    }
    
    return menu;
}

void MainComponent::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
    // Handled by command manager
}

//==============================================================================
// Command Target

void MainComponent::getAllCommands (juce::Array<juce::CommandID>& commands)
{
    commands.add (newProject);
    commands.add (openProject);
    commands.add (saveProject);
    commands.add (saveProjectAs);
    commands.add (importAudio);
    commands.add (exportMixdown);
    commands.add (quitApp);
    commands.add (undoAction);
    commands.add (redoAction);
    commands.add (toggleMixer);
    commands.add (toggleTimeline);
    commands.add (scanPlugins);
    commands.add (showAbout);
}

void MainComponent::getCommandInfo (juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case newProject:
            result.setInfo ("New Project", "Create a new project", "File", 0);
            result.addDefaultKeypress ('n', juce::ModifierKeys::commandModifier);
            break;
        case openProject:
            result.setInfo ("Open Project", "Open an existing project", "File", 0);
            result.addDefaultKeypress ('o', juce::ModifierKeys::commandModifier);
            break;
        case saveProject:
            result.setInfo ("Save Project", "Save the current project", "File", 0);
            result.addDefaultKeypress ('s', juce::ModifierKeys::commandModifier);
            break;
        case saveProjectAs:
            result.setInfo ("Save Project As...", "Save the project with a new name", "File", 0);
            result.addDefaultKeypress ('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
            break;
        case importAudio:
            result.setInfo ("Import Audio...", "Import an audio file to a track", "File", 0);
            result.addDefaultKeypress ('i', juce::ModifierKeys::commandModifier);
            break;
        case exportMixdown:
            result.setInfo ("Export Mixdown...", "Export the mix to an audio file", "File", 0);
            result.addDefaultKeypress ('e', juce::ModifierKeys::commandModifier);
            break;
        case quitApp:
            result.setInfo ("Quit", "Quit the application", "File", 0);
            result.addDefaultKeypress ('q', juce::ModifierKeys::commandModifier);
            break;
        case undoAction:
            result.setInfo ("Undo", "Undo the last action", "Edit", 0);
            result.addDefaultKeypress ('z', juce::ModifierKeys::commandModifier);
            break;
        case redoAction:
            result.setInfo ("Redo", "Redo the last undone action", "Edit", 0);
            result.addDefaultKeypress ('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
            break;
        case toggleMixer:
            result.setInfo ("Toggle Mixer", "Show or hide the mixer", "View", 0);
            result.addDefaultKeypress ('m', juce::ModifierKeys::commandModifier);
            break;
        case toggleTimeline:
            result.setInfo ("Toggle Timeline", "Show or hide the timeline", "View", 0);
            result.addDefaultKeypress ('t', juce::ModifierKeys::commandModifier);
            break;
        case scanPlugins:
            result.setInfo ("Scan for Plugins", "Scan for available audio plugins", "Plugins", 0);
            break;
        case showAbout:
            result.setInfo ("About", "Show information about this application", "Help", 0);
            break;
    }
}

bool MainComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case newProject:        newProjectAction(); return true;
        case openProject:       openProjectAction(); return true;
        case saveProject:       saveProjectAction(); return true;
        case saveProjectAs:     saveProjectAsAction(); return true;
        case importAudio:       importAudioAction(); return true;
        case exportMixdown:     exportMixdownAction(); return true;
        case quitApp:           juce::JUCEApplication::getInstance()->systemRequestedQuit(); return true;
        case undoAction:        return true;  // Stub
        case redoAction:        return true;  // Stub
        case toggleMixer:       showMixer = !showMixer; resized(); return true;
        case toggleTimeline:    showTimeline = !showTimeline; resized(); return true;
        case scanPlugins:       scanPluginsAction(); return true;
        case showAbout:         showAboutDialog(); return true;
        default:                return false;
    }
}

//==============================================================================
// Actions

void MainComponent::newProjectAction()
{
    // Clear current project
    audioEngine.stop();
    audioEngine.setPlayheadPosition (0.0);
    
    projectSerializer.setCurrentProjectFile (juce::File());
}

void MainComponent::openProjectAction()
{
    juce::FileChooser chooser ("Open Project", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.basproj");
    
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        
        if (projectSerializer.loadProject (file))
        {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                                    "Success",
                                                    "Project loaded successfully!");
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                    "Error",
                                                    "Failed to load project file.");
        }
    }
}

void MainComponent::saveProjectAction()
{
    auto currentFile = projectSerializer.getCurrentProjectFile();
    
    if (currentFile == juce::File())
    {
        saveProjectAsAction();
    }
    else
    {
        projectSerializer.saveProject (currentFile);
    }
}

void MainComponent::saveProjectAsAction()
{
    juce::FileChooser chooser ("Save Project As", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.basproj");
    
    if (chooser.browseForFileToSave (true))
    {
        auto file = chooser.getResult().withFileExtension ("basproj");
        
        if (projectSerializer.saveProject (file))
        {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                                    "Success",
                                                    "Project saved successfully!");
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                    "Error",
                                                    "Failed to save project file.");
        }
    }
}

void MainComponent::importAudioAction()
{
    if (audioEngine.getNumTracks() == 0)
    {
        juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                "No Tracks",
                                                "Please create a track first.");
        return;
    }
    
    juce::FileChooser chooser ("Import Audio", juce::File::getSpecialLocation (juce::File::userMusicDirectory), "*.wav;*.aiff;*.flac;*.mp3");
    
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        
        // Import to first track at current playhead position
        audioEngine.importAudioToTrack (0, file, audioEngine.getPlayheadPosition());
        
        juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                                "Success",
                                                "Audio imported to Track 1!");
    }
}

void MainComponent::exportMixdownAction()
{
    juce::FileChooser chooser ("Export Mixdown", juce::File::getSpecialLocation (juce::File::userMusicDirectory), "*.wav");
    
    if (chooser.browseForFileToSave (true))
    {
        auto file = chooser.getResult().withFileExtension ("wav");
        
        // Export from 0 to 60 seconds (or could calculate from clips)
        audioEngine.exportMixdown (file, 0.0, 60.0);
        
        juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                                "Success",
                                                "Mixdown exported successfully!");
    }
}

void MainComponent::scanPluginsAction()
{
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                            "Scanning Plugins",
                                            "Scanning for audio plugins. This may take a moment...");
    
    pluginScanner.scanForPlugins();
    
    int numPlugins = pluginScanner.getPluginList().getNumTypes();
    
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                            "Scan Complete",
                                            "Found " + juce::String (numPlugins) + " plugins.");
}

void MainComponent::showAboutDialog()
{
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                            "About Broken Arrow Studio",
                                            "Broken Arrow Mastering Studio\n\n"
                                            "A full-featured multitrack recording system\n"
                                            "built with the JUCE Framework.\n\n"
                                            "Version 1.0\n"
                                            "© 2025 Broken Arrow");
}

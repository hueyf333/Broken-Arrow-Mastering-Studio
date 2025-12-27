#pragma once
#include <JuceHeader.h>
#include "AudioEngine.h"
#include "TransportBar.h"
#include "TimelineComponent.h"
#include "MixerComponent.h"
#include "PluginScanner.h"
#include "ProjectSerializer.h"

//==============================================================================
/**
 * Main component that manages the application UI and menu system
 */
class MainComponent : public juce::Component,
                     public juce::MenuBarModel,
                     private juce::ApplicationCommandTarget
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // MenuBarModel
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex (int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    
    // ApplicationCommandTarget
    juce::ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
    void getAllCommands (juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo (juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

private:
    enum CommandIDs
    {
        newProject = 1,
        openProject,
        saveProject,
        saveProjectAs,
        importAudio,
        exportMixdown,
        quitApp,
        undoAction,
        redoAction,
        toggleMixer,
        toggleTimeline,
        scanPlugins,
        showAbout
    };
    
    void newProjectAction();
    void openProjectAction();
    void saveProjectAction();
    void saveProjectAsAction();
    void importAudioAction();
    void exportMixdownAction();
    void scanPluginsAction();
    void showAboutDialog();
    
    AudioEngine audioEngine;
    ProjectSerializer projectSerializer;
    PluginScanner pluginScanner;
    
    juce::MenuBarComponent menuBar;
    TransportBar transportBar;
    TimelineComponent timelineComponent;
    MixerComponent mixerComponent;
    
    juce::ApplicationCommandManager commandManager;
    
    bool showMixer {true};
    bool showTimeline {true};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

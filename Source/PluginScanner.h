#pragma once
#include <JuceHeader.h>

//==============================================================================
/**
 * Manages plugin scanning and known plugins list
 */
class PluginScanner
{
public:
    PluginScanner();
    ~PluginScanner() = default;

    // Scanning
    void scanForPlugins();
    bool isScanning() const { return scanning; }
    
    // Plugin list access
    const juce::KnownPluginList& getPluginList() const { return knownPlugins; }
    juce::KnownPluginList& getPluginList() { return knownPlugins; }
    
    // Plugin loading
    std::unique_ptr<juce::AudioPluginInstance> createPlugin (int index);
    
    // Persistence
    void savePluginList();
    void loadPluginList();
    
private:
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPlugins;
    bool scanning {false};
    
    juce::File getPluginListFile() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginScanner)
};

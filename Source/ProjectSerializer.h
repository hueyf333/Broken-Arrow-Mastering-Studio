#pragma once
#include <JuceHeader.h>
#include "AudioEngine.h"

//==============================================================================
/**
 * Handles saving and loading project files
 */
class ProjectSerializer
{
public:
    ProjectSerializer (AudioEngine& engine);
    ~ProjectSerializer() = default;

    // Project file operations
    bool saveProject (const juce::File& file);
    bool loadProject (const juce::File& file);
    
    juce::File getCurrentProjectFile() const { return currentProjectFile; }
    void setCurrentProjectFile (const juce::File& file) { currentProjectFile = file; }
    
private:
    AudioEngine& audioEngine;
    juce::File currentProjectFile;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectSerializer)
};

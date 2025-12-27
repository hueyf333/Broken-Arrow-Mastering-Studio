#include "ProjectSerializer.h"

ProjectSerializer::ProjectSerializer (AudioEngine& engine)
    : audioEngine (engine)
{
}

bool ProjectSerializer::saveProject (const juce::File& file)
{
    currentProjectFile = file;
    
    // Get engine state
    auto state = audioEngine.getState();
    
    // Convert to JSON
    juce::var project = new juce::DynamicObject();
    project.getDynamicObject()->setProperty ("version", 1);
    project.getDynamicObject()->setProperty ("name", file.getFileNameWithoutExtension());
    project.getDynamicObject()->setProperty ("engine", state);
    
    // Write to file
    auto jsonString = juce::JSON::toString (project);
    
    return file.replaceWithText (jsonString);
}

bool ProjectSerializer::loadProject (const juce::File& file)
{
    if (!file.existsAsFile())
        return false;
    
    currentProjectFile = file;
    
    // Read file
    auto jsonString = file.loadFileAsString();
    
    // Parse JSON
    auto project = juce::JSON::parse (jsonString);
    
    if (!project.isObject())
        return false;
    
    auto* projectObj = project.getDynamicObject();
    
    if (projectObj == nullptr)
        return false;
    
    // Load engine state
    auto engineState = projectObj->getProperty ("engine");
    audioEngine.setState (engineState);
    
    return true;
}

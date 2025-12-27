#pragma once
#include <JuceHeader.h>
#include "AudioClip.h"

//==============================================================================
/**
 * Represents a single audio track with clips, processing chain, and controls
 */
class Track
{
public:
    Track (const juce::String& trackName = "New Track");
    ~Track() = default;

    // Track properties
    juce::String getName() const { return name; }
    void setName (const juce::String& newName) { name = newName; }
    
    juce::Colour getColour() const { return colour; }
    void setColour (juce::Colour newColour) { colour = newColour; }
    
    // Transport controls
    bool isMuted() const { return muted; }
    void setMuted (bool shouldBeMuted) { muted = shouldBeMuted; }
    
    bool isSoloed() const { return soloed; }
    void setSoloed (bool shouldBeSoloed) { soloed = shouldBeSoloed; }
    
    bool isArmed() const { return armed; }
    void setArmed (bool shouldBeArmed) { armed = shouldBeArmed; }
    
    // Mix controls
    float getGain() const { return gain; }
    void setGain (float newGain) { gain = juce::jlimit (0.0f, 2.0f, newGain); }
    
    float getPan() const { return pan; }
    void setPan (float newPan) { pan = juce::jlimit (-1.0f, 1.0f, newPan); }
    
    // Level metering
    float getLevel() const { return currentLevel; }
    void updateLevel (float level) { currentLevel = level; }
    
    // Clip management
    void addClip (std::unique_ptr<AudioClip> clip);
    void removeClip (int index);
    int getNumClips() const { return clips.size(); }
    AudioClip* getClip (int index) const;
    
    // Audio processing
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void processBlock (juce::AudioBuffer<float>& buffer, double currentPlayheadPosition);
    void releaseResources();
    
    // Plugin chain
    void addPlugin (std::unique_ptr<juce::AudioPluginInstance> plugin);
    void removePlugin (int index);
    int getNumPlugins() const;
    juce::AudioPluginInstance* getPlugin (int index) const;
    
    // State persistence
    juce::var getState() const;
    void setState (const juce::var& state);
    
private:
    juce::String name;
    juce::Colour colour;
    
    bool muted {false};
    bool soloed {false};
    bool armed {false};
    
    float gain {1.0f};
    float pan {0.0f};
    float currentLevel {0.0f};
    
    juce::OwnedArray<AudioClip> clips;
    juce::OwnedArray<juce::AudioPluginInstance> plugins;
    
    // Audio processing buffers
    juce::AudioBuffer<float> tempBuffer;
    double sampleRate {44100.0};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};

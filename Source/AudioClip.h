#pragma once
#include <JuceHeader.h>

//==============================================================================
/**
 * Represents an audio clip/region on the timeline
 */
class AudioClip
{
public:
    AudioClip (const juce::File& audioFile, double startPosSeconds = 0.0);
    ~AudioClip() = default;

    // Getters
    juce::String getName() const { return name; }
    double getStartPosition() const { return startPosition; }
    double getLength() const { return length; }
    double getEndPosition() const { return startPosition + length; }
    juce::File getFile() const { return file; }
    
    // Setters
    void setStartPosition (double newStart) { startPosition = newStart; }
    void setName (const juce::String& newName) { name = newName; }
    
    // Audio data access
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill,
                           double currentPlayheadPosition);
    void releaseResources();
    
    // State
    juce::var getState() const;
    void setState (const juce::var& state);
    
private:
    juce::String name;
    juce::File file;
    double startPosition {0.0};  // in seconds
    double length {0.0};          // in seconds
    
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioFormatManager formatManager;
    double sourceSampleRate {44100.0};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioClip)
};

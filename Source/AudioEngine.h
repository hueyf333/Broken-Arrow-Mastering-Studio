#pragma once
#include <JuceHeader.h>
#include "Track.h"

//==============================================================================
/**
 * Main audio engine that manages tracks, recording, playback, and mixing
 */
class AudioEngine : public juce::AudioIODeviceCallback,
                   public juce::ChangeBroadcaster
{
public:
    AudioEngine();
    ~AudioEngine() override;

    // Device management
    void initialiseAudioDevice();
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
    
    // Transport controls
    void play();
    void stop();
    void record();
    bool isPlaying() const { return playing; }
    bool isRecording() const { return recording; }
    
    double getPlayheadPosition() const { return playheadPosition; }
    void setPlayheadPosition (double newPosition) { playheadPosition = newPosition; }
    
    double getTempo() const { return tempo; }
    void setTempo (double newTempo) { tempo = juce::jlimit (20.0, 300.0, newTempo); }
    
    bool isMetronomeEnabled() const { return metronomeEnabled; }
    void setMetronomeEnabled (bool enabled) { metronomeEnabled = enabled; }
    
    // Track management
    void addTrack (const juce::String& name = "New Track");
    void removeTrack (int index);
    int getNumTracks() const { return tracks.size(); }
    Track* getTrack (int index) const;
    
    // Audio import
    void importAudioToTrack (int trackIndex, const juce::File& audioFile, double startPosition);
    
    // Export/bounce
    void exportMixdown (const juce::File& outputFile, double startTime, double endTime);
    
    // AudioIODeviceCallback
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;
    
    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
    // State persistence
    juce::var getState() const;
    void setState (const juce::var& state);
    
private:
    juce::AudioDeviceManager deviceManager;
    juce::OwnedArray<Track> tracks;
    
    bool playing {false};
    bool recording {false};
    double playheadPosition {0.0};
    double tempo {120.0};
    bool metronomeEnabled {false};
    
    double sampleRate {44100.0};
    int bufferSize {512};
    
    // Recording
    juce::OwnedArray<juce::AudioFormatWriter::ThreadedWriter> activeRecordings;
    juce::TimeSliceThread writerThread {"Audio Writer"};
    
    // Metronome
    int metronomeBeatPosition {0};
    
    // Mixing
    juce::AudioBuffer<float> mixBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioEngine)
};

#pragma once
#include <JuceHeader.h>
#include "AudioEngine.h"

//==============================================================================
/**
 * Transport controls bar with play/stop/record buttons and time display
 */
class TransportBar : public juce::Component,
                     private juce::Timer
{
public:
    TransportBar (AudioEngine& engine);
    ~TransportBar() override = default;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    void timerCallback() override;
    
    AudioEngine& audioEngine;
    
    juce::TextButton playButton {"Play"};
    juce::TextButton stopButton {"Stop"};
    juce::TextButton recordButton {"Record"};
    juce::ToggleButton metronomeButton {"Metronome"};
    
    juce::Label tempoLabel;
    juce::Slider tempoSlider;
    
    juce::Label timeDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBar)
};

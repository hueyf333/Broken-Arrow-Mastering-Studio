#pragma once
#include <JuceHeader.h>
#include "Track.h"

//==============================================================================
/**
 * UI component for a single track showing controls and level meter
 */
class TrackComponent : public juce::Component,
                      private juce::Timer
{
public:
    TrackComponent (Track& track);
    ~TrackComponent() override = default;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    void timerCallback() override;
    
    Track& track;
    
    juce::Label nameLabel;
    juce::ToggleButton muteButton {"M"};
    juce::ToggleButton soloButton {"S"};
    juce::ToggleButton armButton {"R"};
    
    juce::Slider gainSlider;
    juce::Slider panSlider;
    
    float currentLevel {0.0f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackComponent)
};

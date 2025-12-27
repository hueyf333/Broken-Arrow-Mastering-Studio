#pragma once
#include <JuceHeader.h>
#include "AudioEngine.h"

//==============================================================================
/**
 * Timeline view showing tracks and clips
 */
class TimelineComponent : public juce::Component,
                         private juce::ChangeListener
{
public:
    TimelineComponent (AudioEngine& engine);
    ~TimelineComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    
private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void updateContent();
    
    AudioEngine& audioEngine;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimelineComponent)
};

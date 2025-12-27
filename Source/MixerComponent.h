#pragma once
#include <JuceHeader.h>
#include "AudioEngine.h"
#include "TrackComponent.h"

//==============================================================================
/**
 * Mixer view showing all track controls
 */
class MixerComponent : public juce::Component,
                      private juce::ChangeListener
{
public:
    MixerComponent (AudioEngine& engine);
    ~MixerComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void updateTracks();
    
    AudioEngine& audioEngine;
    juce::OwnedArray<TrackComponent> trackComponents;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerComponent)
};

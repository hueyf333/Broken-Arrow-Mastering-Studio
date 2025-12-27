#include "MixerComponent.h"

MixerComponent::MixerComponent (AudioEngine& engine)
    : audioEngine (engine)
{
    audioEngine.addChangeListener (this);
    updateTracks();
}

MixerComponent::~MixerComponent()
{
    audioEngine.removeChangeListener (this);
}

void MixerComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MixerComponent::resized()
{
    auto area = getLocalBounds().reduced (10);
    
    int trackWidth = 120;
    
    for (auto* trackComp : trackComponents)
    {
        trackComp->setBounds (area.removeFromLeft (trackWidth));
        area.removeFromLeft (10);
    }
}

void MixerComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &audioEngine)
    {
        updateTracks();
    }
}

void MixerComponent::updateTracks()
{
    trackComponents.clear();
    
    for (int i = 0; i < audioEngine.getNumTracks(); ++i)
    {
        auto* track = audioEngine.getTrack (i);
        auto* trackComp = new TrackComponent (*track);
        trackComponents.add (trackComp);
        addAndMakeVisible (trackComp);
    }
    
    resized();
}

#include "TimelineComponent.h"

TimelineComponent::TimelineComponent (AudioEngine& engine)
    : audioEngine (engine)
{
    audioEngine.addChangeListener (this);
}

TimelineComponent::~TimelineComponent()
{
    audioEngine.removeChangeListener (this);
}

void TimelineComponent::paint (juce::Graphics& g)
{
    auto area = getLocalBounds();
    
    g.fillAll (juce::Colours::darkgrey);
    
    // Draw time ruler
    auto rulerArea = area.removeFromTop (30);
    g.setColour (juce::Colours::black);
    g.fillRect (rulerArea);
    
    g.setColour (juce::Colours::white);
    g.setFont (12.0f);
    
    // Draw time markers every second
    int pixelsPerSecond = 50;
    for (int second = 0; second < area.getWidth() / pixelsPerSecond; ++second)
    {
        int x = second * pixelsPerSecond;
        g.drawText (juce::String (second) + "s", x, 5, pixelsPerSecond, 20, 
                   juce::Justification::centred);
        g.drawVerticalLine (x, 25, area.getBottom());
    }
    
    // Draw playhead
    int playheadX = static_cast<int> (audioEngine.getPlayheadPosition() * pixelsPerSecond);
    g.setColour (juce::Colours::yellow);
    g.drawVerticalLine (playheadX, 0, area.getBottom());
    
    // Draw tracks
    int trackHeight = 100;
    int yPos = rulerArea.getBottom();
    
    for (int i = 0; i < audioEngine.getNumTracks(); ++i)
    {
        auto* track = audioEngine.getTrack (i);
        
        auto trackArea = juce::Rectangle<int> (0, yPos, area.getWidth(), trackHeight);
        
        g.setColour (track->getColour().darker());
        g.fillRect (trackArea);
        
        g.setColour (juce::Colours::white);
        g.drawRect (trackArea);
        
        // Draw track name
        g.drawText (track->getName(), trackArea.reduced (5), juce::Justification::topLeft);
        
        // Draw clips
        for (int j = 0; j < track->getNumClips(); ++j)
        {
            auto* clip = track->getClip (j);
            
            int clipX = static_cast<int> (clip->getStartPosition() * pixelsPerSecond);
            int clipWidth = static_cast<int> (clip->getLength() * pixelsPerSecond);
            
            auto clipRect = juce::Rectangle<int> (clipX, yPos + 25, clipWidth, trackHeight - 30);
            
            g.setColour (juce::Colours::lightblue);
            g.fillRect (clipRect);
            
            g.setColour (juce::Colours::white);
            g.drawRect (clipRect);
            
            g.drawText (clip->getName(), clipRect.reduced (5), juce::Justification::topLeft);
        }
        
        yPos += trackHeight;
    }
}

void TimelineComponent::resized()
{
    repaint();
}

void TimelineComponent::mouseDown (const juce::MouseEvent& e)
{
    // Allow clicking to set playhead position
    int pixelsPerSecond = 50;
    double clickedTime = e.x / static_cast<double> (pixelsPerSecond);
    audioEngine.setPlayheadPosition (clickedTime);
    repaint();
}

void TimelineComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &audioEngine)
    {
        updateContent();
    }
}

void TimelineComponent::updateContent()
{
    repaint();
}

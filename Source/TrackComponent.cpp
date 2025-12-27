#include "TrackComponent.h"

TrackComponent::TrackComponent (Track& t)
    : track (t)
{
    addAndMakeVisible (nameLabel);
    nameLabel.setText (track.getName(), juce::dontSendNotification);
    nameLabel.setEditable (true);
    nameLabel.onTextChange = [this] 
    { 
        track.setName (nameLabel.getText()); 
    };
    
    addAndMakeVisible (muteButton);
    muteButton.onClick = [this] 
    { 
        track.setMuted (muteButton.getToggleState()); 
    };
    
    addAndMakeVisible (soloButton);
    soloButton.onClick = [this] 
    { 
        track.setSoloed (soloButton.getToggleState()); 
    };
    
    addAndMakeVisible (armButton);
    armButton.onClick = [this] 
    { 
        track.setArmed (armButton.getToggleState()); 
    };
    armButton.setColour (juce::ToggleButton::tickColourId, juce::Colours::red);
    
    addAndMakeVisible (gainSlider);
    gainSlider.setRange (0.0, 2.0, 0.01);
    gainSlider.setValue (track.getGain());
    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    gainSlider.onValueChange = [this] 
    { 
        track.setGain (static_cast<float> (gainSlider.getValue())); 
    };
    
    addAndMakeVisible (panSlider);
    panSlider.setRange (-1.0, 1.0, 0.01);
    panSlider.setValue (track.getPan());
    panSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    panSlider.onValueChange = [this] 
    { 
        track.setPan (static_cast<float> (panSlider.getValue())); 
    };
    
    startTimer (50);  // Update meters 20 times per second
}

void TrackComponent::paint (juce::Graphics& g)
{
    auto area = getLocalBounds();
    
    g.setColour (track.getColour().darker());
    g.fillRect (area);
    
    g.setColour (track.getColour().darker().darker());
    g.drawRect (area);
    
    // Draw level meter
    auto meterArea = area.removeFromRight (20).reduced (5);
    
    g.setColour (juce::Colours::black);
    g.fillRect (meterArea);
    
    // Draw meter level
    float level = juce::jlimit (0.0f, 1.0f, currentLevel * 5.0f);  // Scale up for visibility
    int levelHeight = static_cast<int> (meterArea.getHeight() * level);
    
    auto levelRect = meterArea.removeFromBottom (levelHeight);
    
    g.setGradientFill (juce::ColourGradient (juce::Colours::green, 
                                             levelRect.getBottomLeft().toFloat(),
                                             juce::Colours::red,
                                             levelRect.getTopLeft().toFloat(),
                                             false));
    g.fillRect (levelRect);
}

void TrackComponent::resized()
{
    auto area = getLocalBounds().reduced (5);
    
    // Reserve space for meter on the right
    area.removeFromRight (20);
    
    nameLabel.setBounds (area.removeFromTop (25));
    
    auto buttonArea = area.removeFromTop (30);
    muteButton.setBounds (buttonArea.removeFromLeft (30));
    soloButton.setBounds (buttonArea.removeFromLeft (30));
    armButton.setBounds (buttonArea.removeFromLeft (30));
    
    panSlider.setBounds (area.removeFromTop (80));
    
    gainSlider.setBounds (area.reduced (10));
}

void TrackComponent::timerCallback()
{
    currentLevel = track.getLevel();
    repaint();
}

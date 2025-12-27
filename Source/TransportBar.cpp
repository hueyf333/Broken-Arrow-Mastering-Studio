#include "TransportBar.h"

TransportBar::TransportBar (AudioEngine& engine)
    : audioEngine (engine)
{
    addAndMakeVisible (playButton);
    playButton.onClick = [this] { audioEngine.play(); };
    
    addAndMakeVisible (stopButton);
    stopButton.onClick = [this] 
    { 
        audioEngine.stop(); 
        audioEngine.setPlayheadPosition (0.0);
    };
    
    addAndMakeVisible (recordButton);
    recordButton.onClick = [this] { audioEngine.record(); };
    recordButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red.darker());
    
    addAndMakeVisible (metronomeButton);
    metronomeButton.onClick = [this] 
    { 
        audioEngine.setMetronomeEnabled (metronomeButton.getToggleState()); 
    };
    
    addAndMakeVisible (tempoLabel);
    tempoLabel.setText ("Tempo:", juce::dontSendNotification);
    tempoLabel.setJustificationType (juce::Justification::centredRight);
    
    addAndMakeVisible (tempoSlider);
    tempoSlider.setRange (20.0, 300.0, 1.0);
    tempoSlider.setValue (120.0);
    tempoSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    tempoSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    tempoSlider.onValueChange = [this] 
    { 
        audioEngine.setTempo (tempoSlider.getValue()); 
    };
    
    addAndMakeVisible (timeDisplay);
    timeDisplay.setJustificationType (juce::Justification::centred);
    timeDisplay.setFont (juce::Font (20.0f, juce::Font::bold));
    
    startTimer (50);  // Update time display 20 times per second
}

void TransportBar::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).darker());
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced (10);
    
    playButton.setBounds (area.removeFromLeft (80));
    area.removeFromLeft (5);
    stopButton.setBounds (area.removeFromLeft (80));
    area.removeFromLeft (5);
    recordButton.setBounds (area.removeFromLeft (80));
    area.removeFromLeft (10);
    
    metronomeButton.setBounds (area.removeFromLeft (100));
    area.removeFromLeft (10);
    
    tempoLabel.setBounds (area.removeFromLeft (60));
    area.removeFromLeft (5);
    tempoSlider.setBounds (area.removeFromLeft (150));
    area.removeFromLeft (20);
    
    timeDisplay.setBounds (area.removeFromLeft (150));
}

void TransportBar::timerCallback()
{
    double position = audioEngine.getPlayheadPosition();
    int minutes = static_cast<int> (position / 60.0);
    int seconds = static_cast<int> (position) % 60;
    int milliseconds = static_cast<int> ((position - static_cast<int> (position)) * 1000);
    
    timeDisplay.setText (juce::String::formatted ("%02d:%02d.%03d", minutes, seconds, milliseconds),
                        juce::dontSendNotification);
    
    // Update button states
    playButton.setEnabled (!audioEngine.isPlaying());
    recordButton.setEnabled (!audioEngine.isRecording());
}

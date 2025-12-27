#include "AudioClip.h"

AudioClip::AudioClip (const juce::File& audioFile, double startPosSeconds)
    : file (audioFile), startPosition (startPosSeconds)
{
    name = file.getFileNameWithoutExtension();
    
    formatManager.registerBasicFormats();
    
    if (auto* reader = formatManager.createReaderFor (file))
    {
        sourceSampleRate = reader->sampleRate;
        length = reader->lengthInSamples / sourceSampleRate;
        readerSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
    }
}

void AudioClip::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    if (readerSource != nullptr)
        readerSource->prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void AudioClip::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill,
                                  double currentPlayheadPosition)
{
    bufferToFill.clearActiveBufferRegion();
    
    if (readerSource == nullptr)
        return;
    
    // Check if playhead is within this clip's range
    if (currentPlayheadPosition < startPosition || 
        currentPlayheadPosition >= getEndPosition())
        return;
    
    // Calculate offset into the clip
    double clipOffset = currentPlayheadPosition - startPosition;
    auto sampleRate = readerSource->getAudioFormatReader()->sampleRate;
    auto startSample = static_cast<juce::int64> (clipOffset * sampleRate);
    
    readerSource->setNextReadPosition (startSample);
    readerSource->getNextAudioBlock (bufferToFill);
}

void AudioClip::releaseResources()
{
    if (readerSource != nullptr)
        readerSource->releaseResources();
}

juce::var AudioClip::getState() const
{
    auto state = new juce::DynamicObject();
    state->setProperty ("name", name);
    state->setProperty ("file", file.getFullPathName());
    state->setProperty ("startPosition", startPosition);
    state->setProperty ("length", length);
    return juce::var (state);
}

void AudioClip::setState (const juce::var& state)
{
    if (auto* obj = state.getDynamicObject())
    {
        name = obj->getProperty ("name").toString();
        startPosition = obj->getProperty ("startPosition");
        length = obj->getProperty ("length");
    }
}

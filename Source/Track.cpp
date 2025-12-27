#include "Track.h"

Track::Track (const juce::String& trackName)
    : name (trackName)
{
    colour = juce::Colours::grey;
}

void Track::addClip (std::unique_ptr<AudioClip> clip)
{
    clips.add (clip.release());
}

void Track::removeClip (int index)
{
    if (juce::isPositiveAndBelow (index, clips.size()))
        clips.remove (index);
}

AudioClip* Track::getClip (int index) const
{
    return clips[index];
}

void Track::prepareToPlay (int samplesPerBlockExpected, double sr)
{
    sampleRate = sr;
    tempBuffer.setSize (2, samplesPerBlockExpected);
    
    for (auto* clip : clips)
        clip->prepareToPlay (samplesPerBlockExpected, sampleRate);
    
    for (auto* plugin : plugins)
        plugin->prepareToPlay (sampleRate, samplesPerBlockExpected);
}

void Track::processBlock (juce::AudioBuffer<float>& buffer, double currentPlayheadPosition)
{
    if (muted)
    {
        buffer.clear();
        return;
    }
    
    // Start with silence
    buffer.clear();
    
    // Mix in all clips that are active at current playhead position
    for (auto* clip : clips)
    {
        tempBuffer.clear();
        
        juce::AudioSourceChannelInfo info;
        info.buffer = &tempBuffer;
        info.startSample = 0;
        info.numSamples = buffer.getNumSamples();
        
        clip->getNextAudioBlock (info, currentPlayheadPosition);
        
        // Add clip audio to track buffer
        for (int ch = 0; ch < juce::jmin (buffer.getNumChannels(), tempBuffer.getNumChannels()); ++ch)
            buffer.addFrom (ch, 0, tempBuffer, ch, 0, buffer.getNumSamples());
    }
    
    // Apply gain
    buffer.applyGain (gain);
    
    // Apply pan (simple constant power panning)
    if (buffer.getNumChannels() >= 2)
    {
        float leftGain = std::cos ((pan + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
        float rightGain = std::sin ((pan + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
        
        buffer.applyGain (0, 0, buffer.getNumSamples(), leftGain);
        buffer.applyGain (1, 0, buffer.getNumSamples(), rightGain);
    }
    
    // Process through plugin chain
    for (auto* plugin : plugins)
    {
        juce::MidiBuffer midiMessages;
        plugin->processBlock (buffer, midiMessages);
    }
    
    // Update level meter (simple RMS)
    currentLevel = buffer.getRMSLevel (0, 0, buffer.getNumSamples());
}

void Track::releaseResources()
{
    for (auto* clip : clips)
        clip->releaseResources();
    
    for (auto* plugin : plugins)
        plugin->releaseResources();
}

void Track::addPlugin (std::unique_ptr<juce::AudioPluginInstance> plugin)
{
    if (plugin != nullptr)
    {
        plugin->prepareToPlay (sampleRate, tempBuffer.getNumSamples());
        plugins.add (plugin.release());
    }
}

void Track::removePlugin (int index)
{
    if (juce::isPositiveAndBelow (index, plugins.size()))
        plugins.remove (index);
}

int Track::getNumPlugins() const
{
    return plugins.size();
}

juce::AudioPluginInstance* Track::getPlugin (int index) const
{
    return plugins[index];
}

juce::var Track::getState() const
{
    auto state = new juce::DynamicObject();
    state->setProperty ("name", name);
    state->setProperty ("colour", colour.toString());
    state->setProperty ("muted", muted);
    state->setProperty ("soloed", soloed);
    state->setProperty ("armed", armed);
    state->setProperty ("gain", gain);
    state->setProperty ("pan", pan);
    
    juce::Array<juce::var> clipsArray;
    for (auto* clip : clips)
        clipsArray.add (clip->getState());
    state->setProperty ("clips", clipsArray);
    
    juce::Array<juce::var> pluginsArray;
    for (auto* plugin : plugins)
    {
        juce::MemoryBlock pluginState;
        plugin->getStateInformation (pluginState);
        
        auto pluginData = new juce::DynamicObject();
        pluginData->setProperty ("name", plugin->getName());
        pluginData->setProperty ("state", pluginState.toBase64Encoding());
        pluginsArray.add (juce::var (pluginData));
    }
    state->setProperty ("plugins", pluginsArray);
    
    return juce::var (state);
}

void Track::setState (const juce::var& state)
{
    if (auto* obj = state.getDynamicObject())
    {
        name = obj->getProperty ("name").toString();
        colour = juce::Colour::fromString (obj->getProperty ("colour").toString());
        muted = obj->getProperty ("muted");
        soloed = obj->getProperty ("soloed");
        armed = obj->getProperty ("armed");
        gain = obj->getProperty ("gain");
        pan = obj->getProperty ("pan");
        
        // Note: Clips and plugins would need to be restored from the state
        // This requires the audio files to still be available and plugins to be scanned
    }
}

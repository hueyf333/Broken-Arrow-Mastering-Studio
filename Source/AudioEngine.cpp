#include "AudioEngine.h"

AudioEngine::AudioEngine()
    : writerThread ("Audio Writer")
{
    writerThread.startThread();
}

AudioEngine::~AudioEngine()
{
    deviceManager.removeAudioCallback (this);
    writerThread.stopThread (1000);
}

void AudioEngine::initialiseAudioDevice()
{
    deviceManager.initialiseWithDefaultDevices (2, 2);
    deviceManager.addAudioCallback (this);
}

void AudioEngine::play()
{
    playing = true;
    recording = false;
}

void AudioEngine::stop()
{
    playing = false;
    recording = false;
    
    // Stop all active recordings
    activeRecordings.clear();
}

void AudioEngine::record()
{
    playing = true;
    recording = true;
    
    // Start recording on armed tracks
    juce::WavAudioFormat wavFormat;
    
    for (int i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->isArmed())
        {
            // Create a temporary file for recording
            juce::File recordingFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
                                           .getChildFile ("Recording_" + juce::String (i) + ".wav");
            
            recordingFile.deleteFile();
            
            if (auto* fileStream = recordingFile.createOutputStream())
            {
                if (auto* writer = wavFormat.createWriterFor (fileStream, sampleRate, 2, 16, {}, 0))
                {
                    auto* threadedWriter = new juce::AudioFormatWriter::ThreadedWriter (writer, writerThread, 32768);
                    activeRecordings.add (threadedWriter);
                }
            }
        }
        else
        {
            activeRecordings.add (nullptr);
        }
    }
}

void AudioEngine::addTrack (const juce::String& name)
{
    auto track = std::make_unique<Track> (name);
    track->setColour (juce::Colours::lightblue);
    
    if (sampleRate > 0)
        track->prepareToPlay (bufferSize, sampleRate);
    
    tracks.add (track.release());
    sendChangeMessage();
}

void AudioEngine::removeTrack (int index)
{
    if (juce::isPositiveAndBelow (index, tracks.size()))
    {
        tracks.remove (index);
        sendChangeMessage();
    }
}

Track* AudioEngine::getTrack (int index) const
{
    return tracks[index];
}

void AudioEngine::importAudioToTrack (int trackIndex, const juce::File& audioFile, double startPosition)
{
    if (juce::isPositiveAndBelow (trackIndex, tracks.size()))
    {
        auto clip = std::make_unique<AudioClip> (audioFile, startPosition);
        clip->prepareToPlay (bufferSize, sampleRate);
        tracks[trackIndex]->addClip (std::move (clip));
        sendChangeMessage();
    }
}

void AudioEngine::exportMixdown (const juce::File& outputFile, double startTime, double endTime)
{
    outputFile.deleteFile();
    
    juce::WavAudioFormat wavFormat;
    
    if (auto* fileStream = outputFile.createOutputStream())
    {
        if (auto* writer = wavFormat.createWriterFor (fileStream, sampleRate, 2, 24, {}, 0))
        {
            const int numSamples = static_cast<int> ((endTime - startTime) * sampleRate);
            const int blockSize = 512;
            juce::AudioBuffer<float> renderBuffer (2, blockSize);
            
            double currentPos = startTime;
            
            for (int samplesRendered = 0; samplesRendered < numSamples;)
            {
                int samplesToRender = juce::jmin (blockSize, numSamples - samplesRendered);
                renderBuffer.clear();
                
                // Mix all tracks
                for (auto* track : tracks)
                {
                    juce::AudioBuffer<float> trackBuffer (2, samplesToRender);
                    track->processBlock (trackBuffer, currentPos);
                    
                    for (int ch = 0; ch < 2; ++ch)
                        renderBuffer.addFrom (ch, 0, trackBuffer, ch, 0, samplesToRender);
                }
                
                writer->writeFromAudioSampleBuffer (renderBuffer, 0, samplesToRender);
                
                samplesRendered += samplesToRender;
                currentPos += samplesToRender / sampleRate;
            }
            
            delete writer;
        }
    }
}

void AudioEngine::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    bufferSize = device->getCurrentBufferSizeSamples();
    
    mixBuffer.setSize (2, bufferSize);
    
    for (auto* track : tracks)
        track->prepareToPlay (bufferSize, sampleRate);
}

void AudioEngine::audioDeviceStopped()
{
    for (auto* track : tracks)
        track->releaseResources();
}

void AudioEngine::audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                    int numInputChannels,
                                                    float* const* outputChannelData,
                                                    int numOutputChannels,
                                                    int numSamples,
                                                    const juce::AudioIODeviceCallbackContext& context)
{
    juce::ignoreUnused (context);
    
    // Clear output
    for (int i = 0; i < numOutputChannels; ++i)
        juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
    
    if (!playing)
        return;
    
    mixBuffer.setSize (numOutputChannels, numSamples, false, true, true);
    mixBuffer.clear();
    
    // Check for soloed tracks
    bool anySoloed = false;
    for (auto* track : tracks)
    {
        if (track->isSoloed())
        {
            anySoloed = true;
            break;
        }
    }
    
    // Mix all tracks
    for (int i = 0; i < tracks.size(); ++i)
    {
        auto* track = tracks[i];
        
        // Skip muted tracks, or non-soloed tracks when solo is active
        if (track->isMuted() || (anySoloed && !track->isSoloed()))
            continue;
        
        juce::AudioBuffer<float> trackBuffer (numOutputChannels, numSamples);
        trackBuffer.clear();
        
        track->processBlock (trackBuffer, playheadPosition);
        
        // Add to mix
        for (int ch = 0; ch < juce::jmin (numOutputChannels, trackBuffer.getNumChannels()); ++ch)
            mixBuffer.addFrom (ch, 0, trackBuffer, ch, 0, numSamples);
    }
    
    // Copy mix to output
    for (int ch = 0; ch < numOutputChannels; ++ch)
        juce::FloatVectorOperations::copy (outputChannelData[ch], 
                                          mixBuffer.getReadPointer (ch), 
                                          numSamples);
    
    // Handle recording
    if (recording && numInputChannels >= 2)
    {
        for (int i = 0; i < tracks.size(); ++i)
        {
            if (activeRecordings[i] != nullptr && tracks[i]->isArmed())
            {
                juce::AudioBuffer<float> recordBuffer (2, numSamples);
                
                for (int ch = 0; ch < juce::jmin (2, numInputChannels); ++ch)
                    recordBuffer.copyFrom (ch, 0, inputChannelData[ch], numSamples);
                
                activeRecordings[i]->write (recordBuffer.getArrayOfReadPointers(), numSamples);
            }
        }
    }
    
    // Simple metronome (click on beat)
    if (metronomeEnabled && playing)
    {
        double samplesPerBeat = (60.0 / tempo) * sampleRate;
        
        for (int i = 0; i < numSamples; ++i)
        {
            if (static_cast<int> (playheadPosition * sampleRate + i) % static_cast<int> (samplesPerBeat) == 0)
            {
                for (int ch = 0; ch < numOutputChannels; ++ch)
                    outputChannelData[ch][i] += 0.1f;
            }
        }
    }
    
    // Advance playhead
    playheadPosition += numSamples / sampleRate;
}

juce::var AudioEngine::getState() const
{
    auto state = new juce::DynamicObject();
    state->setProperty ("tempo", tempo);
    state->setProperty ("playheadPosition", playheadPosition);
    state->setProperty ("metronomeEnabled", metronomeEnabled);
    
    juce::Array<juce::var> tracksArray;
    for (auto* track : tracks)
        tracksArray.add (track->getState());
    state->setProperty ("tracks", tracksArray);
    
    return juce::var (state);
}

void AudioEngine::setState (const juce::var& state)
{
    if (auto* obj = state.getDynamicObject())
    {
        tempo = obj->getProperty ("tempo");
        playheadPosition = obj->getProperty ("playheadPosition");
        metronomeEnabled = obj->getProperty ("metronomeEnabled");
        
        // Restore tracks
        tracks.clear();
        
        if (auto* tracksArray = obj->getProperty ("tracks").getArray())
        {
            for (auto& trackState : *tracksArray)
            {
                auto track = std::make_unique<Track>();
                track->setState (trackState);
                track->prepareToPlay (bufferSize, sampleRate);
                tracks.add (track.release());
            }
        }
        
        sendChangeMessage();
    }
}

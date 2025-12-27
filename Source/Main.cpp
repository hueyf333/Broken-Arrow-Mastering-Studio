#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

using namespace juce;

namespace
{
constexpr int kDefaultSampleRate = 44100;
constexpr int kDefaultBlockSize = 512;
}

class Track
{
public:
    Track(String nameIn, File fileIn, std::unique_ptr<AudioFormatReaderSource> readerSourceIn)
        : name(std::move(nameIn)), file(std::move(fileIn)), readerSource(std::move(readerSourceIn))
    {
        transport.setSource(readerSource.get(), 0, nullptr, readerSource->getAudioFormatReader()->sampleRate);
    }

    void prepareToPlay(int samplesPerBlock, double sampleRate)
    {
        transport.prepareToPlay(samplesPerBlock, sampleRate);
    }

    void releaseResources()
    {
        transport.releaseResources();
    }

    void addToMixer(MixerAudioSource& mixer)
    {
        mixer.addInputSource(&transport, false);
    }

    void removeFromMixer(MixerAudioSource& mixer)
    {
        mixer.removeInputSource(&transport);
    }

    void start()
    {
        transport.start();
    }

    void stop()
    {
        transport.stop();
        transport.setPosition(0.0);
    }

    void setPosition(double positionSeconds)
    {
        transport.setPosition(positionSeconds);
    }

    double getLengthSeconds() const
    {
        return transport.getLengthInSeconds();
    }

    String getName() const
    {
        return name;
    }

    const File& getFile() const
    {
        return file;
    }

private:
    String name;
    File file;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transport;
};

class TrackMixer
{
public:
    void prepareToPlay(int samplesPerBlock, double sampleRate)
    {
        currentBlockSize = samplesPerBlock;
        currentSampleRate = sampleRate;
        isPrepared = true;
        mixer.prepareToPlay(samplesPerBlock, sampleRate);
        for (auto& track : tracks)
            track->prepareToPlay(samplesPerBlock, sampleRate);
    }

    void releaseResources()
    {
        for (auto& track : tracks)
            track->releaseResources();
        mixer.releaseResources();
        isPrepared = false;
    }

    void addTrack(std::unique_ptr<Track> track)
    {
        if (isPrepared)
            track->prepareToPlay(currentBlockSize, currentSampleRate);
        track->addToMixer(mixer);
        tracks.push_back(std::move(track));
    }

    void clearTracks()
    {
        for (auto& track : tracks)
            track->removeFromMixer(mixer);
        tracks.clear();
    }

    void startAll()
    {
        for (auto& track : tracks)
            track->start();
    }

    void stopAll()
    {
        for (auto& track : tracks)
            track->stop();
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& info)
    {
        mixer.getNextAudioBlock(info);
    }

    StringArray getTrackNames() const
    {
        StringArray names;
        for (const auto& track : tracks)
            names.add(track->getName());
        return names;
    }

    const std::vector<std::unique_ptr<Track>>& getTracks() const
    {
        return tracks;
    }

private:
    MixerAudioSource mixer;
    std::vector<std::unique_ptr<Track>> tracks;
    int currentBlockSize = 0;
    double currentSampleRate = 0.0;
    bool isPrepared = false;
};

class SourceAudioProcessor : public AudioProcessor
{
public:
    explicit SourceAudioProcessor(TrackMixer& mixerIn)
        : mixer(mixerIn)
    {
    }

    const String getName() const override
    {
        return "TrackMixerSource";
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        mixer.prepareToPlay(samplesPerBlock, sampleRate);
    }

    void releaseResources() override
    {
        mixer.releaseResources();
    }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override
    {
        return layouts.getMainOutputChannelSet() == AudioChannelSet::stereo();
    }

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override
    {
        buffer.clear();
        AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
        mixer.getNextAudioBlock(info);
    }

    AudioProcessorEditor* createEditor() override
    {
        return nullptr;
    }

    bool hasEditor() const override
    {
        return false;
    }

    double getTailLengthSeconds() const override
    {
        return 0.0;
    }

    int getNumPrograms() override
    {
        return 1;
    }

    int getCurrentProgram() override
    {
        return 0;
    }

    void setCurrentProgram(int) override
    {
    }

    const String getProgramName(int) override
    {
        return {};
    }

    void changeProgramName(int, const String&) override
    {
    }

    void getStateInformation(MemoryBlock&) override
    {
    }

    void setStateInformation(const void*, int) override
    {
    }

private:
    TrackMixer& mixer;
};

class GraphManager
{
public:
    explicit GraphManager(TrackMixer& mixer)
        : graph(std::make_unique<AudioProcessorGraph>())
    {
        formatManager.addDefaultFormats();
        graph->setPlayConfigDetails(0, 2, kDefaultSampleRate, kDefaultBlockSize);
        sourceNode = graph->addNode(std::make_unique<SourceAudioProcessor>(mixer));
        outputNode = graph->addNode(std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor>(
            AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
        rebuildGraph();
    }

    void setSampleRate(double sampleRate, int blockSize)
    {
        graph->setPlayConfigDetails(0, 2, sampleRate, blockSize);
    }

    AudioProcessorGraph& getGraph() const
    {
        return *graph;
    }

    AudioPluginFormatManager& getFormatManager()
    {
        return formatManager;
    }

    void addPlugin(std::unique_ptr<AudioPluginInstance> instance)
    {
        if (instance == nullptr)
            return;

        auto node = graph->addNode(std::move(instance));
        pluginNodes.add(node);
        rebuildGraph();
    }

    void clearPlugins()
    {
        pluginNodes.clear();
        rebuildGraph();
    }

    void rebuildGraph()
    {
        graph->clearConnections();

        AudioProcessorGraph::NodeID lastNode = sourceNode->nodeID;
        if (pluginNodes.isEmpty())
        {
            connectNodes(lastNode, outputNode->nodeID);
            return;
        }

        for (auto* node : pluginNodes)
        {
            connectNodes(lastNode, node->nodeID);
            lastNode = node->nodeID;
        }

        connectNodes(lastNode, outputNode->nodeID);
    }

private:
    void connectNodes(AudioProcessorGraph::NodeID source, AudioProcessorGraph::NodeID destination)
    {
        for (int channel = 0; channel < 2; ++channel)
            graph->addConnection({ { source, channel }, { destination, channel } });
    }

    std::unique_ptr<AudioProcessorGraph> graph;
    AudioProcessorGraph::Node::Ptr sourceNode;
    AudioProcessorGraph::Node::Ptr outputNode;
    Array<AudioProcessorGraph::Node::Ptr> pluginNodes;
    AudioPluginFormatManager formatManager;
};

class AudioRecorder : public AudioIODeviceCallback
{
public:
    explicit AudioRecorder(AudioFormatManager& formatManagerIn)
        : formatManager(formatManagerIn), backgroundThread("Audio Recorder Thread")
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder() override
    {
        stop();
    }

    void startRecording(const File& file)
    {
        stop();

        if (auto fileStream = std::unique_ptr<FileOutputStream>(file.createOutputStream()))
        {
            WavAudioFormat wavFormat;
            if (auto* writer = wavFormat.createWriterFor(fileStream.get(),
                                                        sampleRate,
                                                        2,
                                                        24,
                                                        {},
                                                        0))
            {
                fileStream.release();
                threadedWriter.reset(new AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));
                nextSampleNum = 0;
            }
        }
    }

    void stop()
    {
        threadedWriter.reset();
    }

    void audioDeviceAboutToStart(AudioIODevice* device) override
    {
        sampleRate = device->getCurrentSampleRate();
    }

    void audioDeviceStopped() override
    {
        sampleRate = 0.0;
    }

    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numSamples) override
    {
        for (int channel = 0; channel < numOutputChannels; ++channel)
            if (outputChannelData[channel] != nullptr)
                FloatVectorOperations::clear(outputChannelData[channel], numSamples);

        if (threadedWriter == nullptr)
            return;

        AudioBuffer<float> buffer(numInputChannels, numSamples);
        for (int channel = 0; channel < numInputChannels; ++channel)
            if (auto* data = inputChannelData[channel])
                buffer.copyFrom(channel, 0, data, numSamples);

        threadedWriter->write(buffer.getArrayOfReadPointers(), numSamples);
        nextSampleNum += numSamples;
    }

private:
    AudioFormatManager& formatManager;
    TimeSliceThread backgroundThread;
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter;
    double sampleRate = 0.0;
    int64 nextSampleNum = 0;
};

class AudioEngine
{
public:
    AudioEngine()
        : graphManager(trackMixer), recorder(formatManager)
    {
        formatManager.addDefaultFormats();
        deviceManager.initialise(2, 2, nullptr, true);
        if (auto* device = deviceManager.getCurrentAudioDevice())
            graphManager.setSampleRate(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
        deviceManager.addAudioCallback(&audioPlayer);
        deviceManager.addAudioCallback(&recorder);
        audioPlayer.setProcessor(&graphManager.getGraph());
    }

    ~AudioEngine()
    {
        audioPlayer.setProcessor(nullptr);
        deviceManager.removeAudioCallback(&audioPlayer);
        deviceManager.removeAudioCallback(&recorder);
    }

    void play()
    {
        trackMixer.startAll();
    }

    void stop()
    {
        trackMixer.stopAll();
    }

    void startRecording(const File& file)
    {
        recorder.startRecording(file);
    }

    void stopRecording()
    {
        recorder.stop();
    }

    bool addTrackFromFile(const File& file)
    {
        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
        if (reader == nullptr)
            return false;

        auto readerSource = std::make_unique<AudioFormatReaderSource>(reader.release(), true);
        auto track = std::make_unique<Track>(file.getFileNameWithoutExtension(), file, std::move(readerSource));
        trackMixer.addTrack(std::move(track));
        return true;
    }

    bool exportMixdown(const File& file)
    {
        const auto sampleRate = deviceManager.getCurrentAudioDevice() != nullptr
                                    ? deviceManager.getCurrentAudioDevice()->getCurrentSampleRate()
                                    : static_cast<double>(kDefaultSampleRate);

        WavAudioFormat wavFormat;
        std::unique_ptr<FileOutputStream> stream(file.createOutputStream());
        if (stream == nullptr)
            return false;

        std::unique_ptr<AudioFormatWriter> writer(
            wavFormat.createWriterFor(stream.get(), sampleRate, 2, 24, {}, 0));

        if (writer == nullptr)
            return false;

        stream.release();

        constexpr int renderBlockSize = 512;
        AudioBuffer<float> buffer(2, renderBlockSize);
        AudioSourceChannelInfo info(&buffer, 0, renderBlockSize);

        trackMixer.prepareToPlay(renderBlockSize, sampleRate);
        trackMixer.startAll();

        int64 totalSamples = 0;
        double maxLengthSeconds = 0.0;
        for (const auto& track : trackMixer.getTracks())
            maxLengthSeconds = jmax(maxLengthSeconds, track->getLengthSeconds());

        int64 maxSamples = static_cast<int64>(sampleRate * jmax(1.0, maxLengthSeconds));

        while (totalSamples < maxSamples)
        {
            buffer.clear();
            trackMixer.getNextAudioBlock(info);
            writer->writeFromAudioSampleBuffer(buffer, 0, renderBlockSize);
            totalSamples += renderBlockSize;
        }

        trackMixer.stopAll();
        trackMixer.releaseResources();
        return true;
    }

    TrackMixer& getMixer()
    {
        return trackMixer;
    }

    GraphManager& getGraphManager()
    {
        return graphManager;
    }

    KnownPluginList& getKnownPlugins()
    {
        return knownPlugins;
    }

    AudioDeviceManager& getDeviceManager()
    {
        return deviceManager;
    }

private:
    AudioDeviceManager deviceManager;
    AudioFormatManager formatManager;
    TrackMixer trackMixer;
    GraphManager graphManager;
    AudioProcessorPlayer audioPlayer;
    AudioRecorder recorder;
    KnownPluginList knownPlugins;
};

class TrackListModel : public ListBoxModel
{
public:
    explicit TrackListModel(TrackMixer& mixerIn)
        : mixer(mixerIn)
    {
    }

    int getNumRows() override
    {
        return static_cast<int>(mixer.getTracks().size());
    }

    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(Colours::darkslategrey);

        g.setColour(Colours::white);
        if (isPositiveAndBelow(rowNumber, mixer.getTracks().size()))
        {
            auto name = mixer.getTracks()[static_cast<size_t>(rowNumber)]->getName();
            g.drawText(name, 8, 0, width - 16, height, Justification::centredLeft);
        }
    }

private:
    TrackMixer& mixer;
};

class PluginListWindow : public DocumentWindow
{
public:
    PluginListWindow(KnownPluginList& list, AudioPluginFormatManager& formatManager)
        : DocumentWindow("Plugin Manager", Colours::black, DocumentWindow::closeButton)
    {
        setUsingNativeTitleBar(true);
        auto* component = new PluginListComponent(formatManager, list, false, true);
        setContentOwned(component, true);
        centreWithSize(600, 500);
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        setVisible(false);
    }
};

class MainComponent : public Component, public MenuBarModel
{
public:
    MainComponent()
        : trackListModel(audioEngine.getMixer()), trackList("Tracks", &trackListModel)
    {
        addAndMakeVisible(menuBar);
        addAndMakeVisible(trackList);
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);
        addAndMakeVisible(recordButton);
        addAndMakeVisible(importButton);
        addAndMakeVisible(exportButton);

        playButton.onClick = [this] { audioEngine.play(); };
        stopButton.onClick = [this] { audioEngine.stop(); };
        recordButton.onClick = [this] { toggleRecording(); };
        importButton.onClick = [this] { importAudio(); };
        exportButton.onClick = [this] { exportAudio(); };

        setSize(900, 600);
    }

    ~MainComponent() override
    {
        menuBar.setModel(nullptr);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        menuBar.setBounds(bounds.removeFromTop(24));

        auto controlArea = bounds.removeFromTop(48).reduced(8);
        playButton.setBounds(controlArea.removeFromLeft(90));
        stopButton.setBounds(controlArea.removeFromLeft(90));
        recordButton.setBounds(controlArea.removeFromLeft(110));
        importButton.setBounds(controlArea.removeFromLeft(110));
        exportButton.setBounds(controlArea.removeFromLeft(110));

        trackList.setBounds(bounds.reduced(8));
    }

    StringArray getMenuBarNames() override
    {
        return { "File", "Plugins", "Help" };
    }

    PopupMenu getMenuForIndex(int index, const String&) override
    {
        PopupMenu menu;
        if (index == 0)
        {
            menu.addItem(1, "Import Audio...");
            menu.addItem(2, "Export Mixdown...");
            menu.addSeparator();
            menu.addItem(3, "Quit");
        }
        else if (index == 1)
        {
            menu.addItem(10, "Plugin Manager...");
            menu.addItem(11, "Clear Plugin Chain");
            menu.addSeparator();
            PopupMenu addMenu;
            const auto& types = audioEngine.getKnownPlugins().getTypes();
            for (int i = 0; i < types.size(); ++i)
                addMenu.addItem(kPluginBaseId + i, types.getReference(i).name);
            menu.addSubMenu("Add Plugin", addMenu, types.size() > 0);
        }
        else if (index == 2)
        {
            menu.addItem(20, "About");
        }
        return menu;
    }

    void menuItemSelected(int menuItemID, int) override
    {
        if (menuItemID == 1)
            importAudio();
        else if (menuItemID == 2)
            exportAudio();
        else if (menuItemID == 3)
            JUCEApplication::getInstance()->systemRequestedQuit();
        else if (menuItemID == 10)
            openPluginManager();
        else if (menuItemID == 11)
            audioEngine.getGraphManager().clearPlugins();
        else if (menuItemID >= kPluginBaseId)
            addPlugin(menuItemID - kPluginBaseId);
        else if (menuItemID == 20)
            AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "About",
                                             "Broken Arrow Mastering Studio\nMultitrack Recording System");
    }

private:
    void addPlugin(int pluginIndex)
    {
        const auto& types = audioEngine.getKnownPlugins().getTypes();
        if (!isPositiveAndBelow(pluginIndex, types.size()))
            return;

        String errorMessage;
        auto sampleRate = static_cast<double>(kDefaultSampleRate);
        int blockSize = kDefaultBlockSize;
        if (auto* device = audioEngine.getDeviceManager().getCurrentAudioDevice())
        {
            sampleRate = device->getCurrentSampleRate();
            blockSize = device->getCurrentBufferSizeSamples();
        }

        auto instance = audioEngine.getGraphManager().getFormatManager().createPluginInstance(
            types.getReference(pluginIndex), sampleRate, blockSize, errorMessage);
        if (instance == nullptr)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Plugin Error", errorMessage);
            return;
        }

        audioEngine.getGraphManager().addPlugin(std::move(instance));
    }

    void toggleRecording()
    {
        if (!isRecording)
        {
            recordingFile = File::getSpecialLocation(File::userDocumentsDirectory)
                                .getNonexistentChildFile("recording", ".wav");
            audioEngine.startRecording(recordingFile);
            isRecording = true;
            recordButton.setButtonText("Stop Recording");
        }
        else
        {
            audioEngine.stopRecording();
            isRecording = false;
            recordButton.setButtonText("Record");
            audioEngine.addTrackFromFile(recordingFile);
            trackList.updateContent();
        }
    }

    void importAudio()
    {
        FileChooser chooser("Import Audio", File(), "*.wav;*.aiff;*.flac");
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            if (audioEngine.addTrackFromFile(file))
                trackList.updateContent();
        }
    }

    void exportAudio()
    {
        FileChooser chooser("Export Mixdown", File::getSpecialLocation(File::userDocumentsDirectory), "*.wav");
        if (chooser.browseForFileToSave(true))
        {
            auto file = chooser.getResult();
            audioEngine.exportMixdown(file);
        }
    }

    void openPluginManager()
    {
        if (pluginWindow == nullptr)
            pluginWindow = std::make_unique<PluginListWindow>(audioEngine.getKnownPlugins(),
                                                              audioEngine.getGraphManager().getFormatManager());
        pluginWindow->setVisible(true);
    }

    AudioEngine audioEngine;
    MenuBarComponent menuBar { this };
    TrackListModel trackListModel;
    ListBox trackList;

    TextButton playButton { "Play" };
    TextButton stopButton { "Stop" };
    TextButton recordButton { "Record" };
    TextButton importButton { "Import" };
    TextButton exportButton { "Export" };

    bool isRecording = false;
    std::unique_ptr<PluginListWindow> pluginWindow;
    File recordingFile;

    static constexpr int kPluginBaseId = 1000;
};

class MainWindow : public DocumentWindow
{
public:
    MainWindow(String name)
        : DocumentWindow(std::move(name), Colours::black, DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(), true);
        centreWithSize(getWidth(), getHeight());
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

class BrokenArrowMasteringStudioApplication : public JUCEApplication
{
public:
    BrokenArrowMasteringStudioApplication() = default;

    const String getApplicationName() override
    {
        return "Broken Arrow Mastering Studio";
    }

    const String getApplicationVersion() override
    {
        return "1.0.0";
    }

    void initialise(const String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(BrokenArrowMasteringStudioApplication)

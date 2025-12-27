# Broken Arrow Mastering Studio

A full-featured multitrack recording system built with the JUCE Framework. This DAW-like application provides professional audio recording, editing, mixing, and plugin hosting capabilities.

## Features

### Core Functionality
- **Multitrack Recording & Playback**: Record from audio inputs and play back multiple tracks simultaneously
- **Audio Import/Export**: Support for WAV, AIFF, FLAC, and other common audio formats
- **Plugin Hosting**: VST3 and Audio Unit (AU) plugin support with automatic scanning
- **Professional Mixing**: Individual track controls with gain, pan, mute, solo, and arm
- **Real-time Metering**: Visual level meters for monitoring audio levels
- **Transport Controls**: Standard play, stop, record controls with time display
- **Metronome**: Built-in click track for timing reference
- **Project Management**: Save and load complete project files with all settings

### User Interface
- **Timeline View**: Visual representation of tracks and clips over time
- **Mixer View**: Traditional mixer interface with faders and controls
- **Transport Bar**: Quick access to playback and recording controls
- **Menu System**: Comprehensive menus with keyboard shortcuts

## Requirements

### Software
- **JUCE Framework**: Included as a git submodule (version 7.x or later)
- **Projucer**: JUCE's project management tool (comes with JUCE)
- **C++20 Compiler**:
  - macOS: Xcode 12.5 or later
  - Windows: Visual Studio 2022 or later
  - Linux: GCC 10+ or Clang 11+

### Hardware
- Audio interface (recommended for recording)
- Sufficient disk space for audio files

## Building the Project

### 1. Clone the Repository

```bash
git clone --recursive https://github.com/hueyf333/Broken-Arrow-Mastering-Studio.git
cd Broken-Arrow-Mastering-Studio
```

If you've already cloned without `--recursive`, initialize the submodules:

```bash
git submodule update --init --recursive
```

### 2. Open in Projucer

1. Navigate to the `JUCE` directory and run the Projucer application:
   - macOS: `JUCE/extras/Projucer/Builds/MacOSX/build/Debug/Projucer.app`
   - Windows: `JUCE\extras\Projucer\Builds\VisualStudio2022\x64\Debug\App\Projucer.exe`
   - Linux: Build Projucer first from `JUCE/extras/Projucer/Builds/LinuxMakefile`

2. Open `BrokenArrowStudio.jucer` in Projucer

3. Click "Save Project and Open in IDE" to generate native project files

### 3. Build from IDE

#### macOS (Xcode)
1. The Xcode project will be in `Builds/MacOSX/`
2. Open `BrokenArrowStudio.xcodeproj`
3. Select your build configuration (Debug or Release)
4. Build and run (Cmd+R)

#### Windows (Visual Studio 2022)
1. The Visual Studio solution will be in `Builds/VisualStudio2022/`
2. Open `BrokenArrowStudio.sln`
3. Select your build configuration (Debug or Release)
4. Build and run (F5)

#### Linux (Makefile)
```bash
cd Builds/LinuxMakefile
make CONFIG=Release
./build/BrokenArrowStudio
```

## Usage Guide

### Getting Started

1. **Launch the Application**: Run the built executable
2. **Configure Audio**: The application will automatically use your default audio device
3. **Create Tracks**: The application starts with two default tracks, or use File → New Project

### Basic Workflow

#### Recording Audio
1. Click the "R" (Arm) button on the track you want to record to
2. Click the red "Record" button in the transport bar
3. Play your audio source
4. Click "Stop" when finished
5. Your recording will appear as a clip on the timeline

#### Importing Audio
1. Go to File → Import Audio (or press Cmd/Ctrl+I)
2. Select an audio file (WAV, AIFF, FLAC, etc.)
3. The file will be imported to Track 1 at the current playhead position

#### Mixing Tracks
1. Use the mixer view (bottom panel) to adjust each track:
   - **Gain Slider**: Adjust track volume (0-2x)
   - **Pan Knob**: Position track in stereo field (-1 left to +1 right)
   - **M Button**: Mute track
   - **S Button**: Solo track (only soloed tracks are heard)
   - **Level Meter**: Shows current audio level

#### Working with Plugins
1. Go to Plugins → Scan for Plugins to discover installed plugins
2. Plugins can be added to tracks via the plugin chain system
3. Supported formats: VST3, Audio Units (macOS)

#### Project Management
- **New Project**: File → New Project (Cmd/Ctrl+N)
- **Save Project**: File → Save Project (Cmd/Ctrl+S)
- **Open Project**: File → Open Project (Cmd/Ctrl+O)
- **Export Mixdown**: File → Export Mixdown (Cmd/Ctrl+E)

### Keyboard Shortcuts

- **Cmd/Ctrl+N**: New Project
- **Cmd/Ctrl+O**: Open Project
- **Cmd/Ctrl+S**: Save Project
- **Cmd/Ctrl+Shift+S**: Save Project As
- **Cmd/Ctrl+I**: Import Audio
- **Cmd/Ctrl+E**: Export Mixdown
- **Cmd/Ctrl+M**: Toggle Mixer View
- **Cmd/Ctrl+T**: Toggle Timeline View
- **Cmd/Ctrl+Z**: Undo (stub)
- **Cmd/Ctrl+Shift+Z**: Redo (stub)
- **Cmd/Ctrl+Q**: Quit Application

## Project File Format

Projects are saved as `.basproj` files in JSON format containing:
- Track configurations (name, color, gain, pan, mute/solo/arm states)
- Clip references (file paths and positions)
- Plugin chain states
- Transport settings (tempo, playhead position)
- Global settings

## Architecture

The application follows a clean separation of concerns:

### Engine Layer (`AudioEngine`, `Track`, `AudioClip`)
- Manages audio I/O via JUCE's `AudioDeviceManager`
- Handles real-time audio processing
- Maintains track and clip data structures
- Implements recording and playback logic

### UI Layer (`MainComponent`, `TransportBar`, `TimelineComponent`, `MixerComponent`, `TrackComponent`)
- JUCE Component-based interface
- Updates reflect engine state changes
- User interactions trigger engine operations

### Plugin System (`PluginScanner`)
- Scans system for VST3/AU plugins
- Manages known plugin list
- Handles plugin instantiation

### Persistence (`ProjectSerializer`)
- JSON-based project file format
- Saves/loads complete project state

## Development Notes

- **C++20**: The project uses C++20 features and requires a compatible compiler
- **Audio Thread Safety**: Audio processing code avoids allocations and locks
- **JUCE Best Practices**: Follows JUCE coding standards and patterns
- **Cross-Platform**: Designed to build on macOS, Windows, and Linux

## Troubleshooting

### Build Issues
- Ensure JUCE submodule is properly initialized
- Check that your compiler supports C++20
- Verify Projucer version is up to date

### Audio Issues
- Check your system audio settings
- Ensure audio interface drivers are installed
- Try different buffer sizes in audio settings

### Plugin Issues
- Run plugin scan from Plugins menu
- Check plugin installation paths
- Some plugins may not be compatible

## Contributing

Contributions are welcome! Please ensure code follows JUCE best practices and maintains the existing architecture.

## License

See LICENSE file for details.

## Credits

Built with [JUCE Framework](https://juce.com/) - Cross-platform C++ application framework for audio applications.

## Support

For issues and feature requests, please use the GitHub issue tracker.
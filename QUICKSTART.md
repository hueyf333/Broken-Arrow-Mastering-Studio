# Quick Start Guide - Broken Arrow Studio

## Prerequisites Checklist

- [ ] Git installed
- [ ] C++20 compatible compiler (Xcode 12.5+, VS2022, or GCC 10+)
- [ ] Audio interface (optional, can use built-in audio)

## 5-Minute Setup

### Step 1: Clone and Initialize (2 min)
```bash
git clone --recursive https://github.com/hueyf333/Broken-Arrow-Mastering-Studio.git
cd Broken-Arrow-Mastering-Studio

# If you forgot --recursive:
git submodule update --init --recursive
```

### Step 2: Verify Project (1 min)
```bash
./verify_build.sh
```
Expected output: All checks passed ✓

### Step 3: Open in Projucer (1 min)

**macOS:**
```bash
open JUCE/extras/Projucer/Builds/MacOSX/build/Debug/Projucer.app BrokenArrowStudio.jucer
```

**Windows:**
```cmd
JUCE\extras\Projucer\Builds\VisualStudio2022\x64\Debug\App\Projucer.exe BrokenArrowStudio.jucer
```

**Linux:**
First build Projucer (one-time):
```bash
cd JUCE/extras/Projucer/Builds/LinuxMakefile
make CONFIG=Release
cd ../../../../..
```
Then open:
```bash
JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer BrokenArrowStudio.jucer
```

### Step 4: Export and Build (1 min)

In Projucer:
1. Click the save icon or File → Save Project
2. Click the IDE icon or File → Save and Open in IDE
3. In your IDE, select Release configuration
4. Build (Cmd+B / F7 / make)

## First Run Checklist

After launching the app:

- [ ] Window opens with menu bar
- [ ] Transport bar visible (Play/Stop/Record buttons)
- [ ] Timeline view shows tracks
- [ ] Mixer view shows track controls

## Quick Feature Test

### Import Audio
1. File → Import Audio (Cmd/Ctrl+I)
2. Select a WAV/AIFF/FLAC file
3. See clip appear on Track 1 timeline

### Playback
1. Click in timeline to set playhead
2. Click Play button
3. Hear audio

### Recording
1. Click "R" button on a track to arm
2. Click red Record button
3. Make sound/play instrument
4. Click Stop
5. See new clip on timeline

### Export
1. File → Export Mixdown (Cmd/Ctrl+E)
2. Choose location and filename
3. WAV file created

### Save Project
1. File → Save Project (Cmd/Ctrl+S)
2. Choose location
3. Creates .basproj file

## Keyboard Shortcuts Quick Reference

| Action | macOS | Windows/Linux |
|--------|-------|---------------|
| New Project | Cmd+N | Ctrl+N |
| Open Project | Cmd+O | Ctrl+O |
| Save Project | Cmd+S | Ctrl+S |
| Import Audio | Cmd+I | Ctrl+I |
| Export Mixdown | Cmd+E | Ctrl+E |
| Toggle Mixer | Cmd+M | Ctrl+M |
| Toggle Timeline | Cmd+T | Ctrl+T |
| Quit | Cmd+Q | Ctrl+Q |

## Track Controls

Each track has:
- **Name** - Click to edit
- **M** - Mute
- **S** - Solo (only soloed tracks play)
- **R** - Arm for recording
- **Pan Knob** - L/R positioning
- **Gain Slider** - Volume (0-2x)
- **Level Meter** - Shows audio level

## Transport Controls

- **Play** - Start playback
- **Stop** - Stop and reset to start
- **Record** - Start recording on armed tracks
- **Metronome** - Toggle click track
- **Tempo** - Set BPM (20-300)
- **Time Display** - Shows playhead position

## Troubleshooting

### "JUCE not found" error
```bash
git submodule update --init --recursive
```

### Build fails with compiler errors
- Check C++20 support: Update Xcode/VS2022/GCC
- Clean build folder and rebuild

### No audio heard
- Check system audio settings
- Try different buffer size
- Verify audio device is connected

### Plugins not found
1. Plugins → Scan for Plugins
2. Wait for scan to complete
3. Check standard plugin locations:
   - macOS: ~/Library/Audio/Plug-Ins/VST3
   - Windows: C:\Program Files\Common Files\VST3
   - Linux: ~/.vst3

## Need Help?

1. Check `README.md` for detailed documentation
2. Check `VALIDATION.md` for technical details
3. Review source code comments
4. Open GitHub issue

## Project Structure Overview

```
Broken-Arrow-Mastering-Studio/
├── BrokenArrowStudio.jucer    # Projucer project file
├── Source/                     # All C++ source code
│   ├── Main.cpp               # Application entry
│   ├── MainComponent.*        # Main UI + menus
│   ├── AudioEngine.*          # Audio I/O and mixing
│   ├── Track.*                # Track implementation
│   ├── AudioClip.*            # Clip/region handling
│   ├── TransportBar.*         # Play/record controls
│   ├── TimelineComponent.*    # Timeline view
│   ├── MixerComponent.*       # Mixer view
│   ├── TrackComponent.*       # Track UI strip
│   ├── PluginScanner.*        # Plugin discovery
│   └── ProjectSerializer.*    # Save/load
├── JUCE/                      # JUCE framework (submodule)
├── README.md                  # Full documentation
├── VALIDATION.md              # Technical validation
└── verify_build.sh            # Structure check script
```

---

**Ready to make music!** 🎵🎸🎹🎤

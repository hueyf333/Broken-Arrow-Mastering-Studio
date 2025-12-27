#!/bin/bash
# Build Verification Script for Broken Arrow Studio
# This script performs basic validation without requiring a full build

echo "=================================="
echo "Broken Arrow Studio - Build Verification"
echo "=================================="
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
PASSED=0
FAILED=0

# Function to check if file exists
check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} $1 - MISSING"
        ((FAILED++))
    fi
}

# Function to check if directory exists
check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} $1/ - MISSING"
        ((FAILED++))
    fi
}

# Function to grep for pattern in file
check_pattern() {
    if grep -q "$2" "$1" 2>/dev/null; then
        echo -e "${GREEN}✓${NC} $3"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} $3 - NOT FOUND"
        ((FAILED++))
    fi
}

echo "1. Checking Project Structure..."
echo "--------------------------------"
check_file "BrokenArrowStudio.jucer"
check_file "README.md"
check_file ".gitignore"
check_file ".gitmodules"
check_dir "Source"
check_dir "JUCE"
echo ""

echo "2. Checking Source Files..."
echo "--------------------------------"
check_file "Source/Main.cpp"
check_file "Source/MainComponent.h"
check_file "Source/MainComponent.cpp"
check_file "Source/AudioEngine.h"
check_file "Source/AudioEngine.cpp"
check_file "Source/Track.h"
check_file "Source/Track.cpp"
check_file "Source/AudioClip.h"
check_file "Source/AudioClip.cpp"
check_file "Source/TransportBar.h"
check_file "Source/TransportBar.cpp"
check_file "Source/TrackComponent.h"
check_file "Source/TrackComponent.cpp"
check_file "Source/TimelineComponent.h"
check_file "Source/TimelineComponent.cpp"
check_file "Source/MixerComponent.h"
check_file "Source/MixerComponent.cpp"
check_file "Source/PluginScanner.h"
check_file "Source/PluginScanner.cpp"
check_file "Source/ProjectSerializer.h"
check_file "Source/ProjectSerializer.cpp"
echo ""

echo "3. Checking .jucer Configuration..."
echo "--------------------------------"
check_pattern "BrokenArrowStudio.jucer" "cppLanguageStandard=\"20\"" "C++20 standard configured"
check_pattern "BrokenArrowStudio.jucer" "juce_audio_devices" "juce_audio_devices module"
check_pattern "BrokenArrowStudio.jucer" "juce_audio_processors" "juce_audio_processors module"
check_pattern "BrokenArrowStudio.jucer" "juce_gui_basics" "juce_gui_basics module"
check_pattern "BrokenArrowStudio.jucer" "XCODE_MAC" "macOS export target"
check_pattern "BrokenArrowStudio.jucer" "VS2022" "Windows export target"
check_pattern "BrokenArrowStudio.jucer" "LINUX_MAKE" "Linux export target"
echo ""

echo "4. Checking Key Implementation Features..."
echo "--------------------------------"
check_pattern "Source/AudioEngine.cpp" "audioDeviceIOCallbackWithContext" "Audio callback implementation"
check_pattern "Source/AudioEngine.cpp" "ThreadedWriter" "Recording capability"
check_pattern "Source/Track.cpp" "processBlock" "Track audio processing"
check_pattern "Source/AudioClip.cpp" "AudioFormatReaderSource" "Audio file reading"
check_pattern "Source/MainComponent.cpp" "MenuBarModel" "Menu system"
check_pattern "Source/MainComponent.cpp" "ApplicationCommandManager" "Command manager"
check_pattern "Source/PluginScanner.cpp" "AudioPluginFormatManager" "Plugin support"
check_pattern "Source/ProjectSerializer.cpp" "JSON" "Project persistence"
echo ""

echo "5. Checking JUCE Submodule..."
echo "--------------------------------"
if [ -f "JUCE/modules/juce_core/juce_core.h" ]; then
    echo -e "${GREEN}✓${NC} JUCE submodule initialized"
    ((PASSED++))
else
    echo -e "${YELLOW}⚠${NC} JUCE submodule not initialized (run: git submodule update --init --recursive)"
    echo "   This is normal for fresh clones"
fi
echo ""

echo "6. Checking Documentation..."
echo "--------------------------------"
check_pattern "README.md" "Build" "Build instructions"
check_pattern "README.md" "Usage" "Usage guide"
check_pattern "README.md" "Keyboard Shortcuts" "Keyboard shortcuts"
check_pattern "README.md" "JUCE Framework" "JUCE documentation"
echo ""

echo "=================================="
echo "Verification Summary"
echo "=================================="
echo -e "Passed: ${GREEN}${PASSED}${NC}"
echo -e "Failed: ${RED}${FAILED}${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All checks passed!${NC}"
    echo ""
    echo "Next steps:"
    echo "1. Ensure JUCE submodule is initialized: git submodule update --init --recursive"
    echo "2. Open BrokenArrowStudio.jucer in Projucer"
    echo "3. Click 'Save Project and Open in IDE'"
    echo "4. Build and run the project"
    exit 0
else
    echo -e "${RED}✗ Some checks failed${NC}"
    echo "Please review the errors above"
    exit 1
fi

# Simple Checklist VST3

A minimal VST3 plugin for basic task management in your DAW.

## Features

- ✅ Simple task list
- ✅ Add/remove tasks
- ✅ Check/uncheck completion
- ✅ State persistence in projects

## Installation

1. Download the latest release from GitHub Actions artifacts
2. Extract `Simple Checklist.vst3` folder
3. Copy to: `C:\Program Files\Common Files\VST3\`
4. Restart your DAW
5. Rescan plugins

## Usage

1. Load the plugin in your DAW
2. Type a task in the input box
3. Click "Add" or press Enter
4. Click checkbox to mark complete
5. Click "X" to delete a task

## Build from Source

Requires:
- CMake 3.15+
- Visual Studio 2022 (Windows)
- JUCE 8.0.4

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Support

Compatible with:
- Cubase
- FL Studio
- Ableton Live
- Reaper
- And any DAW that supports VST3

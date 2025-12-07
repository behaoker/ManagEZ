cmake_minimum_required(VERSION 3.15)

project(SimpleChecklistVST VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
add_subdirectory(JUCE)

# Create the plugin
juce_add_plugin(SimpleChecklistVST
    COMPANY_NAME "YourName"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Simp
    PLUGIN_CODE Scvt
    FORMATS VST3 Standalone
    PRODUCT_NAME "Simple Checklist"
)

# Source files
target_sources(SimpleChecklistVST
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# Compile definitions
target_compile_definitions(SimpleChecklistVST
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules - MINIMAL SET
target_link_libraries(SimpleChecklistVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Installation
if(WIN32)
    set(VST3_INSTALL_DIR "$ENV{PROGRAMFILES}/Common Files/VST3")
elseif(APPLE)
    set(VST3_INSTALL_DIR "~/Library/Audio/Plug-Ins/VST3")
else()
    set(VST3_INSTALL_DIR "~/.vst3")
endif()

install(TARGETS SimpleChecklistVST
    LIBRARY DESTINATION "${VST3_INSTALL_DIR}"
    RUNTIME DESTINATION "${VST3_INSTALL_DIR}"
)

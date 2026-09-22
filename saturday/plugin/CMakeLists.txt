cmake_minimum_required(VERSION 3.22)

project(Saturday VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SATURDAY_COPY_AFTER_BUILD "Copy built plugins to OS install folders after build" ON)

if(APPLE)
    set(SATURDAY_FORMATS AU VST3 Standalone)
elseif(WIN32)
    set(SATURDAY_FORMATS VST3 Standalone)
else()
    set(SATURDAY_FORMATS VST3 Standalone)
endif()

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.6
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Saturday
    COMPANY_NAME "Whatever555"
    BUNDLE_ID com.whatever555.saturday
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD ${SATURDAY_COPY_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE Whf5
    PLUGIN_CODE Satu
    FORMATS ${SATURDAY_FORMATS}
    PRODUCT_NAME "Saturday")

target_sources(Saturday
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/SaturdayDSP.cpp)

target_compile_definitions(Saturday
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Saturday
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

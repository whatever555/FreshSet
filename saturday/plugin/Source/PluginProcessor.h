#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SaturdayDSP.h"
#include "SaturdayPresets.h"

class SaturdayAudioProcessor final : public juce::AudioProcessor
{
public:
    SaturdayAudioProcessor();
    ~SaturdayAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return kNumFactoryPresets; }
    int getCurrentProgram() override { return currentPresetIndex; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void applyPreset(int index);
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    juce::ChangeBroadcaster presetBroadcaster;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    SaturdayEngine& getEngine() { return engine; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    SaturdayEngine engine;
    int currentPresetIndex = 0;

    void setParameterValue(const juce::String& id, float value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturdayAudioProcessor)
};

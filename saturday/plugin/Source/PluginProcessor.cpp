#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace IDs
{
static constexpr auto mode    = "mode";
static constexpr auto drive   = "drive";
static constexpr auto tone    = "tone";
static constexpr auto mix     = "mix";
static constexpr auto quality = "quality";
static constexpr auto gate    = "gate";
} // namespace IDs

juce::AudioProcessorValueTreeState::ParameterLayout SaturdayAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        IDs::mode, "Mode", juce::StringArray { "Cassette", "Tube", "Iconic" }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        IDs::drive, "Drive", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 35.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        IDs::tone, "Tone", juce::NormalisableRange<float>(-100.f, 100.f, 1.f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        IDs::mix, "Mix", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 100.f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        IDs::quality, "Quality", juce::StringArray { "Standard", "Hi-Fi", "Ultra" }, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        IDs::gate, "Gate", juce::NormalisableRange<float>(-80.f, 0.f, 1.f), -28.f));
    return layout;
}

SaturdayAudioProcessor::SaturdayAudioProcessor()
    : AudioProcessor(BusesProperties()
                           .withInput("Input", juce::AudioChannelSet::stereo(), true)
                           .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

void SaturdayAudioProcessor::prepareToPlay(double sampleRate, int)
{
    engine.prepare(sampleRate);
}

void SaturdayAudioProcessor::releaseResources() {}

void SaturdayAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : left;

    const int mode = static_cast<int>(apvts.getRawParameterValue(IDs::mode)->load());
    engine.setMode(mode);

    engine.process(left, right, numSamples,
                   apvts.getRawParameterValue(IDs::drive)->load(),
                   apvts.getRawParameterValue(IDs::tone)->load(),
                   apvts.getRawParameterValue(IDs::mix)->load(),
                   static_cast<int>(apvts.getRawParameterValue(IDs::quality)->load()),
                   apvts.getRawParameterValue(IDs::gate)->load());
}

void SaturdayAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
        copyXmlToBinary(*state.createXml(), destData);
}

void SaturdayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes); xml != nullptr)
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* SaturdayAudioProcessor::createEditor()
{
    return new SaturdayAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SaturdayAudioProcessor();
}

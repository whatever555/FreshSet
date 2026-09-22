#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

namespace SaturdayColours
{
inline juce::Colour background()  { return juce::Colour(0xff1a1816); }
inline juce::Colour panel()       { return juce::Colour(0xff242220); }
inline juce::Colour panelHi()     { return juce::Colour(0xff2e2b28); }
inline juce::Colour copper()      { return juce::Colour(0xffd68d5e); }
inline juce::Colour copperDim()   { return juce::Colour(0xff8a5a3a); }
inline juce::Colour text()        { return juce::Colour(0xfff0ebe4); }
inline juce::Colour textDim()     { return juce::Colour(0xff9a9088); }
inline juce::Colour activeText()  { return juce::Colour(0xff1a1816); }
} // namespace SaturdayColours

/** Suppresses default JUCE slider/combo chrome so custom paint shows cleanly. */
class SaturdayInvisibleLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override {}
    void drawLinearSlider(juce::Graphics&, int, int, int, int, float, float, float,
                          juce::Slider::SliderStyle, juce::Slider&) override {}
    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override
    {
        juce::ignoreUnused(width, height, isButtonDown, buttonX, buttonY, buttonW, buttonH);
    }
};

class SaturdayComboLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawComboBox(juce::Graphics& g, int width, int height, bool,
                      int, int, int, int, juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float>(0.f, 0.f, (float) width, (float) height);
        g.setColour(SaturdayColours::panel());
        g.fillRoundedRectangle(bounds, 6.f);
        g.setColour(SaturdayColours::copperDim());
        g.drawRoundedRectangle(bounds, 6.f, 1.f);
        g.setColour(SaturdayColours::copper());
        g.setFont(juce::FontOptions().withHeight(14.f).withStyle("Bold"));
        g.drawText(box.getText(), bounds.reduced(12.f, 0.f).withTrimmedRight(20.f),
                   juce::Justification::centredLeft, true);
        juce::Path chev;
        const float cx = (float) width - 14.f;
        const float cy = height * 0.5f;
        chev.addTriangle(cx - 5.f, cy - 2.f, cx + 5.f, cy - 2.f, cx, cy + 4.f);
        g.fillPath(chev);
    }

    void positionComboBoxText(juce::ComboBox&, juce::Label& label) override
    {
        label.setBounds(0, 0, 0, 0);
        label.setVisible(false);
    }
};

class SaturdayHitArea final : public juce::Component
{
public:
    std::function<void()> onClick;
    void mouseUp(const juce::MouseEvent& e) override
    {
        if (e.mouseWasClicked() && onClick)
            onClick();
    }
};

class SaturdayKnob final : public juce::Component
{
public:
    SaturdayKnob(juce::RangedAudioParameter& param, juce::AudioProcessorValueTreeState& state,
                 juce::String label, bool isPercent, bool isTone);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::String labelText;
    bool percent = true;
    bool tone = false;
    juce::Slider slider;
    SaturdayInvisibleLookAndFeel knobLnF;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    void drawKnob(juce::Graphics& g, juce::Rectangle<float> bounds, float norm) const;
    juce::String valueText() const;
};

class SaturdayMeter final : public juce::Component, private juce::Timer
{
public:
    SaturdayMeter(juce::String labelText, std::function<float()> peakFn);
    void paint(juce::Graphics& g) override;

private:
    juce::String label;
    std::function<float()> getPeak;
    float displayPeak = 0.f;
    void timerCallback() override;
};

class SaturdayGateControl final : public juce::Component
{
public:
    SaturdayGateControl(juce::Slider& slider);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider& gateSlider;
    juce::String valueText() const;
    SaturdayInvisibleLookAndFeel lnf;
};

class SaturdayAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit SaturdayAudioProcessorEditor(SaturdayAudioProcessor&);
    ~SaturdayAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SaturdayAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;

    SaturdayHitArea modeHitAreas[3];
    SaturdayKnob driveKnob, mixKnob, toneKnob;
    SaturdayMeter inMeter, outMeter;
    juce::Slider gateSlider;
    SaturdayGateControl gateControl;
    juce::ComboBox qualityBox;
    SaturdayComboLookAndFeel comboLnF;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> qualityAttachment;

    juce::Font titleFont, labelFont, valueFont;
    void drawLogo(juce::Graphics& g, juce::Rectangle<int> area) const;
    void timerCallback() override;
    void paintModeButton(juce::Graphics& g, juce::Rectangle<int> bounds, juce::String text, bool active) const;
    void styleQualityBox();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturdayAudioProcessorEditor)
};

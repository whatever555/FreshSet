#include "PluginEditor.h"

namespace
{
constexpr int kWidth = 640;
constexpr int kHeight = 480;
constexpr int kLogoY = 14;
constexpr int kModeBtnY = 66;
constexpr int kMainPanelY = 110;
constexpr int kMainPanelH = 248;
constexpr int kFooterY = 378;
constexpr float kKnobStart = juce::MathConstants<float>::pi * 1.22f;
constexpr float kKnobEnd = juce::MathConstants<float>::pi * 2.78f;
constexpr float kKnobSpan = kKnobEnd - kKnobStart;
constexpr const char* kModeLabels[] = { "Cassette", "Tube", "Iconic" };

void styleInvisibleSlider(juce::Slider& slider, SaturdayInvisibleLookAndFeel& lnf)
{
    slider.setLookAndFeel(&lnf);
    slider.setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
}
} // namespace

SaturdayKnob::SaturdayKnob(juce::RangedAudioParameter& param, juce::AudioProcessorValueTreeState& state,
                           juce::String label, bool isPercent, bool isTone)
    : labelText(std::move(label)), percent(isPercent), tone(isTone)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setRotaryParameters(kKnobStart, kKnobEnd, true);
    styleInvisibleSlider(slider, knobLnF);
    addAndMakeVisible(slider);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, param.getParameterID(), slider);
}

void SaturdayKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(SaturdayColours::copper());
    g.setFont(juce::FontOptions().withHeight(11.f).withStyle("Bold"));
    g.drawText(labelText, bounds.removeFromTop(22.f), juce::Justification::centred);

    auto valueArea = bounds.removeFromBottom(34.f);
    auto knobBounds = bounds.reduced(2.f);
    const float knobSize = juce::jmin(knobBounds.getWidth(), knobBounds.getHeight());
    auto knobArea = knobBounds.withSizeKeepingCentre(knobSize, knobSize);

    const float norm = static_cast<float>(slider.valueToProportionOfLength(slider.getValue()));
    drawKnob(g, knobArea, norm);

    auto box = valueArea.withSizeKeepingCentre(76.f, 28.f);
    g.setColour(SaturdayColours::panel());
    g.fillRoundedRectangle(box, 6.f);
    g.setColour(SaturdayColours::copperDim());
    g.drawRoundedRectangle(box, 6.f, 1.f);
    g.setColour(SaturdayColours::copper());
    g.setFont(juce::FontOptions().withHeight(14.f).withStyle("Bold"));
    g.drawText(valueText(), box, juce::Justification::centred);
}

void SaturdayKnob::drawKnob(juce::Graphics& g, juce::Rectangle<float> area, float norm) const
{
    const auto cx = area.getCentreX();
    const auto cy = area.getCentreY();
    const auto r = area.getWidth() * 0.5f;
    const auto face = area.reduced(6.f);
    const auto arcR = r - 2.f;

    g.setColour(SaturdayColours::panelHi());
    g.fillEllipse(face);
    g.setColour(SaturdayColours::copperDim());
    g.drawEllipse(face, 1.f);

    juce::Path track;
    track.addCentredArc(cx, cy, arcR, arcR, 0.f, kKnobStart, kKnobEnd, true);
    g.setColour(SaturdayColours::copperDim().withAlpha(0.45f));
    g.strokePath(track, juce::PathStrokeType(2.f));

    if (norm > 0.001f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(cx, cy, arcR, arcR, 0.f, kKnobStart, kKnobStart + norm * kKnobSpan, true);
        g.setColour(SaturdayColours::copper());
        g.strokePath(valueArc, juce::PathStrokeType(2.5f));
    }

    const float angle = kKnobStart + norm * kKnobSpan;
    const auto tip = juce::Point<float>(cx, cy).getPointOnCircumference(r - 10.f, angle);
    g.setColour(SaturdayColours::text());
    g.drawLine(cx, cy, tip.x, tip.y, 2.f);
}

juce::String SaturdayKnob::valueText() const
{
    if (tone)
    {
        const int v = static_cast<int>(slider.getValue());
        return juce::String(v > 0 ? "+" : "") + juce::String(v);
    }
    if (percent) return juce::String(static_cast<int>(slider.getValue())) + "%";
    return juce::String(slider.getValue(), 1);
}

void SaturdayKnob::resized()
{
    slider.setBounds(getLocalBounds().withTrimmedTop(22).withTrimmedBottom(34));
}

SaturdayMeter::SaturdayMeter(juce::String labelText, std::function<float()> peakFn)
    : label(std::move(labelText)), getPeak(std::move(peakFn))
{
    startTimerHz(30);
}

void SaturdayMeter::timerCallback()
{
    displayPeak = displayPeak * 0.72f + getPeak() * 0.28f;
    repaint();
}

void SaturdayMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(SaturdayColours::copper());
    g.setFont(juce::FontOptions().withHeight(11.f).withStyle("Bold"));
    g.drawText(label, bounds.removeFromTop(14.f), juce::Justification::centred);

    bounds.removeFromTop(4.f);
    g.setColour(SaturdayColours::textDim());
    g.setFont(juce::FontOptions().withHeight(8.f));
    g.drawText("0", bounds.removeFromTop(10.f), juce::Justification::centred);

    g.setColour(SaturdayColours::textDim());
    g.drawText("-60", bounds.removeFromBottom(10.f), juce::Justification::centred);

    auto meter = bounds.reduced(1.f);
    g.setColour(SaturdayColours::panel());
    g.fillRoundedRectangle(meter, 4.f);
    g.setColour(SaturdayColours::copperDim());
    g.drawRoundedRectangle(meter, 4.f, 1.f);

    for (int i = 1; i < 6; ++i)
    {
        const float y = meter.getY() + meter.getHeight() * i / 6.f;
        g.setColour(SaturdayColours::copperDim().withAlpha(0.25f));
        g.drawHorizontalLine(juce::roundToInt(y), meter.getX() + 3.f, meter.getRight() - 3.f);
    }

    const float db = displayPeak > 1e-6f ? juce::jlimit(-60.f, 0.f, 20.f * std::log10(displayPeak)) : -60.f;
    const float norm = (db + 60.f) / 60.f;
    const float fillH = norm * (meter.getHeight() - 6.f);
    if (fillH > 1.f)
    {
        g.setColour(SaturdayColours::copper());
        g.fillRoundedRectangle(meter.getX() + 3.f, meter.getBottom() - 3.f - fillH,
                              meter.getWidth() - 6.f, fillH, 2.f);
    }
}

SaturdayGateControl::SaturdayGateControl(juce::Slider& slider) : gateSlider(slider)
{
    gateSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    styleInvisibleSlider(gateSlider, lnf);
    addAndMakeVisible(gateSlider);
    gateSlider.onValueChange = [this] { repaint(); };
}

void SaturdayGateControl::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(SaturdayColours::copper());
    g.setFont(juce::FontOptions().withHeight(11.f).withStyle("Bold"));
    g.drawText("GATE", bounds.removeFromTop(16.f), juce::Justification::centredLeft);

    auto valueBox = bounds.removeFromTop(28.f).withWidth(96.f);
    g.setColour(SaturdayColours::panel());
    g.fillRoundedRectangle(valueBox, 6.f);
    g.setColour(SaturdayColours::copperDim());
    g.drawRoundedRectangle(valueBox, 6.f, 1.f);
    g.setColour(SaturdayColours::copper());
    g.setFont(juce::FontOptions().withHeight(14.f).withStyle("Bold"));
    g.drawText(valueText(), valueBox, juce::Justification::centred);

    bounds.removeFromTop(10.f);
    auto track = bounds.removeFromTop(6.f);
    g.setColour(SaturdayColours::panelHi().withAlpha(0.5f));
    g.fillRoundedRectangle(track, 3.f);

    const float norm = static_cast<float>((gateSlider.getValue() - gateSlider.getMinimum())
                                          / (gateSlider.getMaximum() - gateSlider.getMinimum()));
    if (norm > 0.001f)
    {
        g.setColour(SaturdayColours::copper());
        g.fillRoundedRectangle(track.getX(), track.getY(), track.getWidth() * norm, track.getHeight(), 3.f);
    }

    const float thumbX = track.getX() + track.getWidth() * norm;
    g.setColour(SaturdayColours::copper());
    g.fillEllipse(thumbX - 6.f, track.getCentreY() - 6.f, 12.f, 12.f);
    g.setColour(SaturdayColours::text());
    g.fillEllipse(thumbX - 3.f, track.getCentreY() - 3.f, 6.f, 6.f);
}

juce::String SaturdayGateControl::valueText() const
{
    const float v = static_cast<float>(gateSlider.getValue());
    return v <= -79.f ? "Off" : juce::String(static_cast<int>(v)) + " dB";
}

void SaturdayGateControl::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(16 + 28 + 10);
    gateSlider.setBounds(area.removeFromTop(6).expanded(0, 12));
}

SaturdayAudioProcessorEditor::~SaturdayAudioProcessorEditor()
{
    qualityBox.setLookAndFeel(nullptr);
}

SaturdayAudioProcessorEditor::SaturdayAudioProcessorEditor(SaturdayAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      apvts(p.apvts),
      driveKnob(*apvts.getParameter("drive"), apvts, "DRIVE", true, false),
      mixKnob(*apvts.getParameter("mix"), apvts, "MIX", true, false),
      toneKnob(*apvts.getParameter("tone"), apvts, "TONE", false, true),
      inMeter("IN", [this] { return juce::jmax(processor.getEngine().getMeterInL(),
                                             processor.getEngine().getMeterInR()); }),
      outMeter("OUT", [this] { return juce::jmax(processor.getEngine().getMeterOutL(),
                                              processor.getEngine().getMeterOutR()); }),
      gateControl(gateSlider)
{
    titleFont = juce::Font(juce::FontOptions().withName("Georgia").withHeight(30.f));
    labelFont = juce::Font(juce::FontOptions().withName("Helvetica Neue").withHeight(11.f).withStyle("Bold"));
    valueFont = juce::Font(juce::FontOptions().withName("Helvetica Neue").withHeight(14.f).withStyle("Bold"));

    for (int i = 0; i < 3; ++i)
    {
        modeHitAreas[i].onClick = [this, i]
        {
            if (auto* param = apvts.getParameter("mode"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(i)));
        };
        addAndMakeVisible(modeHitAreas[i]);
    }

    addAndMakeVisible(driveKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(toneKnob);
    addAndMakeVisible(inMeter);
    addAndMakeVisible(outMeter);
    addAndMakeVisible(gateControl);

    gateSlider.setRange(-80.0, 0.0, 1.0);
    gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "gate", gateSlider);

    qualityBox.addItemList({ "Standard", "Hi-Fi", "Ultra" }, 1);
    addAndMakeVisible(qualityBox);
    qualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "quality", qualityBox);
    styleQualityBox();

    startTimerHz(20);
    setSize(kWidth, kHeight);
    setResizable(false, false);
}

void SaturdayAudioProcessorEditor::styleQualityBox()
{
    qualityBox.setLookAndFeel(&comboLnF);
    qualityBox.setColour(juce::ComboBox::backgroundColourId, SaturdayColours::panel());
    qualityBox.setColour(juce::ComboBox::outlineColourId, SaturdayColours::copperDim());
    qualityBox.setColour(juce::ComboBox::textColourId, SaturdayColours::copper());
    qualityBox.setColour(juce::ComboBox::arrowColourId, SaturdayColours::copper());
}

void SaturdayAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(SaturdayColours::background());

    g.setColour(SaturdayColours::copperDim());
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.f), 12.f, 1.5f);

    drawLogo(g, { 24, kLogoY, 280, 54 });

    g.setColour(SaturdayColours::panel().withAlpha(0.55f));
    g.fillRoundedRectangle(20.f, (float) kMainPanelY, 600.f, (float) kMainPanelH, 10.f);
    g.setColour(SaturdayColours::copperDim().withAlpha(0.55f));
    g.drawRoundedRectangle(20.f, (float) kMainPanelY, 600.f, (float) kMainPanelH, 10.f, 1.f);

    g.setColour(SaturdayColours::panel().withAlpha(0.55f));
    g.fillRoundedRectangle(20.f, (float) kFooterY, 600.f, 88.f, 10.f);
    g.setColour(SaturdayColours::copperDim().withAlpha(0.55f));
    g.drawRoundedRectangle(20.f, (float) kFooterY, 600.f, 88.f, 10.f, 1.f);

    g.setColour(SaturdayColours::copper());
    g.setFont(labelFont);
    g.drawText("QUALITY", 332, kFooterY + 8, 80, 16, juce::Justification::centredLeft);

    const int btnW = (getWidth() - 40 - 12) / 3;
    const int mode = static_cast<int>(apvts.getRawParameterValue("mode")->load());
    for (int i = 0; i < 3; ++i)
        paintModeButton(g, { 20 + i * (btnW + 6), kModeBtnY, btnW, 38 }, kModeLabels[i], i == mode);
}

void SaturdayAudioProcessorEditor::drawLogo(juce::Graphics& g, juce::Rectangle<int> area) const
{
    const auto textCol = area.withTrimmedLeft(28);
    const int textX = textCol.getX();
    const int textY = textCol.getY();

    g.setFont(titleFont);
    const int titleH = juce::roundToInt(titleFont.getHeight());
    const float waveMidY = (float) textY + titleFont.getAscent() * 0.52f;

    juce::Path wave;
    wave.startNewSubPath((float) area.getX(), waveMidY);
    wave.lineTo((float) area.getX() + 6.f, waveMidY);
    wave.lineTo((float) area.getX() + 10.f, waveMidY - 8.f);
    wave.lineTo((float) area.getX() + 14.f, waveMidY + 4.f);
    wave.lineTo((float) area.getX() + 18.f, waveMidY - 4.f);
    wave.lineTo((float) area.getX() + 22.f, waveMidY + 2.f);
    g.setColour(SaturdayColours::copper());
    g.strokePath(wave, juce::PathStrokeType(2.f));

    g.setColour(SaturdayColours::copper());
    g.drawText("Saturday", textX, textY, textCol.getWidth(), titleH, juce::Justification::topLeft);

    const auto subtitleFont = juce::Font(juce::FontOptions().withName("Helvetica Neue").withHeight(10.f));
    g.setFont(subtitleFont);
    g.setColour(SaturdayColours::textDim());
    const int subtitleY = textY + titleH + 2;
    const int subtitleH = juce::roundToInt(subtitleFont.getHeight());
    g.drawText("SATURATION SHAPER", textX, subtitleY, textCol.getWidth(), subtitleH, juce::Justification::topLeft);
}

void SaturdayAudioProcessorEditor::resized()
{
    const int btnW = (getWidth() - 40 - 12) / 3;
    for (int i = 0; i < 3; ++i)
        modeHitAreas[i].setBounds(20 + i * (btnW + 6), kModeBtnY, btnW, 38);

    const int meterY = kMainPanelY + 14;
    const int meterH = kMainPanelH - 28;
    inMeter.setBounds(52, meterY, 28, meterH);
    outMeter.setBounds(560, meterY, 28, meterH);
    driveKnob.setBounds(152, kMainPanelY + 24, 120, kMainPanelH - 48);
    mixKnob.setBounds(284, kMainPanelY + 24, 120, kMainPanelH - 48);
    toneKnob.setBounds(416, kMainPanelY + 24, 120, kMainPanelH - 48);

    gateControl.setBounds(32, kFooterY + 6, 276, 74);
    qualityBox.setBounds(332, kFooterY + 28, 276, 28);
}

void SaturdayAudioProcessorEditor::timerCallback()
{
    driveKnob.repaint();
    mixKnob.repaint();
    toneKnob.repaint();
    gateControl.repaint();
    repaint();
}

void SaturdayAudioProcessorEditor::paintModeButton(juce::Graphics& g, juce::Rectangle<int> bounds,
                                                   juce::String text, bool active) const
{
    auto r = bounds.toFloat();
    if (active)
    {
        g.setColour(SaturdayColours::copper());
        g.fillRoundedRectangle(r, 10.f);
        g.setColour(SaturdayColours::activeText());
    }
    else
    {
        g.setColour(SaturdayColours::panel().withAlpha(0.8f));
        g.fillRoundedRectangle(r, 10.f);
        g.setColour(SaturdayColours::copperDim());
        g.drawRoundedRectangle(r, 10.f, 1.f);
        g.setColour(SaturdayColours::copper());
    }
    g.setFont(juce::FontOptions().withHeight(13.f).withStyle("Bold"));
    g.drawText(text, bounds, juce::Justification::centred);
}

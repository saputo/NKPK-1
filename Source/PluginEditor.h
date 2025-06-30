#pragma once

#include <JuceHeader.h>

// Forward declare the processor to break the include cycle.
class NKPK1AudioProcessor;

//==============================================================================
class NKPK1AudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Timer
{
public:
    explicit NKPK1AudioProcessorEditor(NKPK1AudioProcessor&);
    ~NKPK1AudioProcessorEditor() noexcept override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    NKPK1AudioProcessor& audioProcessor;

    // --- UI Elements ---
    juce::Slider oscWaveTypeSlider, filterCutoffSlider, filterResonanceSlider, filterEnvAmountSlider, ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider;
    juce::TextButton monoPolyPad, subOscPad, lfoPitchPad, lfoFilterPad, glidePad, warpPad, noisePad, chorusPad;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscWaveTypeAttachment, filterCutoffAttachment, filterResonanceAttachment, filterEnvAmountAttachment, ampAttackAttachment, ampDecayAttachment, ampSustainAttachment, ampReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoPolyPadAttachment, subOscPadAttachment, lfoPitchPadAttachment, lfoFilterPadAttachment, glidePadAttachment, warpPadAttachment, noisePadAttachment, chorusPadAttachment;
    juce::Component kaossPad, kaossPuck;

    // --- LookAndFeel ---
    // The entire class, including method bodies, is now defined here.
    class MPKMiniLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        MPKMiniLookAndFeel()
        {
            setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xFF222222));
            setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFFCC0000));
            setColour(juce::Slider::thumbColourId, juce::Colours::white);
            setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
            setColour(juce::Slider::textBoxTextColourId, juce::Colours::lightgrey);
            setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF333333));
            setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFFCC0000));
            setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
            setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
        {
            juce::ignoreUnused(slider);
            auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
            auto centreX = (float)x + (float)width * 0.5f;
            auto centreY = (float)y + (float)height * 0.5f;
            auto rx = centreX - radius;
            auto ry = centreY - radius;
            auto rw = radius * 2.0f;
            auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

            g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
            g.fillEllipse(rx, ry, rw, rw);

            juce::Path p;
            auto pointerLength = radius * 0.8f;
            auto pointerThickness = 2.5f;
            p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
            p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

            g.setColour(findColour(juce::Slider::thumbColourId));
            g.fillPath(p);
        }
    };

    MPKMiniLookAndFeel mpkMiniLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NKPK1AudioProcessorEditor)
};
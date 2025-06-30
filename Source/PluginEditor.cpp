#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NKPK1AudioProcessorEditor::NKPK1AudioProcessorEditor(NKPK1AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&mpkMiniLookAndFeel);

    // Kaoss Pad Setup
    addAndMakeVisible(kaossPad);
    addAndMakeVisible(kaossPuck);
    kaossPuck.setColour(juce::Slider::thumbColourId, juce::Colours::white.withAlpha(0.7f));

    // Knob Setup
    auto setupKnob = [&](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        addAndMakeVisible(slider);
        };

    setupKnob(oscWaveTypeSlider);
    oscWaveTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WAVETYPE", oscWaveTypeSlider);
    setupKnob(filterCutoffSlider);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FILTER_CUTOFF", filterCutoffSlider);
    setupKnob(filterResonanceSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FILTER_RES", filterResonanceSlider);
    setupKnob(filterEnvAmountSlider);
    filterEnvAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FILTER_ENV_AMT", filterEnvAmountSlider);
    setupKnob(ampAttackSlider);
    ampAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "AMP_ATTACK", ampAttackSlider);
    setupKnob(ampDecaySlider);
    ampDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "AMP_DECAY", ampDecaySlider);
    setupKnob(ampSustainSlider);
    ampSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "AMP_SUSTAIN", ampSustainSlider);
    setupKnob(ampReleaseSlider);
    ampReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "AMP_RELEASE", ampReleaseSlider);

    // Pad Setup
    auto setupPad = [&](juce::TextButton& button, const juce::String& text) {
        button.setButtonText(text);
        button.setClickingTogglesState(true);
        addAndMakeVisible(button);
        };

    setupPad(monoPolyPad, "Mono/Poly");
    monoPolyPadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "MONO_POLY", monoPolyPad);
    setupPad(subOscPad, "Sub Osc");
    subOscPadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SUB_OSC_ON", subOscPad);
    setupPad(lfoPitchPad, "LFO>Pitch");
    lfoPitchPadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LFO_PITCH_ON", lfoPitchPad);
    setupPad(lfoFilterPad, "LFO>Filter");
    lfoFilterPadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LFO_FILTER_ON", lfoFilterPad);
    setupPad(glidePad, "Glide");
    glidePadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "GLIDE_ON", glidePad);
    setupPad(warpPad, "Warp");
    warpPadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "WARP_ON", warpPad);
    setupPad(noisePad, "Noise");
    noisePadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "NOISE_ON", noisePad);
    setupPad(chorusPad, "Chorus");
    chorusPadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "CHORUS_ON", chorusPad);

    setSize(950, 400);
    startTimerHz(30);
}

NKPK1AudioProcessorEditor::~NKPK1AudioProcessorEditor() noexcept
{
    setLookAndFeel(nullptr);
    stopTimer();
}

//==============================================================================
void NKPK1AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(getLookAndFeel().findColour(juce::GroupComponent::outlineColourId));
    g.drawRect(kaossPad.getBounds(), 1.5f);
}

void NKPK1AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Pads are on the left, Knobs on the right.
    auto padsColumn = bounds.removeFromLeft(350);
    auto knobsColumn = bounds.removeFromRight(350);
    auto kaossColumn = bounds;

    // --- 1. Layout Pads (Left Column) ---
    {
        int padSize = 75;
        int padSpacingX = (padsColumn.getWidth() - (padSize * 4)) / 5;
        int padSpacingY = (padsColumn.getHeight() - (padSize * 2)) / 3;
        int pads_currentX = padsColumn.getX() + padSpacingX;
        int pads_topRowY = padsColumn.getY() + padSpacingY;
        int pads_bottomRowY = pads_topRowY + padSize + padSpacingY;

        // The Glide/Warp/Noise/Chorus pads on the TOP row.
        glidePad.setBounds(pads_currentX, pads_topRowY, padSize, padSize);
        pads_currentX += padSize + padSpacingX;
        warpPad.setBounds(pads_currentX, pads_topRowY, padSize, padSize);
        pads_currentX += padSize + padSpacingX;
        noisePad.setBounds(pads_currentX, pads_topRowY, padSize, padSize);
        pads_currentX += padSize + padSpacingX;
        chorusPad.setBounds(pads_currentX, pads_topRowY, padSize, padSize);

        // The Mono/Sub/LFO pads on the BOTTOM row.
        pads_currentX = padsColumn.getX() + padSpacingX; // Reset X
        monoPolyPad.setBounds(pads_currentX, pads_bottomRowY, padSize, padSize);
        pads_currentX += padSize + padSpacingX;
        subOscPad.setBounds(pads_currentX, pads_bottomRowY, padSize, padSize);
        pads_currentX += padSize + padSpacingX;
        lfoPitchPad.setBounds(pads_currentX, pads_bottomRowY, padSize, padSize);
        pads_currentX += padSize + padSpacingX;
        lfoFilterPad.setBounds(pads_currentX, pads_bottomRowY, padSize, padSize);
    }

    // --- 2. Layout Knobs (Right Column) ---
    {
        int knobWidth = 75;
        int knobHeight = 95;
        int knobSpacingX = (knobsColumn.getWidth() - (knobWidth * 4)) / 5;
        int knobSpacingY = (knobsColumn.getHeight() - (knobHeight * 2)) / 3;
        int knobs_currentX = knobsColumn.getX() + knobSpacingX;
        int knobs_topRowY = knobsColumn.getY() + knobSpacingY;

        oscWaveTypeSlider.setBounds(knobs_currentX, knobs_topRowY, knobWidth, knobHeight);
        knobs_currentX += knobWidth + knobSpacingX;
        filterCutoffSlider.setBounds(knobs_currentX, knobs_topRowY, knobWidth, knobHeight);
        knobs_currentX += knobWidth + knobSpacingX;
        filterResonanceSlider.setBounds(knobs_currentX, knobs_topRowY, knobWidth, knobHeight);
        knobs_currentX += knobWidth + knobSpacingX;
        filterEnvAmountSlider.setBounds(knobs_currentX, knobs_topRowY, knobWidth, knobHeight);

        knobs_currentX = knobsColumn.getX() + knobSpacingX;
        int knobs_bottomRowY = knobs_topRowY + knobHeight + knobSpacingY;
        ampAttackSlider.setBounds(knobs_currentX, knobs_bottomRowY, knobWidth, knobHeight);
        knobs_currentX += knobWidth + knobSpacingX;
        ampDecaySlider.setBounds(knobs_currentX, knobs_bottomRowY, knobWidth, knobHeight);
        knobs_currentX += knobWidth + knobSpacingX;
        ampSustainSlider.setBounds(knobs_currentX, knobs_bottomRowY, knobWidth, knobHeight);
        knobs_currentX += knobWidth + knobSpacingX;
        ampReleaseSlider.setBounds(knobs_currentX, knobs_bottomRowY, knobWidth, knobHeight);
    }

    // --- 3. Layout Kaoss Pad (Middle Column) ---
    {
        int squareSize = juce::jmin(kaossColumn.getWidth(), kaossColumn.getHeight()) - 20;
        kaossPad.setBounds(kaossColumn.withSizeKeepingCentre(squareSize, squareSize));
    }
}

void NKPK1AudioProcessorEditor::timerCallback()
{
    float joyX = audioProcessor.getJoystickX();
    float joyY = audioProcessor.getJoystickY();

    auto padBounds = kaossPad.getBounds();
    float puckX = padBounds.getX() + (joyX + 1.0f) * 0.5f * padBounds.getWidth();
    float puckY = padBounds.getY() + (1.0f - joyY) * padBounds.getHeight();

    int puckSize = 16;
    kaossPuck.setBounds(juce::Rectangle<int>((int)puckX, (int)puckY, 0, 0).withSizeKeepingCentre(puckSize, puckSize));
}
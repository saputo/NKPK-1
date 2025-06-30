#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"

//==============================================================================
class NKPK1AudioProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    NKPK1AudioProcessor();
    ~NKPK1AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // ... (other functions remain the same) ...

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // VST Coder Add: Public getter methods for Kaoss Pad visualization
    float getJoystickX() const { return smoothedPitchBend.getCurrentValue(); }
    float getJoystickY() const { return smoothedModWheel.getCurrentValue(); }

    juce::AudioProcessorValueTreeState apvts;

private:
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::Synthesiser synth;
    const int numVoices = 8;

    juce::SmoothedValue<float> smoothedPitchBend;
    juce::SmoothedValue<float> smoothedModWheel;

    // Internal state variables
    float oscWaveType;
    float filterCutoff;
    float filterResonance;
    float filterEnvAmount;
    float ampAttack;
    float ampDecay;
    float ampSustain;
    float ampRelease;
    bool isMonophonic;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NKPK1AudioProcessor)
};
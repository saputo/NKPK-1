#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h" // We now include our separate SynthVoice header

//==============================================================================
class NKPK1AudioProcessor : public juce::AudioProcessor
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

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // The main synthesizer object
    juce::Synthesiser synth;
    const int numVoices = 8;

    // "Kaoss Stick" members for smoothing joystick input
    juce::SmoothedValue<float> smoothedPitchBend; // For X-axis
    juce::SmoothedValue<float> smoothedModWheel;  // For Y-axis

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NKPK1AudioProcessor)
};
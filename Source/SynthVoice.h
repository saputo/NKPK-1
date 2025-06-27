#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"

//==============================================================================
// SynthSound: Informs the synthesiser that it can play any note on any channel.
//==============================================================================
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};


//==============================================================================
// SynthVoice: Represents a single polyphonic voice of the synthesizer.
// Handles sound generation, filtering, and envelopes for a single note.
//==============================================================================
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice(); // Constructor

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Parameter setters called from the PluginProcessor
    void setADSRParameters(float attack, float decay, float sustain, float release);
    void setFilterParameters(float cutoff, float resonance, float envAmount);
    void setWaveType(float newWaveType);
    void setJoystickPositions(float x, float y); // For our "Kaoss Stick"

    // Preparation method called from PluginProcessor::prepareToPlay
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);

    int voiceIndex = 0; // For debugging

private:
    // Main DSP components
    Oscillator oscillator;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::ADSR ampADSR;
    juce::ADSR filterADSR;

    // Parameter storage
    float currentWaveType = 0.0f;
    float currentFilterCutoff = 6000.0f;
    float currentFilterResonance = 0.0f;
    float currentFilterEnvAmount = 0.0f;
    float velocity = 0.0f;

    // Joystick position storage
    float joystickX = 0.0f; // Mapped to Pitch Bend -> Filter Cutoff modulation
    float joystickY = 0.0f; // Mapped to Mod Wheel -> Filter Resonance modulation

    // Flag to ensure smooth parameter updates
    bool parametersChanged = true;
};
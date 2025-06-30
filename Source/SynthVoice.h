#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { juce::ignoreUnused(midiNoteNumber); return true; }
    bool appliesToChannel(int midiChannel) override { juce::ignoreUnused(midiChannel); return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setADSRParameters(float attack, float decay, float sustain, float release);
    void setFilterParameters(float cutoff, float resonance, float envAmount);
    void setWaveType(float newWaveType);
    void setJoystickPositions(float x, float y);
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    int voiceIndex = 0;
private:
    Oscillator oscillator;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::ADSR ampADSR;
    juce::ADSR filterADSR;
    float currentFilterCutoff = 6000.0f;
    float currentFilterResonance = 0.0f;
    float currentFilterEnvAmount = 0.0f;
    float velocity = 0.0f;
    float joystickX = 0.0f;
    float joystickY = 0.0f;
    bool parametersChanged = true;
};
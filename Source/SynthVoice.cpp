#include "SynthVoice.h"

SynthVoice::SynthVoice() {}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    juce::ignoreUnused(sound, currentPitchWheelPosition);
    double frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(frequency, getSampleRate());
    this->velocity = velocity;
    ampADSR.noteOn();
    filterADSR.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    juce::ignoreUnused(velocity);
    ampADSR.noteOff();
    filterADSR.noteOff();
    if (!allowTailOff) { clearCurrentNote(); }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue) { juce::ignoreUnused(newPitchWheelValue); }
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) { juce::ignoreUnused(controllerNumber, newControllerValue); }

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;
    oscillator.setFrequency(440.0, sampleRate);
    filter.prepare(spec);
    ampADSR.setSampleRate(sampleRate);
    filterADSR.setSampleRate(sampleRate);
    juce::ADSR::Parameters filterADSRParams;
    filterADSRParams.attack = 0.1f;
    filterADSRParams.decay = 0.2f;
    filterADSRParams.sustain = 0.0f;
    filterADSRParams.release = 0.3f;
    filterADSR.setParameters(filterADSRParams);
    filter.reset();
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive()) return;
    juce::AudioBuffer<float> synthBuffer;
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float oscSample = oscillator.getNextSample();
        float filterEnvValue = filterADSR.getNextSample();
        float joystickCutoffMod = joystickX * 4000.0f;
        float modulatedCutoff = currentFilterCutoff + joystickCutoffMod + (filterEnvValue * currentFilterEnvAmount * 5000.0f);
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
        float modulatedResonance = currentFilterResonance + (joystickY * 0.8f);
        modulatedResonance = juce::jlimit(0.1f, 1.0f, modulatedResonance);
        filter.setCutoffFrequency(modulatedCutoff);
        filter.setResonance(modulatedResonance);
        float filteredSample = filter.processSample(0, oscSample);
        float ampEnvValue = ampADSR.getNextSample();
        float finalSample = filteredSample * velocity * ampEnvValue;
        for (int channel = 0; channel < synthBuffer.getNumChannels(); ++channel)
        {
            synthBuffer.setSample(channel, sample, finalSample);
        }
    }
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
    }
    if (!ampADSR.isActive() && !isKeyDown())
    {
        clearCurrentNote();
    }
}

void SynthVoice::setADSRParameters(float attack, float decay, float sustain, float release) { ampADSR.setParameters({ attack, decay, sustain, release }); }
void SynthVoice::setFilterParameters(float cutoff, float resonance, float envAmount) { currentFilterCutoff = cutoff; currentFilterResonance = resonance; currentFilterEnvAmount = envAmount; }
void SynthVoice::setWaveType(float newWaveType) { oscillator.setWaveType(newWaveType); }
void SynthVoice::setJoystickPositions(float x, float y) { joystickX = x; joystickY = y; }
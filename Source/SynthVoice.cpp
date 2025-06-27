#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    // Constructor
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    // Set the oscillator frequency based on the MIDI note
    double frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(frequency, getSampleRate());

    this->velocity = velocity; // Store the note velocity (0.0 to 1.0)

    // Trigger the attack phase of both envelopes
    ampADSR.noteOn();
    filterADSR.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    // Trigger the release phase of both envelopes
    ampADSR.noteOff();
    filterADSR.noteOff();

    // If tail-off is not allowed, stop the note immediately
    if (!allowTailOff)
    {
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    // Implementation will be handled by the "Kaoss Stick" logic in PluginProcessor
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // Implementation will be handled by the "Kaoss Stick" logic in PluginProcessor
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    // Prepare the DSP components with the host's ProcessSpec
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;

    oscillator.setFrequency(440.0, sampleRate); // Set a default frequency
    filter.prepare(spec);
    ampADSR.setSampleRate(sampleRate);
    filterADSR.setSampleRate(sampleRate);

    // Hard-code some default parameters for the Filter ADSR for now
    juce::ADSR::Parameters filterADSRParams;
    filterADSRParams.attack = 0.1f;
    filterADSRParams.decay = 0.2f;
    filterADSRParams.sustain = 0.0f; // Filter env often has 0 sustain
    filterADSRParams.release = 0.3f;
    filterADSR.setParameters(filterADSRParams);

    filter.reset();
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    // If the voice is not active, there's nothing to do
    if (!isVoiceActive())
        return;

    // To avoid writing directly to the output buffer if it's not cleared,
    // we use a temporary buffer for our voice's output.
    juce::AudioBuffer<float> synthBuffer;
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();

    // The main per-sample processing loop
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // 1. Get raw sample from the oscillator
        float oscSample = oscillator.getNextSample();

        // 2. Process through the Filter
        // Get the next value from the filter envelope
        float filterEnvValue = filterADSR.getNextSample();

        // Modulate the filter cutoff with the joystick X-axis and the filter envelope
        // Joystick X is mapped from pitch bend's -1 to 1 range, scaling cutoff up/down
        float joystickCutoffMod = joystickX * 4000.0f; // Modulate up to 4kHz
        float modulatedCutoff = currentFilterCutoff + joystickCutoffMod + (filterEnvValue * currentFilterEnvAmount * 5000.0f);
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);

        // Modulate resonance with joystick Y-axis
        float modulatedResonance = currentFilterResonance + (joystickY * 0.8f); // Add up to 0.8 resonance
        modulatedResonance = juce::jlimit(0.0f, 1.0f, modulatedResonance);

        // Set the filter's state for this sample
        filter.setCutoffFrequency(modulatedCutoff);
        filter.setResonance(modulatedResonance);

        // Process the oscillator sample through the filter
        float filteredSample = filter.processSample(0, oscSample); // Processing mono sample on channel 0

        // 3. Process through the Amplifier
        // Get the next value from the amp envelope
        float ampEnvValue = ampADSR.getNextSample();

        // Apply velocity and the amp envelope
        float finalSample = filteredSample * velocity * ampEnvValue;

        // Write the final sample to all channels of our temporary buffer
        for (int channel = 0; channel < synthBuffer.getNumChannels(); ++channel)
        {
            synthBuffer.setSample(channel, sample, finalSample);
        }
    }

    // Add our voice's rendered audio to the main output buffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
    }

    // If the note has finished its release phase, make the voice available again
    if (!ampADSR.isActive() && !isKeyDown())
    {
        clearCurrentNote();
    }
}

void SynthVoice::setADSRParameters(float attack, float decay, float sustain, float release)
{
    ampADSR.setParameters({ attack, decay, sustain, release });
}

void SynthVoice::setFilterParameters(float cutoff, float resonance, float envAmount)
{
    currentFilterCutoff = cutoff;
    currentFilterResonance = resonance;
    currentFilterEnvAmount = envAmount;
}

void SynthVoice::setWaveType(float newWaveType)
{
    oscillator.setWaveType(newWaveType);
}

void SynthVoice::setJoystickPositions(float x, float y)
{
    // The pitch bend value from the processor is already -1.0 to 1.0
    joystickX = x;
    // The mod wheel value from the processor is 0.0 to 1.0
    joystickY = y;
}
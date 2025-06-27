#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This class will handle all of our sound wave generation.
*/
class Oscillator
{
public:
    Oscillator(); // This is the constructor

    // Sets the frequency of the oscillator (i.e., the pitch of the note)
    void setFrequency(double frequency, double sampleRate);

    // Sets the waveform type (0.0 for sine, 1.0 for saw)
    void setWaveType(float waveType);

    // Gets the next audio sample from the oscillator
    float getNextSample();

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    float currentWaveType = 0.0f;
};
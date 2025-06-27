// Oscillator.cpp

#include "Oscillator.h"
#include <cmath> // For M_PI and trigonometric functions

//==============================================================================
Oscillator::Oscillator()
{
    // Constructor - we don't need to do anything here for now
}

void Oscillator::setFrequency(double frequency, double sampleRate)
{
    // This calculates how much our angle should change for each audio sample
    // to produce the correct pitch.
    auto cyclesPerSample = frequency / sampleRate;
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

void Oscillator::setWaveType(float waveType)
{
    // This just stores the current wave type (0.0 to 1.0)
    // 0.0 will result in a pure Sine wave, 1.0 will result in a pure Saw wave,
    // and values in between will linearly crossfade the two.
    currentWaveType = waveType;
}

float Oscillator::getNextSample()
{
    // Calculate the two base waveforms: Sine and Saw
    float sineSample = static_cast<float>(std::sin(currentAngle));

    // This creates a symmetric saw wave that goes from -1.0 to 1.0
    // It normalizes the current angle to 0-1, then maps it to a sawtooth shape.
    float sawSample = static_cast<float>(2.0 * (currentAngle / juce::MathConstants<double>::twoPi - std::floor(currentAngle / juce::MathConstants<double>::twoPi + 0.5)));

    // Mix the two waves together based on our currentWaveType knob value (0.0 to 1.0)
    // This is a linear interpolation (LERP) formula: (1-t)*A + t*B
    float mixedSample = (1.0f - currentWaveType) * sineSample + currentWaveType * sawSample;

    // Advance the angle for the next sample
    currentAngle += angleDelta;

    // Wrap the angle to stay within 0 to 2*PI.
    // This prevents floating point precision issues over long periods of time.
    while (currentAngle >= juce::MathConstants<double>::twoPi)
    {
        currentAngle -= juce::MathConstants<double>::twoPi;
    }

    return mixedSample;
}

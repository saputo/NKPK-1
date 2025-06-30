#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"

//==============================================================================
NKPK1AudioProcessor::NKPK1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    apvts.addParameterListener("WAVETYPE", this);
    apvts.addParameterListener("FILTER_CUTOFF", this);
    apvts.addParameterListener("FILTER_RES", this);
    apvts.addParameterListener("FILTER_ENV_AMT", this);
    apvts.addParameterListener("AMP_ATTACK", this);
    apvts.addParameterListener("AMP_DECAY", this);
    apvts.addParameterListener("AMP_SUSTAIN", this);
    apvts.addParameterListener("AMP_RELEASE", this);
    apvts.addParameterListener("MONO_POLY", this);
    apvts.addParameterListener("SUB_OSC_ON", this);
    apvts.addParameterListener("LFO_PITCH_ON", this);
    apvts.addParameterListener("LFO_FILTER_ON", this);
    apvts.addParameterListener("GLIDE_ON", this);
    apvts.addParameterListener("WARP_ON", this);
    apvts.addParameterListener("NOISE_ON", this);
    apvts.addParameterListener("CHORUS_ON", this);

    synth.addSound(new SynthSound());

    for (int i = 0; i < numVoices; ++i)
    {
        synth.addVoice(new SynthVoice());
    }
}

NKPK1AudioProcessor::~NKPK1AudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout NKPK1AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Default values are set to their minimums so all controls start "off" or "left".
    params.push_back(std::make_unique<juce::AudioParameterFloat>("WAVETYPE", "Wave Type", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF", "Filter Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_RES", "Filter Resonance", juce::NormalisableRange<float>(0.1f, 1.0f, 0.01f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_ENV_AMT", "Filter Env Amt", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_ATTACK", "Amp Attack", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.5f), 0.001f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_DECAY", "Amp Decay", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.5f), 0.001f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_SUSTAIN", "Amp Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_RELEASE", "Amp Release", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.5f), 0.001f));

    params.push_back(std::make_unique<juce::AudioParameterBool>("MONO_POLY", "Mono/Poly", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("SUB_OSC_ON", "Sub-Osc On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("LFO_PITCH_ON", "LFO Pitch On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("LFO_FILTER_ON", "LFO Filter On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("GLIDE_ON", "Glide On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("WARP_ON", "Warp On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("NOISE_ON", "Noise On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("CHORUS_ON", "Chorus On", false));

    return { params.begin(), params.end() };
}

void NKPK1AudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "WAVETYPE")        oscWaveType = newValue;
    else if (parameterID == "FILTER_CUTOFF")  filterCutoff = newValue;
    else if (parameterID == "FILTER_RES")     filterResonance = newValue;
    else if (parameterID == "FILTER_ENV_AMT") filterEnvAmount = newValue;
    else if (parameterID == "AMP_ATTACK")     ampAttack = newValue;
    else if (parameterID == "AMP_DECAY")      ampDecay = newValue;
    else if (parameterID == "AMP_SUSTAIN")    ampSustain = newValue;
    else if (parameterID == "AMP_RELEASE")    ampRelease = newValue;
    else if (parameterID == "MONO_POLY")      isMonophonic = static_cast<bool>(newValue);
}

const juce::String NKPK1AudioProcessor::getName() const { return JucePlugin_Name; }
bool NKPK1AudioProcessor::acceptsMidi() const { return true; }
bool NKPK1AudioProcessor::producesMidi() const { return false; }
bool NKPK1AudioProcessor::isMidiEffect() const { return false; }
double NKPK1AudioProcessor::getTailLengthSeconds() const { return 0.0; }
int NKPK1AudioProcessor::getNumPrograms() { return 1; }
int NKPK1AudioProcessor::getCurrentProgram() { return 0; }
void NKPK1AudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
const juce::String NKPK1AudioProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }
void NKPK1AudioProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void NKPK1AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    smoothedPitchBend.reset(sampleRate, 0.02);
    smoothedModWheel.reset(sampleRate, 0.02);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

void NKPK1AudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NKPK1AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::disabled())
        return false;

    return true;
}
#endif

void NKPK1AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // 1. Process incoming MIDI messages to update parameters and controller values.
    juce::MidiBuffer processedMidi;
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        bool messageWasConsumed = false;

        if (message.isController())
        {
            const int controllerNumber = message.getControllerNumber();
            const float controllerValue = message.getControllerValue() / 127.0f;

            if (controllerNumber >= 70 && controllerNumber <= 77)
            {
                juce::String paramId;
                switch (controllerNumber)
                {
                case 70: paramId = "WAVETYPE";       break;
                case 71: paramId = "FILTER_CUTOFF";  break;
                case 72: paramId = "FILTER_RES";     break;
                case 73: paramId = "FILTER_ENV_AMT"; break;
                case 74: paramId = "AMP_ATTACK";     break;
                case 75: paramId = "AMP_DECAY";      break;
                case 76: paramId = "AMP_SUSTAIN";    break;
                case 77: paramId = "AMP_RELEASE";    break;
                }

                if (auto* param = apvts.getParameter(paramId))
                {
                    param->setValueNotifyingHost(controllerValue);
                    messageWasConsumed = true;
                }
            }
            else if (controllerNumber == 1) // Mod Wheel
            {
                smoothedModWheel.setTargetValue(controllerValue);
                messageWasConsumed = true;
            }
        }
        else if (message.isNoteOn())
        {
            const int noteNumber = message.getNoteNumber();
            if (noteNumber >= 36 && noteNumber <= 43)
            {
                juce::String paramIdToToggle;
                switch (noteNumber)
                {
                case 36: paramIdToToggle = "MONO_POLY";     break;
                case 37: paramIdToToggle = "SUB_OSC_ON";    break;
                case 38: paramIdToToggle = "LFO_PITCH_ON";  break;
                case 39: paramIdToToggle = "LFO_FILTER_ON"; break;
                case 40: paramIdToToggle = "GLIDE_ON";      break;
                case 41: paramIdToToggle = "WARP_ON";       break;
                case 42: paramIdToToggle = "NOISE_ON";      break;
                case 43: paramIdToToggle = "CHORUS_ON";     break;
                }

                if (auto* param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(paramIdToToggle)))
                {
                    param->setValueNotifyingHost(!(*param));
                    messageWasConsumed = true;
                }
            }
        }
        else if (message.isPitchWheel())
        {
            const float pitchBendValue = (static_cast<float>(message.getPitchWheelValue()) - 8192.0f) / 8192.0f;
            smoothedPitchBend.setTargetValue(pitchBendValue);
        }

        if (!messageWasConsumed)
        {
            processedMidi.addEvent(message, metadata.samplePosition);
        }
    }

    // 2. Now that parameters and smoothers are updated, push the latest values to the synth voices.
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setWaveType(oscWaveType);
            voice->setADSRParameters(ampAttack, ampDecay, ampSustain, ampRelease);
            voice->setFilterParameters(filterCutoff, filterResonance, filterEnvAmount);
            voice->setJoystickPositions(smoothedPitchBend.getNextValue(), smoothedModWheel.getNextValue());
        }
    }

    // 3. Render the audio using the filtered MIDI buffer.
    synth.renderNextBlock(buffer, processedMidi, 0, buffer.getNumSamples());
}

bool NKPK1AudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* NKPK1AudioProcessor::createEditor() { return new NKPK1AudioProcessorEditor(*this); }

void NKPK1AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NKPK1AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}
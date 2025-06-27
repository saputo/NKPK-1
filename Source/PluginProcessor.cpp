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
    synth.addSound(new SynthSound());

    for (int i = 0; i < numVoices; ++i)
    {
        SynthVoice* newVoice = new SynthVoice();
        newVoice->voiceIndex = i;
        synth.addVoice(newVoice);
    }
}

NKPK1AudioProcessor::~NKPK1AudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout NKPK1AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("WAVETYPE", "Wave Type", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF", "Filter Cutoff", juce::NormalisableRange<float>(100.0f, 6000.0f, 0.1f, 0.2f), 6000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FILTER_RES", "Filter Resonance", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FILTER_ENV_AMT", "Filter Env Amt", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("AMP_ATTACK", "Amp Attack", juce::NormalisableRange<float>(0.001f, 3.0f, 0.01f, 0.2f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("AMP_DECAY", "Amp Decay", juce::NormalisableRange<float>(0.001f, 3.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("AMP_SUSTAIN", "Amp Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("AMP_RELEASE", "Amp Release", juce::NormalisableRange<float>(0.001f, 3.0f, 0.01f, 0.3f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("MONO_POLY", "Mono/Poly", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("SUB_OSC_ON", "Sub-Oscillator", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_PITCH_ON", "LFO to Pitch", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_FILTER_ON", "LFO to Filter", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("GLIDE_ON", "Glide", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("WARP_ON", "Warp", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("NOISE_ON", "Noise", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("CHORUS_ON", "Chorus", false));

    return layout;
}

//==============================================================================
const juce::String NKPK1AudioProcessor::getName() const { return JucePlugin_Name; }
bool NKPK1AudioProcessor::acceptsMidi() const { return true; }
bool NKPK1AudioProcessor::producesMidi() const { return false; }
bool NKPK1AudioProcessor::isMidiEffect() const { return false; }
double NKPK1AudioProcessor::getTailLengthSeconds() const { return 0.0; }
int NKPK1AudioProcessor::getNumPrograms() { return 1; }
int NKPK1AudioProcessor::getCurrentProgram() { return 0; }
void NKPK1AudioProcessor::setCurrentProgram(int index) { (void)index; } // Silencing unused param warning
const juce::String NKPK1AudioProcessor::getProgramName(int index) { (void)index; return {}; } // Silencing unused param warning
void NKPK1AudioProcessor::changeProgramName(int index, const juce::String& newName) { (void)index; (void)newName; } // Silencing unused param warning

//==============================================================================
void NKPK1AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    smoothedPitchBend.reset(sampleRate, 0.02);
    smoothedModWheel.reset(sampleRate, 0.02);
    smoothedPitchBend.setCurrentAndTargetValue(0.0f);
    smoothedModWheel.setCurrentAndTargetValue(0.0f);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

void NKPK1AudioProcessor::releaseResources()
{
    synth.clearSounds();
    synth.clearVoices();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NKPK1AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This is the corrected line that fixes the build error
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::disabled())
        return false;

    return true;
}
#endif

//==============================================================================
void NKPK1AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    bool isMonoMode = static_cast<bool>(apvts.getRawParameterValue("MONO_POLY")->load());
    auto waveTypeValue = apvts.getRawParameterValue("WAVETYPE")->load();
    auto filterCutoffValue = apvts.getRawParameterValue("FILTER_CUTOFF")->load();
    auto filterResValue = apvts.getRawParameterValue("FILTER_RES")->load();
    auto filterEnvAmtValue = apvts.getRawParameterValue("FILTER_ENV_AMT")->load();
    auto ampAttackValue = apvts.getRawParameterValue("AMP_ATTACK")->load();
    auto ampDecayValue = apvts.getRawParameterValue("AMP_DECAY")->load();
    auto ampSustainValue = apvts.getRawParameterValue("AMP_SUSTAIN")->load();
    auto ampReleaseValue = apvts.getRawParameterValue("AMP_RELEASE")->load();

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setWaveType(waveTypeValue);
            voice->setADSRParameters(ampAttackValue, ampDecayValue, ampSustainValue, ampReleaseValue);
            voice->setFilterParameters(filterCutoffValue, filterResValue, filterEnvAmtValue);
            voice->setJoystickPositions(smoothedPitchBend.getNextValue(), smoothedModWheel.getNextValue());
        }
    }

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isController())
        {
            auto controllerNumber = message.getControllerNumber();
            float controllerValueRaw = (float)message.getControllerValue();

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
                default: break;
                }

                auto* param = apvts.getParameter(paramId);
                if (param)
                {
                    param->setValueNotifyingHost(controllerValueRaw / 127.0f);
                }
            }

            if (controllerNumber == 1)
            {
                smoothedModWheel.setTargetValue(controllerValueRaw / 127.0f);
            }
        }
        else if (message.isNoteOn())
        {
            auto noteNumber = message.getNoteNumber();
            if (noteNumber >= 36 && noteNumber <= 43)
            {
                juce::String paramIdToToggle;
                switch (noteNumber)
                {
                case 36: paramIdToToggle = "MONO_POLY"; break;
                case 37: paramIdToToggle = "SUB_OSC_ON"; break;
                case 38: paramIdToToggle = "LFO_PITCH_ON"; break;
                case 39: paramIdToToggle = "LFO_FILTER_ON"; break;
                case 40: paramIdToToggle = "GLIDE_ON"; break;
                case 41: paramIdToToggle = "WARP_ON"; break;
                case 42: paramIdToToggle = "NOISE_ON"; break;
                case 43: paramIdToToggle = "CHORUS_ON"; break;
                default: break;
                }

                auto* toggleParam = apvts.getParameter(paramIdToToggle);
                if (toggleParam)
                {
                    bool currentState = static_cast<bool>(toggleParam->getValue());
                    toggleParam->setValueNotifyingHost(!currentState ? 1.0f : 0.0f);
                }
            }
            else
            {
                if (isMonoMode)
                {
                    synth.allNotesOff(0, false);
                }
                synth.noteOn(message.getChannel(), message.getNoteNumber(), message.getVelocity() / 127.0f);
            }
        }
        else if (message.isNoteOff())
        {
            synth.noteOff(message.getChannel(), message.getNoteNumber(), message.getVelocity() / 127.0f, true);
        }
        else if (message.isPitchWheel())
        {
            int pitchWheelValue = message.getPitchWheelValue();
            float normalizedBend = (static_cast<float>(pitchWheelValue) - 8192.0f) / 8192.0f;
            smoothedPitchBend.setTargetValue(normalizedBend);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool NKPK1AudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* NKPK1AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
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

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NKPK1AudioProcessor();
}
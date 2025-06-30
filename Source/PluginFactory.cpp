#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Global function required by JUCE to create the plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NKPK1AudioProcessor();
}
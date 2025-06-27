#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NKPK1AudioProcessorEditor::NKPK1AudioProcessorEditor(NKPK1AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
}

NKPK1AudioProcessorEditor::~NKPK1AudioProcessorEditor()
{
}

//==============================================================================
void NKPK1AudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    // This is the line we changed
    g.drawFittedText("NKPK-1", getLocalBounds(), juce::Justification::centred, 1);
}

void NKPK1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
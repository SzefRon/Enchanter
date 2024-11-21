#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(textButton);
    textButton.setButtonText("Mode");
    textButton.setClickingTogglesState(true);
    textButton.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour::fromRGB(255, 0, 0));
    textButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour::fromRGB(0, 255, 0));

    textButton.onClick = [&]()
    {
        processorRef.mode ^= 1;
        std::cout << processorRef.mode;
    };

    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    textButton.setBounds(0, 0, 100, 100);
}

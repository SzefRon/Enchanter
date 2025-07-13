#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final :
    public juce::AudioProcessorEditor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~AudioPluginAudioProcessorEditor() override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::AudioProcessorValueTreeState &valueTreeState;

    juce::ToggleButton modeToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeToggleAttachement;

    juce::ComboBox fftSizeComboBox;
    juce::Label fftSizeLabel;

    juce::TextButton bypassButton;
    juce::Label bypassLabel;

    juce::Slider offsetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetAttachement;
    juce::Label offsetLabel;

    int offset = 0;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    void updateOffsetLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

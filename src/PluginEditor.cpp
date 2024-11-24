#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (410, 60);

    // --------- MODE TOGGLE BUTTON

    modeToggleButton.setButtonText("Mode 1");

    modeToggleButton.onClick = [&]() {
        bool state = modeToggleButton.getToggleState();
        if (!state) {
            modeToggleButton.setButtonText("Mode 1");
            processorRef.fftMode = false;
        }
        else {
            modeToggleButton.setButtonText("Mode 2");
            processorRef.fftMode = true;
        }
    };

    addAndMakeVisible(modeToggleButton);

    // --------- FFT SIZE COMBO BOX


    fftSizeComboBox.addItemList({"128", "256", "512", "1024", "2048", "4096", "8192"}, 1);
    fftSizeComboBox.setSelectedId(4, juce::NotificationType::dontSendNotification);
    fftSizeComboBox.onChange = [&]() {
        auto selectedID = fftSizeComboBox.getSelectedId();
        auto str = fftSizeComboBox.getItemText(selectedID - 1);
        auto value = std::stoi(str.toStdString());
        int order = static_cast<int>(std::log2f(value));

        processorRef.bypassed = true;
        bypassLabel.setText("Bypassed", juce::NotificationType::dontSendNotification);
        processorRef.changeOrder(order);
    };

    addAndMakeVisible(fftSizeComboBox);

    // --------- FFT SIZE LABEL

    fftSizeLabel.setText("FFT size", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(fftSizeLabel);

    // --------- BYPASS BUTTON

    bypassButton.setButtonText("Bypass");
    bypassButton.onClick = [&]() {
        processorRef.bypassed = true;
        bypassLabel.setText("Bypassed", juce::NotificationType::dontSendNotification);
    };

    addAndMakeVisible(bypassButton);

    // --------- BYPASS LABEL

    bypassLabel.setText("Bypassed", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(bypassLabel);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // int width = getWidth(), height = getHeight();
    // int padding = 10;

    modeToggleButton.setBounds(10, 10, 80, 40);
    fftSizeComboBox.setBounds(100, 10, 40, 40);
    fftSizeLabel.setBounds(150, 10, 60, 40);
    bypassButton.setBounds(220, 10, 100, 40);
    bypassLabel.setBounds(330, 10, 70, 40);
}

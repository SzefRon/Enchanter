#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState(vts),
    leftArrowButton("left", 0.5f, juce::Colours::white),
    rightArrowButton("right", 0.0f, juce::Colours::white)
{
    vts.addParameterListener("bypassed", this);

    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (590, 60);

    // --------- MODE TOGGLE BUTTON

    modeToggleButton.setButtonText("Mode 1");

    modeToggleButton.onStateChange = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        bool state = modeToggleButton.getToggleState();
        if (!state) {
            modeToggleButton.setButtonText("Mode 1");
        }
        else {
            modeToggleButton.setButtonText("Mode 2");
        }
    };

    addAndMakeVisible(modeToggleButton);
    modeToggleAttachement.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        valueTreeState, "fftMode", modeToggleButton
    ));

    // --------- FFT SIZE COMBO BOX


    fftSizeComboBox.addItemList({"128", "256", "512", "1024", "2048", "4096", "8192"}, 1);
    fftSizeComboBox.setSelectedId(processorRef.fftOrder->get() - 6, juce::NotificationType::dontSendNotification);
    fftSizeComboBox.onChange = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        auto selectedID = fftSizeComboBox.getSelectedId();
        auto str = fftSizeComboBox.getItemText(selectedID - 1);
        auto value = std::stoi(str.toStdString());
        int order = static_cast<int>(std::log2f(value));

        DBG(order);

        *processorRef.bypassed = true;
        *processorRef.fftOrder = order;
        processorRef.changeOrder(order);
    };

    addAndMakeVisible(fftSizeComboBox);

    // --------- FFT SIZE LABEL

    fftSizeLabel.setText("FFT size", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(fftSizeLabel);

    // --------- BYPASS BUTTON

    bypassButton.setButtonText("Bypass");
    bypassButton.onClick = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        *processorRef.bypassed = true;
        processorRef.fftProcessor.reset();
    };

    addAndMakeVisible(bypassButton);

    // --------- BYPASS LABEL

    bypassLabel.setText(processorRef.bypassed->get() ? "Bypassed" : "Active", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(bypassLabel);

    // --------- OFFSET ARROW BUTTONS

    leftArrowButton.onClick = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        int value = processorRef.fftProcessor.hopPos;
        processorRef.fftProcessor.hopPos = (value == 0) ? processorRef.fftProcessor.fftHopAmount - 1 : value - 1;
        value = processorRef.fftProcessor.samplePos;
        processorRef.fftProcessor.samplePos = (value == 0) ? processorRef.fftProcessor.fftSampleAmount - 1 : value - 1;
        offset -= 1;
        offsetLabel.setText(std::to_string(offset), juce::NotificationType::dontSendNotification);
    };

    rightArrowButton.onClick = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        int value = processorRef.fftProcessor.hopPos;
        processorRef.fftProcessor.hopPos = (value == processorRef.fftProcessor.fftHopAmount - 1) ? 0 : value + 1;
        value = processorRef.fftProcessor.samplePos;
        processorRef.fftProcessor.samplePos = (value == processorRef.fftProcessor.fftSampleAmount - 1) ? 0 : value + 1;
        offset += 1;
        offsetLabel.setText(std::to_string(offset), juce::NotificationType::dontSendNotification);
    };

    addAndMakeVisible(leftArrowButton);
    addAndMakeVisible(rightArrowButton);

    // --------- OFFSET LABEL

    offsetLabel.setText("0", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(offsetLabel);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    valueTreeState.removeParameterListener("bypassed", this);
}

void AudioPluginAudioProcessorEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "bypassed") {
        juce::MessageManager::callAsync([&]()
        {
            auto* bypassParam = valueTreeState.getRawParameterValue("bypassed");
            bool bypassed = bypassParam && (*bypassParam >= 0.5f);
            bypassLabel.setText(bypassed ? "Bypassed" : "Active", juce::dontSendNotification);
        });
    }
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
    leftArrowButton.setBounds(410, 10, 40, 40);
    rightArrowButton.setBounds(460, 10, 40, 40);
    offsetLabel.setBounds(510, 10, 40, 40);
}

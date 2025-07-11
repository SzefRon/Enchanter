#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState(vts)
{
    vts.addParameterListener("bypassed", this);

    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (510, 60);

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
    fftSizeComboBox.setSelectedId(processorRef.fftOrder->get() - 6, juce::dontSendNotification);
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

        offsetSlider.setRange(0.0, (1 << processorRef.fftOrder->get()) - 1, 1.0);
    };

    addAndMakeVisible(fftSizeComboBox);

    // --------- FFT SIZE LABEL

    fftSizeLabel.setText("FFT size", juce::dontSendNotification);
    fftSizeLabel.setJustificationType(juce::Justification::centred);

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

    bypassLabel.setText(processorRef.bypassed->get() ? "Bypassed" : "Active", juce::dontSendNotification);
    bypassLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(bypassLabel);

    // --------- OFFSET SLIDER

    offsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    offsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    offsetSlider.onValueChange = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        int offsetValue = static_cast<int>(std::lround(offsetSlider.getValue()));
        *processorRef.fftProcessor.hopPosOffset = offsetValue;

        offsetLabel.setText(std::to_string(offsetValue), juce::dontSendNotification);
    };

    addAndMakeVisible(offsetSlider);
    offsetAttachement.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "hopPosOffset", offsetSlider
    ));
    offsetSlider.setRange(0.0, (1 << processorRef.fftOrder->get()) - 1, 1.0);

    // --------- OFFSET LABEL

    offsetLabel.setText(std::to_string(processorRef.fftProcessor.hopPosOffset->get()), 
        juce::NotificationType::dontSendNotification);

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
    offsetSlider.setBounds(400, 0, 60, 60);
    offsetLabel.setBounds(460, 10, 40, 40);
}

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState(vts)
{
    setLookAndFeel(&cs);
    
    vts.addParameterListener("bypassed", this);
    vts.addParameterListener("fftOrder", this);
    vts.addParameterListener("hopPosOffsetPercent", this);

    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (480, 120);
    setResizable(false, false);

    // --------- MODE LABEL

    modeLabel.setText("MODE", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centred);
    modeLabel.setFont(CustomStyle::customFont.withHeight(30.0f));

    addAndMakeVisible(modeLabel);

    // --------- MODE TOGGLE BUTTON

    addAndMakeVisible(modeToggleButton);
    modeToggleAttachement.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        valueTreeState, "fftMode", modeToggleButton
    ));

    // --------- FFT SIZE COMBO BOX

    // fftSizeComboBox.setLookAndFeel(&testStyle);
    fftSizeComboBox.setJustificationType(juce::Justification::centred);
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
    };

    addAndMakeVisible(fftSizeComboBox);

    // --------- FFT SIZE LABEL

    fftSizeLabel.setText("FFT size", juce::dontSendNotification);
    fftSizeLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(fftSizeLabel);

    // --------- BYPASS BUTTON

    bypassButton.setButtonText("BYPASS");
    bypassButton.onClick = [&]() {
        std::lock_guard<std::mutex> lock(processorRef.mutex);
        *processorRef.bypassed = true;
        processorRef.fftProcessor.reset();
    };

    addAndMakeVisible(bypassButton);

    // --------- BYPASS DIODE

    bypassDiode.setState(!(*processorRef.bypassed));
    addAndMakeVisible(bypassDiode);

    // --------- OFFSET NAME LABEL

    offsetNameLabel.setText("OFFSET", juce::dontSendNotification);
    offsetNameLabel.setJustificationType(juce::Justification::centred);
    offsetNameLabel.setFont(CustomStyle::customFont.withHeight(30.0f));
    addAndMakeVisible(offsetNameLabel);

    // --------- OFFSET SLIDER

    offsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    offsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    addAndMakeVisible(offsetSlider);
    offsetAttachement.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "hopPosOffsetPercent", offsetSlider
    ));
    offsetSlider.setRange(0.0, 1.0);

    // --------- OFFSET LABEL

    offsetLabel.setInterceptsMouseClicks(false, false);
    offsetLabel.setJustificationType(juce::Justification::centred);
    offsetLabel.setFont(CustomStyle::customFont.withHeight(17.5f));
    updateOffsetLabel();

    addAndMakeVisible(offsetLabel);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    valueTreeState.removeParameterListener("bypassed", this);
}

void AudioPluginAudioProcessorEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    juce::ignoreUnused(newValue);
    if (parameterID == "bypassed") {
        juce::MessageManager::callAsync([&]()
        {
            auto* bypassParam = valueTreeState.getRawParameterValue("bypassed");
            bool bypassed = bypassParam && (*bypassParam >= 0.5f);
            bypassDiode.setState(!bypassed);
        });
    }
    else if (parameterID == "fftOrder") {
        juce::MessageManager::callAsync([&]()
        {
            updateOffsetLabel();
        });
    }
    else if (parameterID == "hopPosOffsetPercent") {
        juce::MessageManager::callAsync([&]()
        {
            updateOffsetLabel();
        });
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    juce::Image bg = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImage(bg, getBounds().toFloat());

    juce::Image logo = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    g.drawImage(logo, juce::Rectangle<float>(10.0f, 10.0f, 100.0f, 100.0f));

    //auto modeLabelBounds = juce::Rectangle<int>(393, 40, 70, 70);
    //g.fillRect(modeLabelBounds);
    
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // int width = getWidth(), height = getHeight();
    // int padding = 10;

    modeLabel.setBounds(120, 10, 80, 30);
    modeToggleButton.setBounds(125, 40, 70, 70);
    bypassButton.setBounds(210, 10, 110, 40);
    bypassDiode.setBounds(330, 10, 40, 40);
    fftSizeComboBox.setBounds(210, 60, 165, 50);
    offsetNameLabel.setBounds(385, 10, 85, 30);
    offsetSlider.setBounds(388, 40, 80, 80);
    offsetLabel.setBounds(388, 40, 80, 80);
    // fftSizeLabel.setBounds(150, 10, 60, 40);
    // bypassLabel.setBounds(330, 10, 70, 40);
}

void AudioPluginAudioProcessorEditor::updateOffsetLabel()
{
    int fftSampleAmount = 1 << processorRef.fftOrder->get();
    int hopPosOffset = static_cast<int>(
        std::lroundf(processorRef.fftProcessor.hopPosOffsetPercent->get() * (fftSampleAmount - 1))
    );
    offsetLabel.setText(std::to_string(hopPosOffset), juce::dontSendNotification);
}

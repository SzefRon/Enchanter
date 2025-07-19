#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "CustomAudioWaveVisualiser.h"

#include "FFTProcessor.h"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockIn (float *const *&writePtrs, const int &sampleCount);
    void processBlockOut (float *const *&writePtrs, const int &sampleCount);
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    juce::AudioProcessorValueTreeState parameters;
    juce::AudioParameterBool *fftMode = nullptr;
    juce::AudioParameterBool *bypassed = nullptr;
    juce::AudioParameterInt *fftOrder = nullptr;

    CustomAudioWaveVisualiser waveVisualiserTop;
    CustomAudioWaveVisualiser waveVisualiserBottom;

    bool operatingChannel = false;

    std::mutex mutex;

    void changeOrder(const int &order);

    FFTProcessor fftProcessor;
private:

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <vector>

class FFTProcessor
{
public:
    FFTProcessor();
    ~FFTProcessor();

    void reset();
    void changeOrder(const int &order);

    std::pair<float, float> processSampleIn(const float &sample);
    void processBlockIn();

    float processSampleOut(const float &sampleL, const float &sampleR);
    void processBlockOut();

    const int &getSamples() const;

    juce::AudioParameterInt *hopPosOffset = nullptr;

    int samplePos = 0;
    int hopPos = 0;
    int fftSampleAmount = 1 << fftOrder;
    int fftHopAmount = fftSampleAmount / 2;

private:
    int fftOrder = 10;

    std::array<std::vector<float>, 2> inputSamples;
    std::vector<float> fftSamples;
    std::array<std::vector<float>, 2> outputSamples;

    bool channelSwitch = false;

    std::unique_ptr<juce::dsp::FFT> processor;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
};


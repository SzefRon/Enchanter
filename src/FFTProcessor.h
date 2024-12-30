#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_dsp/juce_dsp.h>

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

    std::pair<float, float> processSampleOut(const float &sampleL, const float &sampleR);
    void processBlockOut();

    const int &getSamples() const;

private:
    int fftOrder = 10;
    int fftSampleAmount = 1 << fftOrder;
    int fftHopAmount = fftSampleAmount / 2;

    std::vector<float> samples;
    std::vector<float> fftSamples;
    std::array<std::vector<float>, 2> outputSamples;
    int samplePos = 0;
    int hopPos = 0;
    bool channelSwitch = false;

    juce::dsp::FFT processor;
    juce::dsp::WindowingFunction<float> window;
};


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

    float processSample(const float &sample, const bool &fftMode);

    void processBlockIn();
    void processBlockOut();

    const int &getSamples() const;

private:
    int fftOrder = 10;
    int fftSampleAmount = 1 << fftOrder;

    std::vector<float> samples;
    int samplePos = 0;

    juce::dsp::FFT processor;
};


#include "FFTProcessor.h"

FFTProcessor::FFTProcessor()
    : samples(fftSampleAmount << 1), processor(fftOrder)
{
}

FFTProcessor::~FFTProcessor()
{
}

void FFTProcessor::reset()
{
    std::fill(samples.begin(), samples.end(), 0.0f);
    samplePos = 0;
}

void FFTProcessor::changeOrder(const int &order)
{
    fftOrder = order;
    fftSampleAmount = 1 << fftOrder;

    samples = std::vector<float>(fftSampleAmount << 1);
    processor = juce::dsp::FFT(fftOrder);

    reset();
}

float FFTProcessor::processSample(const float &sample, const bool &fftMode)
{
    float outputSample = samples[samplePos];

    samples[samplePos] = sample;
    samplePos++;

    if (samplePos >= fftSampleAmount) {
        samplePos = 0;

        if (!fftMode)
            processBlockIn();
        else
            processBlockOut();
    }

    return outputSample;
}

void FFTProcessor::processBlockIn()
{
    auto data = samples.data();
    processor.performRealOnlyForwardTransform(data, true);
}

void FFTProcessor::processBlockOut()
{
    auto data = samples.data();
    processor.performRealOnlyInverseTransform(data);
}

const int &FFTProcessor::getSamples() const
{
    return fftSampleAmount;
}

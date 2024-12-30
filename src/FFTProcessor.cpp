#include "FFTProcessor.h"

FFTProcessor::FFTProcessor()
    : samples(fftSampleAmount << 1),
    fftSamples(fftSampleAmount << 1),
    outputSamples{std::vector<float>(fftSampleAmount << 1), std::vector<float>(fftSampleAmount << 1)},
    processor(fftOrder),
    window(fftSampleAmount + 1, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, false)
{
}

FFTProcessor::~FFTProcessor()
{
}

void FFTProcessor::reset()
{
    std::fill(samples.begin(), samples.end(), 0.0f);
    std::fill(fftSamples.begin(), fftSamples.end(), 0.0f);
    std::fill(outputSamples[0].begin(), outputSamples[0].end(), 0.0f);
    std::fill(outputSamples[1].begin(), outputSamples[1].end(), 0.0f);
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

std::pair<float, float> FFTProcessor::processSampleIn(const float &sample)
{
    float outputSampleL = outputSamples[0][samplePos];
    float outputSampleR = outputSamples[1][samplePos];

    samples[samplePos] = sample;
    
    samplePos++;
    if (samplePos >= fftSampleAmount) {
        samplePos = 0;
    }

    hopPos++;
    if (hopPos >= fftHopAmount) {
        hopPos = 0;
        processBlockIn();
    }

    return {outputSampleL, outputSampleR};
}

void FFTProcessor::processBlockIn()
{
    auto data = samples.data();
    auto fftData = fftSamples.data();
    
    std::memcpy(fftData, data + samplePos, (fftSampleAmount - samplePos) * sizeof(float));
    if (samplePos > 0) {
        std::memcpy(fftData + fftSampleAmount - samplePos, data, samplePos * sizeof(float));
    }

    window.multiplyWithWindowingTable(fftData, fftSampleAmount);
    processor.performRealOnlyForwardTransform(fftData, true);

    auto outputData = outputSamples[(!channelSwitch ? 0 : 1)].data();

    std::memcpy(outputData, fftData + fftSampleAmount - samplePos, samplePos * sizeof(float));
    std::memcpy(outputData + samplePos, fftData, (fftSampleAmount - samplePos) * sizeof(float));

    channelSwitch = !channelSwitch;
}

std::pair<float, float> FFTProcessor::processSampleOut(const float &sampleL, const float &sampleR)
{
    return std::pair<float, float>();
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

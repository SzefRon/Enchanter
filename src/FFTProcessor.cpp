#include "FFTProcessor.h"

FFTProcessor::FFTProcessor()
    : inputSamples{std::vector<float>(fftSampleAmount), std::vector<float>(fftSampleAmount)},
    fftSamples(fftSampleAmount << 1),
    outputSamples{std::vector<float>(fftSampleAmount), std::vector<float>(fftSampleAmount)},
    processor(std::make_unique<juce::dsp::FFT>(fftOrder)),
    window(std::make_unique<juce::dsp::WindowingFunction<float>>(fftSampleAmount + 1, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, true))
{
    reset();
}

FFTProcessor::~FFTProcessor()
{
}

void FFTProcessor::reset()
{
    std::fill(fftSamples.begin(), fftSamples.end(), 0.0f);
    
    for (int i = 0; i < 2; i++) {
        std::fill(inputSamples[i].begin(), inputSamples[i].end(), 0.0f);
        std::fill(outputSamples[i].begin(), outputSamples[i].end(), 0.0f);
    }

    samplePos = 0;
    hopPos = 0;
    channelSwitch = false;
}

void FFTProcessor::changeOrder(const int &order)
{
    fftOrder = order;
    fftSampleAmount = 1 << fftOrder;
    fftHopAmount = fftSampleAmount / 2;

    for (int i = 0; i < 2; i++) {
        inputSamples[i] = std::vector<float>(fftSampleAmount);
        outputSamples[i] = std::vector<float>(fftSampleAmount);
    }
    fftSamples = std::vector<float>(fftSampleAmount << 1);

    processor = std::make_unique<juce::dsp::FFT>(fftOrder);
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSampleAmount + 1, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, true);

    reset();
}

std::pair<float, float> FFTProcessor::processSampleIn(const float &sample)
{
    float outputSampleL = outputSamples[0][samplePos];
    float outputSampleR = outputSamples[1][samplePos];

    inputSamples[0][samplePos] = sample;
    
    samplePos++;
    if (samplePos >= fftSampleAmount) {
        samplePos = 0;
    }

    hopPos++;
    int hopPosOffset = static_cast<int>(
        std::lroundf(hopPosOffsetPercent->get() * (fftSampleAmount - 1))
    );
    int hopPosFinal = hopPos + hopPosOffset;
    if (hopPosFinal >= fftHopAmount) {
        hopPos -= fftHopAmount;
        processBlockIn();
    }

    return {outputSampleL, outputSampleR};
}

void FFTProcessor::processBlockIn()
{
    auto data = inputSamples[0].data();
    auto fftData = fftSamples.data();
    
    std::memcpy(fftData, data + samplePos, (fftSampleAmount - samplePos) * sizeof(float));
    if (samplePos > 0) {
        std::memcpy(fftData + fftSampleAmount - samplePos, data, samplePos * sizeof(float));
    }

    window->multiplyWithWindowingTable(fftData, fftSampleAmount);
    processor->performRealOnlyForwardTransform(fftData, true);

    int compensation = (fftSampleAmount / 2);
    for (int i = 0; i < fftSampleAmount; i++) {
        fftData[i] /= compensation;
    }

    // auto complexData = reinterpret_cast<std::complex<float> *>(fftData);
    // for (int i = 0; i < fftSampleAmount; i++) {
    //     auto val = complexData[i];
    //     complexData[i] = std::complex<float>(std::abs(val) * 0.31831f, std::arg(val) * 0.31831f);
    // }

    int channel = !channelSwitch ? 0 : 1;
    auto outputData = outputSamples[channel].data();

    std::memcpy(outputData, fftData + fftSampleAmount - samplePos, samplePos * sizeof(float));
    std::memcpy(outputData + samplePos, fftData, (fftSampleAmount - samplePos) * sizeof(float));

    channelSwitch = !channelSwitch;
}

float FFTProcessor::processSampleOut(const float &sampleL, const float &sampleR)
{
    float outputSample = outputSamples[0][samplePos];
    outputSamples[0][samplePos] = 0.0f;

    inputSamples[0][samplePos] = sampleL;
    inputSamples[1][samplePos] = sampleR;
    
    samplePos++;
    if (samplePos >= fftSampleAmount) {
        samplePos = 0;
    }

    hopPos++;
    int hopPosOffset = static_cast<int>(
        std::lroundf(hopPosOffsetPercent->get() * (fftSampleAmount - 1))
    );
    int hopPosFinal = hopPos + hopPosOffset;
    if (hopPosFinal >= fftHopAmount) {
        hopPos -= fftHopAmount;
        processBlockOut();
    }

    return outputSample;
}

void FFTProcessor::processBlockOut()
{
    int channel = !channelSwitch ? 0 : 1;
    auto data = inputSamples[channel].data();
    auto fftData = fftSamples.data();
    
    std::memcpy(fftData, data + samplePos, (fftSampleAmount - samplePos) * sizeof(float));
    if (samplePos > 0) {
        std::memcpy(fftData + fftSampleAmount - samplePos, data, samplePos * sizeof(float));
    }

    // auto complexData = reinterpret_cast<std::complex<float> *>(fftData);
    // for (int i = 0; i < fftSampleAmount; i++) {
    //     auto val = complexData[i];
    //     complexData[i] = std::polar(val.real() * 3.14159f, val.imag() * 3.14159f);
    // }

    // Scale down audio to compensate windowing
    int compensation = (fftSampleAmount / 2);
    for (int i = 0; i < fftSampleAmount; i++) {
        fftData[i] *= 0.5f;
        fftData[i] *= compensation;
    }

    processor->performRealOnlyInverseTransform(fftData);
    // No need for 2nd windowing. Otherwise hann won't add up.
    // window->multiplyWithWindowingTable(fftData, fftSampleAmount);


    auto outputData = outputSamples[0].data();

    for (int i = 0; i < samplePos; i++) {
        outputData[i] += fftData[i + fftSampleAmount - samplePos];
    }
    for (int i = 0; i < fftSampleAmount - samplePos; ++i) {
        outputData[i + samplePos] += fftData[i];
    }

    channelSwitch = !channelSwitch;
}

const int &FFTProcessor::getSamples() const
{
    return fftSampleAmount;
}

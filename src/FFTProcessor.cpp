#include "FFTProcessor.h"

FFTProcessor::FFTProcessor() :
    fft(fftOrder),
    window(fftSize + 1, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, false)
{
    // Note that the window is of length `fftSize + 1` because JUCE's windows
    // are symmetrical, which is wrong for overlap-add processing. To make the
    // window periodic, set size to 1025 but only use the first 1024 samples.
}

void FFTProcessor::reset()
{
    count = 0;
    pos = 0;

    // Zero out the circular buffers.
    std::fill(inputFifo.begin(), inputFifo.end(), 0.0f);
    std::fill(outputFifo.begin(), outputFifo.end(), 0.0f);
}

void FFTProcessor::processBlock(float* data, int numSamples)
{
    // for (int i = 0; i < numSamples; ++i) {
    //     data[i] = processSample(data[i]);
    // }
}

float FFTProcessor::processSample(float sample, bool mode)
{
    // Push the new sample value into the input FIFO.
    inputFifo[pos] = sample;

    // Read the output value from the output FIFO. Since it takes fftSize
    // timesteps before actual samples are read from this FIFO instead of
    // the initial zeros, the sound output is delayed by fftSize samples,
    // which we will report as our latency.
    float outputSample = outputFifo[pos];

    // Once we've read the sample, set this position in the FIFO back to
    // zero so we can add the IFFT results to it later.
    outputFifo[pos] = 0.0f;

    // Advance the FIFO index and wrap around if necessary.
    pos += 1;
    if (pos == fftSize) {
        pos = 0;
        // if (!mode)
        //     processFrameInputTest();
        // else
        //     processFrameOutputTest();
    }

    // Process the FFT frame once we've collected hopSize samples.
    count += 1;
    if (count == hopSize) {
        count = 0;
        if (!mode)
            processFrameInput();
        else
            processFrameOutput();
    }

    return outputSample;
}

void FFTProcessor::processFrameInputTest()
{
    const float* inputPtr = inputFifo.data();
    float* fftPtr = fftData.data();

    // Copy the input FIFO into the FFT working space in two parts.
    std::memcpy(fftPtr, inputPtr, (fftSize) * sizeof(float));

    float energyBeforeFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyBeforeFFT += fftPtr[i] * fftPtr[i];
    }

    // Perform the forward FFT.
    fft.performRealOnlyForwardTransform(fftPtr, true);
    // fft.performRealOnlyInverseTransform(fftPtr);
    // fft.performRealOnlyForwardTransform(fftPtr);

    // Do stuff with the FFT data.
    // processSpectrum(fftPtr, numBins);

    float energyAfterFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyAfterFFT += fftPtr[i] * fftPtr[i];
    }

    // Scale the FFT data to conserve the energy.
    if (energyAfterFFT > 0.0f) {
        float scaleFactor = std::sqrt(energyBeforeFFT / energyAfterFFT);
        for (int i = 0; i < fftSize; ++i) {
            fftPtr[i] *= scaleFactor;
        }
    }

    // Add the IFFT results to the output FIFO.
    for (int i = 0; i < fftSize - pos; ++i) {
        outputFifo[i + pos] = fftData[i];
    }
}

void FFTProcessor::processFrameOutputTest()
{
    const float* inputPtr = inputFifo.data();
    float* fftPtr = fftData.data();

    // Copy the input FIFO into the FFT working space in two parts.
    std::memcpy(fftPtr, inputPtr, (fftSize) * sizeof(float));

    float energyBeforeFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyBeforeFFT += fftPtr[i] * fftPtr[i];
    }

    // Perform the forward FFT.
    // fft.performRealOnlyForwardTransform(fftPtr, true);
    fft.performRealOnlyInverseTransform(fftPtr);

    // Do stuff with the FFT data.
    // processSpectrum(fftPtr, numBins);

    float energyAfterFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyAfterFFT += fftPtr[i] * fftPtr[i];
    }

    // Scale the FFT data to conserve the energy.
    if (energyAfterFFT > 0.0f) {
        float scaleFactor = std::sqrt(energyBeforeFFT / energyAfterFFT);
        for (int i = 0; i < fftSize; ++i) {
            fftPtr[i] *= scaleFactor;
        }
    }

    // Add the IFFT results to the output FIFO.
    for (int i = 0; i < fftSize - pos; ++i) {
        outputFifo[i + pos] = fftData[i];
    }
}

void FFTProcessor::processFrameInput()
{
    const float* inputPtr = inputFifo.data();
    float* fftPtr = fftData.data();

    // Copy the input FIFO into the FFT working space in two parts.
    std::memcpy(fftPtr, inputPtr + pos, (fftSize - pos) * sizeof(float));
    if (pos > 0) {
        std::memcpy(fftPtr + fftSize - pos, inputPtr, pos * sizeof(float));
    }

    float energyBeforeFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyBeforeFFT += fftPtr[i] * fftPtr[i];
    }

    // Apply the window to avoid spectral leakage.
    window.multiplyWithWindowingTable(fftPtr, fftSize);

    // Perform the forward FFT.
    fft.performRealOnlyForwardTransform(fftPtr, false);
    fft.performRealOnlyInverseTransform(fftPtr);

    // Do stuff with the FFT data.
    // processSpectrum(fftPtr, numBins);

    float energyAfterFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyAfterFFT += fftPtr[i] * fftPtr[i];
    }

    // Scale the FFT data to conserve the energy.
    // if (energyAfterFFT > 0.0f) {
    //     float scaleFactor = std::sqrt(energyBeforeFFT / energyAfterFFT);
    //     for (int i = 0; i < fftSize; ++i) {
    //         fftPtr[i] *= scaleFactor;
    //     }
    // }

    // Apply the window again for resynthesis.
    window.multiplyWithWindowingTable(fftPtr, fftSize);

    // Scale down the output samples because of the overlapping windows.
    for (int i = 0; i < fftSize; ++i) {
        fftPtr[i] *= windowCorrection;
    }

    // Add the IFFT results to the output FIFO.
    for (int i = 0; i < pos; ++i) {
        outputFifo[i] += fftData[i + fftSize - pos];
    }
    for (int i = 0; i < fftSize - pos; ++i) {
        outputFifo[i + pos] += fftData[i];
    }
}

void FFTProcessor::processFrameOutput()
{
    const float* inputPtr = inputFifo.data();
    float* fftPtr = fftData.data();

    // Copy the input FIFO into the FFT working space in two parts.
    std::memcpy(fftPtr, inputPtr + pos, (fftSize - pos) * sizeof(float));
    if (pos > 0) {
        std::memcpy(fftPtr + fftSize - pos, inputPtr, pos * sizeof(float));
    }

    float energyBeforeFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyBeforeFFT += fftPtr[i] * fftPtr[i];
    }

    // Apply the window to avoid spectral leakage.
    window.multiplyWithWindowingTable(fftPtr, fftSize);

    // Perform the forward FFT.
    // fft.performRealOnlyForwardTransform(fftPtr, true);
    fft.performRealOnlyInverseTransform(fftPtr);

    // Do stuff with the FFT data.
    // processSpectrum(fftPtr, numBins);

    float energyAfterFFT = 0.0f;
    for (int i = 0; i < fftSize; ++i) {
        energyAfterFFT += fftPtr[i] * fftPtr[i];
    }

    // Scale the FFT data to conserve the energy.
    if (energyAfterFFT > 0.0f) {
        float scaleFactor = std::sqrt(energyBeforeFFT / energyAfterFFT);
        for (int i = 0; i < fftSize; ++i) {
            fftPtr[i] *= scaleFactor;
        }
    }

    // Apply the window again for resynthesis.
    window.multiplyWithWindowingTable(fftPtr, fftSize);

    // Scale down the output samples because of the overlapping windows.
    for (int i = 0; i < fftSize; ++i) {
        fftPtr[i] *= windowCorrection;
    }

    // Add the IFFT results to the output FIFO.
    for (int i = 0; i < pos; ++i) {
        outputFifo[i] += fftData[i + fftSize - pos];
    }
    for (int i = 0; i < fftSize - pos; ++i) {
        outputFifo[i + pos] += fftData[i];
    }
}

void FFTProcessor::processSpectrum(float* data, int numBins)
{
    // The spectrum data is floats organized as [re, im, re, im, ...]
    // but it's easier to deal with this as std::complex values.
    auto* cdata = reinterpret_cast<std::complex<float>*>(data);

    std::reverse(cdata, cdata + numBins);
}
#include "dsp/STFTProcessor.h"
#include <cmath>

void STFTProcessor::prepare(int newFftOrder, int hopSize)
{
    fftOrder = newFftOrder;
    fftSize = 1 << fftOrder;
    hop = hopSize;

    fft = juce::dsp::FFT(fftOrder);
    fftTimeDomain.assign(2 * fftSize, 0.0f);
    window.resize(fftSize);

    for (int i = 0; i < fftSize; ++i)
        window[static_cast<size_t>(i)] = 0.5f * (1.0f - std::cos(2.0 * juce::MathConstants<double>::pi * i / (fftSize - 1)));
}

SpectralFrame STFTProcessor::forward(const juce::AudioBuffer<float>& monoBlock)
{
    jassert(monoBlock.getNumChannels() == 1);

    SpectralFrame frame;
    frame.real.assign(static_cast<size_t>(fftSize), 0.0f);
    frame.imag.assign(static_cast<size_t>(fftSize), 0.0f);

    const auto* input = monoBlock.getReadPointer(0);
    const int samples = juce::jmin(monoBlock.getNumSamples(), fftSize);

    for (int i = 0; i < samples; ++i)
        fftTimeDomain[static_cast<size_t>(i)] = input[i] * window[static_cast<size_t>(i)];

    std::fill(fftTimeDomain.begin() + samples, fftTimeDomain.end(), 0.0f);

    fft.performRealOnlyForwardTransform(fftTimeDomain.data());

    for (int bin = 0; bin < fftSize; ++bin)
    {
        frame.real[static_cast<size_t>(bin)] = fftTimeDomain[static_cast<size_t>(2 * bin)];
        frame.imag[static_cast<size_t>(bin)] = fftTimeDomain[static_cast<size_t>(2 * bin + 1)];
    }

    return frame;
}

void STFTProcessor::inverse(const SpectralFrame& frame, juce::AudioBuffer<float>& outMonoBlock)
{
    jassert(outMonoBlock.getNumChannels() == 1);

    std::fill(fftTimeDomain.begin(), fftTimeDomain.end(), 0.0f);

    const auto bins = juce::jmin(fftSize, static_cast<int>(frame.real.size()));
    for (int bin = 0; bin < bins; ++bin)
    {
        fftTimeDomain[static_cast<size_t>(2 * bin)] = frame.real[static_cast<size_t>(bin)];
        fftTimeDomain[static_cast<size_t>(2 * bin + 1)] = frame.imag[static_cast<size_t>(bin)];
    }

    fft.performRealOnlyInverseTransform(fftTimeDomain.data());

    auto* output = outMonoBlock.getWritePointer(0);
    const int outSamples = juce::jmin(outMonoBlock.getNumSamples(), fftSize);

    for (int i = 0; i < outSamples; ++i)
        output[i] = (fftTimeDomain[static_cast<size_t>(i)] * window[static_cast<size_t>(i)]) / static_cast<float>(fftSize);
}

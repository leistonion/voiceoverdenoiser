#pragma once

#include <juce_dsp/juce_dsp.h>

struct SpectralFrame
{
    std::vector<float> real;
    std::vector<float> imag;
};

class STFTProcessor
{
public:
    void prepare(int fftOrder, int hopSize);
    SpectralFrame forward(const juce::AudioBuffer<float>& monoBlock);
    void inverse(const SpectralFrame& frame, juce::AudioBuffer<float>& outMonoBlock);

    [[nodiscard]] int getFftSize() const noexcept { return fftSize; }

private:
    int fftOrder = 10;
    int fftSize = 1 << fftOrder;
    int hop = 256;

    juce::dsp::FFT fft { fftOrder };
    std::vector<float> fftTimeDomain;
    std::vector<float> window;
};

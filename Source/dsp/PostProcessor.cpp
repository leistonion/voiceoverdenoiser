#include "dsp/PostProcessor.h"
#include <cmath>

void PostProcessor::setVoicePreservation(float value01) noexcept
{
    voicePreservation = juce::jlimit(0.0f, 1.0f, value01);
}

void PostProcessor::setFineTune(float value01) noexcept
{
    fineTune = juce::jlimit(0.0f, 1.0f, value01);
}

void PostProcessor::refineMask(SpectralFrame& frame) const
{
    const float alpha = juce::jmap(fineTune, 0.05f, 0.35f);

    for (size_t i = 1; i + 1 < frame.real.size(); ++i)
    {
        const float magL = std::hypot(frame.real[i - 1], frame.imag[i - 1]);
        const float magC = std::hypot(frame.real[i], frame.imag[i]);
        const float magR = std::hypot(frame.real[i + 1], frame.imag[i + 1]);

        const float smoothed = (magL + magC + magR) / 3.0f;
        const float preserve = juce::jmap(voicePreservation, 0.1f, 1.0f);
        const float gain = juce::jlimit(0.25f, 1.2f, preserve + alpha * (smoothed / (magC + 1.0e-7f)));

        frame.real[i] *= gain;
        frame.imag[i] *= gain;
    }
}

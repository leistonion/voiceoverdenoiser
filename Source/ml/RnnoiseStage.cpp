#include "ml/RnnoiseStage.h"

void RnnoiseStage::prepare(double sampleRate, int frameSize)
{
    currentSampleRate = sampleRate;
    frame = frameSize;
}

void RnnoiseStage::process(juce::AudioBuffer<float>& monoBuffer, float suppressionAmount) const
{
    jassert(monoBuffer.getNumChannels() == 1);

    auto* data = monoBuffer.getWritePointer(0);
    const int n = monoBuffer.getNumSamples();
    const float alpha = juce::jmap(juce::jlimit(0.0f, 1.0f, suppressionAmount), 0.995f, 0.92f);

    float running = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        // Baseline stationary-noise suppression emulates RNNoise-like pre-cleaning.
        running = alpha * running + (1.0f - alpha) * data[i];
        data[i] -= (data[i] - running) * suppressionAmount * 0.35f;
    }

    juce::ignoreUnused(currentSampleRate, frame);
}

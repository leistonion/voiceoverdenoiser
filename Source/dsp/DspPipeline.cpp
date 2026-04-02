#include "dsp/DspPipeline.h"
#include <cmath>

void DspPipeline::prepare(double sr, int samplesPerBlock, int numChannels)
{
    sampleRate = sr;

    int fftOrder = 10;
    if (latencyMode == 0)
        fftOrder = 9;  // 512
    else if (latencyMode == 2)
        fftOrder = 11; // 2048

    stft.prepare(fftOrder, (1 << fftOrder) / 4);
    rnnoise.prepare(sr, 480);

    monoScratch.setSize(1, juce::jmax(samplesPerBlock, stft.getFftSize()));

    modelManager.ensureModelsAvailable();
    modelManager.loadForTier(static_cast<ModelManager::ModelTier>(latencyMode), denoiseModel, dereverbModel);

    juce::ignoreUnused(numChannels);
    updateSmoothingCoefficients();
}

void DspPipeline::process(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() == 0)
        return;

    monoScratch.setSize(1, buffer.getNumSamples(), false, false, true);
    monoScratch.clear();

    // Sum-to-mono inference path keeps model CPU cost bounded for realtime operation.
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        monoScratch.addFrom(0, 0, buffer, ch, 0, buffer.getNumSamples(), 1.0f / static_cast<float>(buffer.getNumChannels()));

    // Stage 1: lightweight suppression targeting stationary background noise only.
    rnnoise.process(monoScratch, noiseReductionAmount * noiseProfileBias);

    // Stage 2: STFT domain neural denoise + dereverb (frame-sized for low latency).
    auto spectrum = stft.forward(monoScratch);

    if (denoiseModel.isReady())
        denoiseModel.processFrame(spectrum, noiseReductionAmount);

    dereverb.setAmount(dereverbAmount);
    dereverb.process(spectrum);

    post.setFineTune(fineTune);
    post.setVoicePreservation(voicePreservation);
    post.refineMask(spectrum);

    stft.inverse(spectrum, monoScratch);

    // Smoothing avoids zipper artifacts when hosts automate macro controls.
    auto* clean = monoScratch.getReadPointer(0);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const float target = juce::jlimit(0.2f, 1.0f, 1.0f - 0.55f * noiseReductionAmount + 0.25f * voicePreservation);
        envelope = (target < envelope) ? attackCoeff * (envelope - target) + target
                                       : releaseCoeff * (envelope - target) + target;

        const float mixed = clean[sample] * envelope + clean[sample] * (1.0f - envelope) * fineTune;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, sample, mixed);
    }
}

void DspPipeline::setNoiseReductionAmount(float value01) noexcept { noiseReductionAmount = juce::jlimit(0.0f, 1.0f, value01); }
void DspPipeline::setDereverbAmount(float value01) noexcept { dereverbAmount = juce::jlimit(0.0f, 1.0f, value01); }
void DspPipeline::setFineTune(float value01) noexcept { fineTune = juce::jlimit(0.0f, 1.0f, value01); }
void DspPipeline::setNoiseProfileBias(float value01) noexcept { noiseProfileBias = juce::jlimit(0.0f, 1.0f, value01); }
void DspPipeline::setVoicePreservation(float value01) noexcept { voicePreservation = juce::jlimit(0.0f, 1.0f, value01); }

void DspPipeline::setAttackRelease(float newAttackMs, float newReleaseMs) noexcept
{
    attackMs = juce::jmax(1.0f, newAttackMs);
    releaseMs = juce::jmax(10.0f, newReleaseMs);
    updateSmoothingCoefficients();
}

void DspPipeline::setLatencyMode(int modeIndex)
{
    const int clamped = juce::jlimit(0, 2, modeIndex);
    if (clamped == latencyMode)
        return;

    latencyMode = clamped;
    prepare(sampleRate, monoScratch.getNumSamples(), 1);
}

void DspPipeline::updateSmoothingCoefficients()
{
    attackCoeff = std::exp(-1.0f / (0.001f * attackMs * static_cast<float>(sampleRate)));
    releaseCoeff = std::exp(-1.0f / (0.001f * releaseMs * static_cast<float>(sampleRate)));
}

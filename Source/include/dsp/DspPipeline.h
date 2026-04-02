#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "dsp/STFTProcessor.h"
#include "dsp/PostProcessor.h"
#include "ml/ModelManager.h"
#include "ml/OnnxModelRunner.h"
#include "ml/RnnoiseStage.h"
#include "ml/DereverbStage.h"

class DspPipeline
{
public:
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void process(juce::AudioBuffer<float>& buffer);

    void setNoiseReductionAmount(float value01) noexcept;
    void setDereverbAmount(float value01) noexcept;
    void setFineTune(float value01) noexcept;
    void setNoiseProfileBias(float value01) noexcept;
    void setVoicePreservation(float value01) noexcept;
    void setAttackRelease(float attackMs, float releaseMs) noexcept;
    void setLatencyMode(int modeIndex);

private:
    void updateSmoothingCoefficients();

    STFTProcessor stft;
    PostProcessor post;
    RnnoiseStage rnnoise;
    DereverbStage dereverb;
    OnnxModelRunner denoiseModel;
    OnnxModelRunner dereverbModel;
    ModelManager modelManager;

    double sampleRate = 48000.0;
    float noiseReductionAmount = 0.5f;
    float dereverbAmount = 0.4f;
    float fineTune = 0.5f;
    float noiseProfileBias = 0.8f;
    float voicePreservation = 0.85f;
    float attackMs = 20.0f;
    float releaseMs = 100.0f;

    int latencyMode = 1;
    float envelope = 1.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;

    juce::AudioBuffer<float> monoScratch;
};

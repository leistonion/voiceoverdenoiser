#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class RnnoiseStage
{
public:
    void prepare(double sampleRate, int frameSize);
    void process(juce::AudioBuffer<float>& monoBuffer, float suppressionAmount) const;

private:
    double currentSampleRate = 48000.0;
    int frame = 480;
};

#include <juce_dsp/juce_dsp.h>
#include "dsp/STFTProcessor.h"
#include "dsp/PostProcessor.h"
#include <cmath>

int main()
{
    STFTProcessor stft;
    stft.prepare(10, 256);

    juce::AudioBuffer<float> mono(1, 1024);
    mono.clear();
    for (int i = 0; i < mono.getNumSamples(); ++i)
        mono.setSample(0, i, std::sin(2.0f * juce::MathConstants<float>::pi * i / 32.0f) * 0.2f);

    auto frame = stft.forward(mono);

    PostProcessor post;
    post.setFineTune(0.5f);
    post.setVoicePreservation(0.9f);
    post.refineMask(frame);

    stft.inverse(frame, mono);

    return mono.getRMSLevel(0, 0, mono.getNumSamples()) > 0.0001f ? 0 : 1;
}

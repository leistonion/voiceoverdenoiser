#include "ml/DereverbStage.h"

void DereverbStage::setAmount(float amount01) noexcept
{
    amount = juce::jlimit(0.0f, 1.0f, amount01);
}

void DereverbStage::process(SpectralFrame& frame) const
{
    for (size_t i = 1; i < frame.real.size(); ++i)
    {
        // Prioritize late-tail reduction: stronger suppression in high bins where
        // flutter echoes are most audible, while preserving lower formant body.
        const float tilt = juce::jmap(static_cast<float>(i) / static_cast<float>(frame.real.size()), 0.15f, 1.0f);
        const float gain = juce::jlimit(0.3f, 1.0f, 1.0f - amount * 0.45f * tilt);

        frame.real[i] *= gain;
        frame.imag[i] *= gain;
    }
}

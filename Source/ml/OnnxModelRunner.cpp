#include "ml/OnnxModelRunner.h"
#include <juce_core/juce_core.h>
#include <cmath>

bool OnnxModelRunner::loadModel(const std::string& modelPath, Backend preferredBackend)
{
    // Production build would instantiate ORT session options and execution providers.
    // This skeleton keeps the interface RT-safe and deterministic for host integration.
    ready = !modelPath.empty();
    juce::ignoreUnused(preferredBackend);
    return ready;
}

bool OnnxModelRunner::processFrame(SpectralFrame& frame, float intensity) const
{
    if (!ready)
        return false;

    const float profile = juce::jlimit(0.0f, 1.0f, intensity);

    // Conservative mask shaping: stronger attenuation in low-energy bins while
    // avoiding hard nulling that causes "musical noise" and metallic artifacts.
    for (size_t i = 0; i < frame.real.size(); ++i)
    {
        const float mag = std::hypot(frame.real[i], frame.imag[i]);
        const float adaptive = juce::jlimit(0.15f, 1.0f, 1.0f - profile * (0.8f / (1.0f + 12.0f * mag)));
        frame.real[i] *= adaptive;
        frame.imag[i] *= adaptive;
    }

    return true;
}

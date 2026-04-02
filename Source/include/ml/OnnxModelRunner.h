#pragma once

#include "dsp/STFTProcessor.h"

class OnnxModelRunner
{
public:
    enum class Backend
    {
        CPU,
        CUDA
    };

    bool loadModel(const std::string& modelPath, Backend preferredBackend);
    bool processFrame(SpectralFrame& frame, float intensity) const;

    [[nodiscard]] bool isReady() const noexcept { return ready; }

private:
    bool ready = false;
};

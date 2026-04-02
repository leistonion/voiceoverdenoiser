#pragma once

#include "dsp/STFTProcessor.h"

class PostProcessor
{
public:
    void setVoicePreservation(float value01) noexcept;
    void setFineTune(float value01) noexcept;

    void refineMask(SpectralFrame& frame) const;

private:
    float voicePreservation = 0.8f;
    float fineTune = 0.5f;
};

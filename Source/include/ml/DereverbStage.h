#pragma once

#include "dsp/STFTProcessor.h"

class DereverbStage
{
public:
    void setAmount(float amount01) noexcept;
    void process(SpectralFrame& frame) const;

private:
    float amount = 0.5f;
};

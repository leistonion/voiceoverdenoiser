#pragma once

#include <juce_core/juce_core.h>
#include "ml/OnnxModelRunner.h"

class ModelManager
{
public:
    enum class ModelTier
    {
        Lightweight,
        Balanced,
        HighQuality
    };

    bool ensureModelsAvailable();
    bool loadForTier(ModelTier tier, OnnxModelRunner& denoiseModel, OnnxModelRunner& dereverbModel);

private:
    juce::File getModelRoot() const;
    juce::File getDenoiseModelPath(ModelTier tier) const;
    juce::File getDereverbModelPath(ModelTier tier) const;
};

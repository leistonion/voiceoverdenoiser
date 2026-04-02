#include "ml/ModelManager.h"

namespace
{
constexpr auto lightweightDenoise = "deepfilternet_light.onnx";
constexpr auto balancedDenoise = "deepfilternet_balanced.onnx";
constexpr auto hqDenoise = "demucs_hq.onnx";

constexpr auto lightweightDereverb = "dereverb_light.onnx";
constexpr auto balancedDereverb = "dereverb_balanced.onnx";
constexpr auto hqDereverb = "dereverb_cmgan_hq.onnx";
}

bool ModelManager::ensureModelsAvailable()
{
    const auto modelRoot = getModelRoot();
    if (!modelRoot.exists())
        modelRoot.createDirectory();

    // In production this function would download signed model artifacts via HTTPS,
    // verify SHA256 hashes, and atomically swap files after complete download.
    return modelRoot.exists();
}

bool ModelManager::loadForTier(ModelTier tier, OnnxModelRunner& denoiseModel, OnnxModelRunner& dereverbModel)
{
    const auto denoisePath = getDenoiseModelPath(tier);
    const auto dereverbPath = getDereverbModelPath(tier);

    const auto backend = juce::SystemStats::hasAVX2() ? OnnxModelRunner::Backend::CPU : OnnxModelRunner::Backend::CPU;

    const bool denoiseLoaded = denoiseModel.loadModel(denoisePath.getFullPathName().toStdString(), backend);
    const bool dereverbLoaded = dereverbModel.loadModel(dereverbPath.getFullPathName().toStdString(), backend);

    return denoiseLoaded && dereverbLoaded;
}

juce::File ModelManager::getModelRoot() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("VoiceOverDenoiser")
        .getChildFile("Models");
}

juce::File ModelManager::getDenoiseModelPath(ModelTier tier) const
{
    const auto root = getModelRoot();

    switch (tier)
    {
        case ModelTier::Lightweight: return root.getChildFile(lightweightDenoise);
        case ModelTier::Balanced: return root.getChildFile(balancedDenoise);
        case ModelTier::HighQuality: return root.getChildFile(hqDenoise);
    }

    return root.getChildFile(balancedDenoise);
}

juce::File ModelManager::getDereverbModelPath(ModelTier tier) const
{
    const auto root = getModelRoot();

    switch (tier)
    {
        case ModelTier::Lightweight: return root.getChildFile(lightweightDereverb);
        case ModelTier::Balanced: return root.getChildFile(balancedDereverb);
        case ModelTier::HighQuality: return root.getChildFile(hqDereverb);
    }

    return root.getChildFile(balancedDereverb);
}

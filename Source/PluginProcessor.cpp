#include "PluginProcessor.h"
#include "PluginEditor.h"

VoiceOverDenoiserAudioProcessor::VoiceOverDenoiserAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

void VoiceOverDenoiserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    pipeline.prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
}

void VoiceOverDenoiserAudioProcessor::releaseResources() {}

bool VoiceOverDenoiserAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (input == juce::AudioChannelSet::mono() || input == juce::AudioChannelSet::stereo());
}

void VoiceOverDenoiserAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto dry = buffer;

    pipeline.setNoiseReductionAmount(apvts.getRawParameterValue("noiseReduction")->load());
    pipeline.setDereverbAmount(apvts.getRawParameterValue("dereverbReduction")->load());
    pipeline.setFineTune(apvts.getRawParameterValue("fineTune")->load());
    pipeline.setNoiseProfileBias(apvts.getRawParameterValue("noiseProfileBias")->load());
    pipeline.setVoicePreservation(apvts.getRawParameterValue("voicePreservation")->load());
    pipeline.setAttackRelease(apvts.getRawParameterValue("attackMs")->load(),
                              apvts.getRawParameterValue("releaseMs")->load());
    pipeline.setLatencyMode(static_cast<int>(apvts.getRawParameterValue("latencyMode")->load()));

    pipeline.process(buffer);

    if (apvts.getRawParameterValue("abCompare")->load() > 0.5f)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.copyFrom(ch, 0, dry, ch, 0, buffer.getNumSamples());
    }
}

juce::AudioProcessorEditor* VoiceOverDenoiserAudioProcessor::createEditor()
{
    return new VoiceOverDenoiserAudioProcessorEditor(*this);
}

void VoiceOverDenoiserAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void VoiceOverDenoiserAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout VoiceOverDenoiserAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("noiseReduction", "Noise Reduction", 0.0f, 1.0f, 0.6f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("dereverbReduction", "Reverb Reduction", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fineTune", "Fine Tune", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("noiseProfileBias", "Noise Profile Bias", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("voicePreservation", "Voice Preservation", 0.0f, 1.0f, 0.85f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attackMs", "Attack", 1.0f, 250.0f, 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("releaseMs", "Release", 10.0f, 1000.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("latencyMode", "Latency Mode",
        juce::StringArray{ "Low", "Balanced", "High Quality" }, 1));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("abCompare", "A/B Compare",
        juce::StringArray{ "Processed", "Dry Reference" }, 0));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoiceOverDenoiserAudioProcessor();
}

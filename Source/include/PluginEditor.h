#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class VoiceOverDenoiserAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit VoiceOverDenoiserAudioProcessorEditor(VoiceOverDenoiserAudioProcessor& p);
    ~VoiceOverDenoiserAudioProcessorEditor() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void configureSlider(juce::Slider& slider, const juce::String& labelText);

    VoiceOverDenoiserAudioProcessor& processor;

    juce::Slider noiseReductionSlider;
    juce::Slider dereverbSlider;
    juce::Slider fineTuneSlider;
    juce::Slider noiseProfileBiasSlider;
    juce::Slider voicePreservationSlider;
    juce::Slider attackMsSlider;
    juce::Slider releaseMsSlider;

    juce::ComboBox latencyModeBox;
    juce::ComboBox abModeBox;

    juce::Label titleLabel;

    std::unique_ptr<SliderAttachment> noiseReductionAttachment;
    std::unique_ptr<SliderAttachment> dereverbAttachment;
    std::unique_ptr<SliderAttachment> fineTuneAttachment;
    std::unique_ptr<SliderAttachment> noiseProfileAttachment;
    std::unique_ptr<SliderAttachment> voicePreserveAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<ComboAttachment> latencyAttachment;
    std::unique_ptr<ComboAttachment> abAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceOverDenoiserAudioProcessorEditor)
};

#include "PluginEditor.h"

VoiceOverDenoiserAudioProcessorEditor::VoiceOverDenoiserAudioProcessorEditor(VoiceOverDenoiserAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    titleLabel.setText("VoiceOver Denoiser", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    configureSlider(noiseReductionSlider, "Noise Reduction");
    configureSlider(dereverbSlider, "Reverb Reduction");
    configureSlider(fineTuneSlider, "Fine Tune");
    configureSlider(noiseProfileBiasSlider, "Noise Profile Bias");
    configureSlider(voicePreservationSlider, "Voice Preservation");
    configureSlider(attackMsSlider, "Attack (ms)");
    configureSlider(releaseMsSlider, "Release (ms)");

    latencyModeBox.addItemList({ "Low", "Balanced", "High Quality" }, 1);
    addAndMakeVisible(latencyModeBox);
    abModeBox.addItemList({ "Processed", "Dry Reference" }, 1);
    addAndMakeVisible(abModeBox);

    noiseReductionAttachment = std::make_unique<SliderAttachment>(processor.apvts, "noiseReduction", noiseReductionSlider);
    dereverbAttachment = std::make_unique<SliderAttachment>(processor.apvts, "dereverbReduction", dereverbSlider);
    fineTuneAttachment = std::make_unique<SliderAttachment>(processor.apvts, "fineTune", fineTuneSlider);
    noiseProfileAttachment = std::make_unique<SliderAttachment>(processor.apvts, "noiseProfileBias", noiseProfileBiasSlider);
    voicePreserveAttachment = std::make_unique<SliderAttachment>(processor.apvts, "voicePreservation", voicePreservationSlider);
    attackAttachment = std::make_unique<SliderAttachment>(processor.apvts, "attackMs", attackMsSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(processor.apvts, "releaseMs", releaseMsSlider);
    latencyAttachment = std::make_unique<ComboAttachment>(processor.apvts, "latencyMode", latencyModeBox);
    abAttachment = std::make_unique<ComboAttachment>(processor.apvts, "abCompare", abModeBox);

    setSize(960, 620);
}

void VoiceOverDenoiserAudioProcessorEditor::configureSlider(juce::Slider& slider, const juce::String& labelText)
{
    slider.setTextValueSuffix(" " + labelText);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 140, 24);
    addAndMakeVisible(slider);
}

void VoiceOverDenoiserAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(18, 20, 28));

    g.setColour(juce::Colour::fromRGB(42, 48, 66));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 12.0f);

    g.setColour(juce::Colours::white.withAlpha(0.16f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 12.0f, 1.2f);

    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.setFont(16.0f);
    g.drawText("Production Voiceover Denoise + Dereverb", 0, 46, getWidth(), 28, juce::Justification::centred);
}

void VoiceOverDenoiserAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(32);
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(12);

    auto row1 = bounds.removeFromTop(230);
    auto row2 = bounds.removeFromTop(230);

    const int knobWidth = 130;
    const int gap = 14;

    noiseReductionSlider.setBounds(row1.removeFromLeft(knobWidth));
    row1.removeFromLeft(gap);
    dereverbSlider.setBounds(row1.removeFromLeft(knobWidth));
    row1.removeFromLeft(gap);
    fineTuneSlider.setBounds(row1.removeFromLeft(knobWidth));
    row1.removeFromLeft(gap);
    noiseProfileBiasSlider.setBounds(row1.removeFromLeft(knobWidth));

    voicePreservationSlider.setBounds(row2.removeFromLeft(knobWidth));
    row2.removeFromLeft(gap);
    attackMsSlider.setBounds(row2.removeFromLeft(knobWidth));
    row2.removeFromLeft(gap);
    releaseMsSlider.setBounds(row2.removeFromLeft(knobWidth));
    row2.removeFromLeft(gap);
    latencyModeBox.setBounds(row2.removeFromLeft(160).reduced(4, 90));
    row2.removeFromLeft(gap);
    abModeBox.setBounds(row2.removeFromLeft(160).reduced(4, 90));
}

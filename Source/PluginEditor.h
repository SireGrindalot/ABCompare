#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class ABCompareAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ABCompareAudioProcessorEditor (ABCompareAudioProcessor&);
    ~ABCompareAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ABCompareAudioProcessor& audioProcessor;

    juce::ComboBox   mp3Selector;         // Dropdown for available MP3 files
    juce::ToggleButton abButton { "A/B" }; // OFF = Logic, ON = MP3
    juce::Slider     gainSlider;           // Gain for MP3 only
    juce::Label      gainLabel;
    juce::Label      titleLabel;

    void refreshMP3List();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABCompareAudioProcessorEditor)
};

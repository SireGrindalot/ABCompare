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

    // UI components
    juce::Label         abLabel;            // "A / B" header
    juce::ComboBox      refSelector;        // File dropdown
    juce::ToggleButton  abSwitch;           // A/B toggle (custom styled)
    juce::Slider        gainKnob;           // Reference gain knob
    juce::Label         gainLabel;

    // Utility
    void refreshRefList();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABCompareAudioProcessorEditor)
};

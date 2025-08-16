#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// ======= LookAndFeel for flat minimal UI =======
class ABLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ABLookAndFeel()
    {
        setColour(juce::ComboBox::backgroundColourId, juce::Colour::fromRGB(32, 40, 60));
        setColour(juce::ComboBox::outlineColourId, juce::Colour::fromRGB(80, 130, 255));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::skyblue);

        setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(80, 130, 255));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(80, 130, 255));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB(22, 24, 32));

        setColour(juce::Label::textColourId, juce::Colours::white);
    }
    
    // This new function tells JUCE how to draw our buttons.
    // Instead of the default rounded shape, we just draw a sharp rectangle.
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        g.setColour (backgroundColour);
        g.fillRect (button.getLocalBounds());
    }
};

//==============================================================================
class ABCompareAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ABCompareAudioProcessorEditor (ABCompareAudioProcessor&);
    ~ABCompareAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ABLookAndFeel abLnF; // LookAndFeel is a member variable

    ABCompareAudioProcessor& audioProcessor;

    // UI components
    juce::Label         abLabel;
    juce::ComboBox      refSelector;
    juce::TextButton    buttonA;
    juce::TextButton    buttonB;
    juce::Slider        gainKnob;
    juce::Label         gainLabel;

    // Utility
    void refreshRefList();
    void updateButtonStates();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABCompareAudioProcessorEditor)
};

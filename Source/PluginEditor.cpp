#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ABCompareAudioProcessorEditor::ABCompareAudioProcessorEditor (ABCompareAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 300);
    setLookAndFeel (&abLnF);

    // === A/B label ===
    abLabel.setText ("A / B", juce::dontSendNotification);
    abLabel.setJustificationType (juce::Justification::centred);
    juce::Font labelFont { juce::FontOptions { 24.0f } };
    labelFont.setBold (true);
    abLabel.setFont (labelFont);
    addAndMakeVisible (abLabel);

    // === Reference Selector (wide pulldown) ===
    addAndMakeVisible (refSelector);
    refSelector.onChange = [this] {
        int idx = refSelector.getSelectedId() - 1;
        if (idx >= 0)
            audioProcessor.setSelectedRefIndex (idx);
    };

    // === A/B Buttons ===
    buttonA.setButtonText ("A");
    addAndMakeVisible (buttonA);
    buttonA.onClick = [this]
    {
        audioProcessor.setUseReference (false);
        updateButtonStates();
    };

    buttonB.setButtonText ("B");
    addAndMakeVisible (buttonB);
    buttonB.onClick = [this]
    {
        audioProcessor.setUseReference (true);
        updateButtonStates();
    };

    // === Gain Knob ===
    gainKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    gainKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    gainKnob.setRange (-12.0, 0.0, 0.01);
    gainKnob.setValue (0.0);
    addAndMakeVisible (gainKnob);
    gainKnob.onValueChange = [this] {
        audioProcessor.setReferenceGainDb ((float) gainKnob.getValue());
    };

    // === Gain Label ===
    gainLabel.setText ("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType (juce::Justification::centred);
    gainLabel.setFont (juce::Font { juce::FontOptions { 14.0f } });
    addAndMakeVisible (gainLabel);

    refreshRefList();
    updateButtonStates();
}

ABCompareAudioProcessorEditor::~ABCompareAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================

void ABCompareAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Blue-grey flat background
    g.fillAll (juce::Colour::fromRGB (24, 29, 44));
}

void ABCompareAudioProcessorEditor::resized()
{
    const int margin = 20;
    auto area = getLocalBounds().reduced (margin);

    // Header
    abLabel.setBounds (area.removeFromTop (40));

    // File selector (wide)
    refSelector.setBounds (area.removeFromTop (40).reduced (0, 8).withWidth (getWidth() - margin * 2));

    // A/B buttons area - centered
    auto buttonArea = area.removeFromTop(48).withSizeKeepingCentre(180, 38);
    buttonA.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(2));
    buttonB.setBounds(buttonArea.reduced(2));

    // Gain knob and label (centered, below switch)
    auto knobArea = area.removeFromTop (100).withSizeKeepingCentre (72, 72);
    gainKnob.setBounds (knobArea);

    gainLabel.setBounds (knobArea.withY (knobArea.getBottom() + 4).withHeight (18));
}

void ABCompareAudioProcessorEditor::refreshRefList()
{
    refSelector.clear();
    const auto names = audioProcessor.getAvailableRefs();
    for (int i = 0; i < names.size(); ++i)
        refSelector.addItem (names[i], i + 1);
    if (names.size() > 0)
        refSelector.setSelectedId (1);
}

void ABCompareAudioProcessorEditor::updateButtonStates()
{
    const bool isB = audioProcessor.getUseReference();
    
    auto activeColour   = juce::Colour::fromRGB (80, 130, 255);
    auto inactiveColour = juce::Colour::fromRGB (44, 54, 84);

    buttonA.setColour (juce::TextButton::buttonColourId, isB ? inactiveColour : activeColour);
    buttonB.setColour (juce::TextButton::buttonColourId, isB ? activeColour : inactiveColour);
}

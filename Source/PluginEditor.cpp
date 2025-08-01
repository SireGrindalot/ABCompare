/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ABCompareAudioProcessorEditor::ABCompareAudioProcessorEditor (ABCompareAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 200);

    // --- Title ---
    addAndMakeVisible (titleLabel);
    titleLabel.setText ("ABCompare Plugin", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    {
        // Avoid deprecated Font constructors/setters:
        // take the label's current font and transform it using non-deprecated helpers
        auto f = titleLabel.getFont().withHeight (18.0f).boldened();
        titleLabel.setFont (f);
    }

    // --- MP3 selector ---
    addAndMakeVisible (mp3Selector);
    mp3Selector.onChange = [this]
    {
        const int idx = mp3Selector.getSelectedId() - 1; // ComboBox IDs are 1-based
        if (idx >= 0)
            audioProcessor.setSelectedMP3Index (idx);
    };

    // --- A/B toggle: OFF = Logic audio; ON = MP3 reference ---
    addAndMakeVisible (abButton);
    abButton.onClick = [this]
    {
        audioProcessor.setUseReference (abButton.getToggleState());
    };

    // Populate list and auto-select first (triggers onChange)
    refreshMP3List();
}

ABCompareAudioProcessorEditor::~ABCompareAudioProcessorEditor() {}

//==============================================================================
void ABCompareAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void ABCompareAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (16);

    // Title at top
    titleLabel.setBounds (area.removeFromTop (28));

    // Row: selector left
    auto row = area.removeFromTop (32);
    mp3Selector.setBounds (row.removeFromLeft (juce::jmax (200, getWidth() / 2)));

    // A/B button centred in remaining area
    abButton.setBounds (area.withSizeKeepingCentre (100, 28));
}

void ABCompareAudioProcessorEditor::refreshMP3List()
{
    mp3Selector.clear();

    const auto names = audioProcessor.getAvailableMP3s();
    for (int i = 0; i < names.size(); ++i)
        mp3Selector.addItem (names[i], i + 1); // IDs are 1-based

    if (names.size() > 0)
        mp3Selector.setSelectedId (1); // triggers onChange -> loads first file
}

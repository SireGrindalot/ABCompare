#include "PluginProcessor.h"
#include "PluginEditor.h"

// ======= LookAndFeel for blue pill toggle and flat minimal UI =======
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

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& btn, bool, bool) override
    {
        auto area = btn.getLocalBounds().toFloat().reduced (2, 4);
        auto isB = btn.getToggleState();

        auto bg = juce::Colour::fromRGB (44, 54, 84);
        auto pill = juce::Colour::fromRGB (80, 130, 255);
        auto text = juce::Colours::white;

        // Draw pill background
        g.setColour (bg);
        g.fillRoundedRectangle (area, area.getHeight() / 2);

        // Draw selection
        auto half = area;
        half.setWidth (area.getWidth() / 2.0f);
        if (isB)
            half.setX (area.getX() + area.getWidth() / 2.0f);
        g.setColour (pill);
        g.fillRoundedRectangle (half, area.getHeight() / 2);

        // Draw "A" and "B"
        juce::Font aFont, bFont;
        aFont = juce::Font (area.getHeight() * 0.5f, juce::Font::plain).boldened();
        bFont = juce::Font (area.getHeight() * 0.5f, juce::Font::plain).boldened();

        // Draw A
        g.setFont (aFont);
        g.setColour (isB ? text : bg.contrasting (1.0f));
        g.drawFittedText ("A", area.getSmallestIntegerContainer().removeFromLeft (area.getWidth() / 2), juce::Justification::centred, 1);

        // Draw B
        g.setFont (bFont);
        g.setColour (isB ? bg.contrasting (1.0f) : text);
        g.drawFittedText ("B", area.getSmallestIntegerContainer().removeFromRight (area.getWidth() / 2), juce::Justification::centred, 1);
    }
};

static ABLookAndFeel abLnF;

//==============================================================================

ABCompareAudioProcessorEditor::ABCompareAudioProcessorEditor (ABCompareAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 300);
    setLookAndFeel (&abLnF);

    // === A/B label ===
    abLabel.setText ("A / B", juce::dontSendNotification);
    abLabel.setJustificationType (juce::Justification::centred);
    abLabel.setFont (juce::Font (24.0f, juce::Font::plain).boldened());
    addAndMakeVisible (abLabel);

    // === Reference Selector (wide pulldown) ===
    refSelector.setLookAndFeel (&abLnF);
    addAndMakeVisible (refSelector);
    refSelector.onChange = [this] {
        int idx = refSelector.getSelectedId() - 1;
        if (idx >= 0)
            audioProcessor.setSelectedRefIndex (idx);
    };

    // === A/B Switch ===
    abSwitch.setLookAndFeel (&abLnF);
    abSwitch.setClickingTogglesState (true);
    abSwitch.setToggleState (false, juce::dontSendNotification);
    addAndMakeVisible (abSwitch);
    abSwitch.onClick = [this] {
        audioProcessor.setUseReference (abSwitch.getToggleState());
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
    gainLabel.setFont (juce::Font (14.0f, juce::Font::plain));
    addAndMakeVisible (gainLabel);

    refreshRefList();
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

    // A/B switch - big pill button
    abSwitch.setBounds (area.removeFromTop (48).withSizeKeepingCentre (180, 38));

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

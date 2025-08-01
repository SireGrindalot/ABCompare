#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
ABCompareAudioProcessor::ABCompareAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                    #if ! JucePlugin_IsMidiEffect
                     #if ! JucePlugin_IsSynth
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     #endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                    #endif
                      )
#endif
{
    formatManager.registerBasicFormats();

    // Use ~/Music/ABCompare for reference MP3s, create if missing
    auto folder = juce::File::getSpecialLocation (juce::File::userMusicDirectory)
                    .getChildFile ("ABCompare");
    if (! folder.exists())
        folder.createDirectory();

    loadMP3FilesFromFolder (folder);
}

ABCompareAudioProcessor::~ABCompareAudioProcessor()
{
    if (transportIsPlaying.load())
        transportSource.stop();
    transportSource.releaseResources();
}

//==============================================================================
const juce::String ABCompareAudioProcessor::getName() const { return JucePlugin_Name; }
bool ABCompareAudioProcessor::acceptsMidi() const { return false; }
bool ABCompareAudioProcessor::producesMidi() const { return false; }
bool ABCompareAudioProcessor::isMidiEffect() const { return false; }
double ABCompareAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int ABCompareAudioProcessor::getNumPrograms() { return 1; }
int ABCompareAudioProcessor::getCurrentProgram() { return 0; }
void ABCompareAudioProcessor::setCurrentProgram (int) {}
const juce::String ABCompareAudioProcessor::getProgramName (int) { return {}; }
void ABCompareAudioProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================
void ABCompareAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    transportSource.prepareToPlay (samplesPerBlock, sampleRate);
}

void ABCompareAudioProcessor::releaseResources()
{
    if (transportIsPlaying.load())
        transportSource.stop();
    transportSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ABCompareAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    auto mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (mainOut != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ABCompareAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear any extra outputs if needed (keep existing input audio intact)
    const auto totalNumIn  = getTotalNumInputChannels();
    const auto totalNumOut = getTotalNumOutputChannels();
    for (int ch = totalNumIn; ch < totalNumOut; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    const bool refMode = getUseReference();

    // Determine desired transport state from host transport (modern API only)
    bool desiredPlay = false;
    if (refMode)
    {
        if (auto* playHead = getPlayHead())
        {
            if (auto pos = playHead->getPosition())
                desiredPlay = pos->getIsPlaying();
        }
    }

    // If desired state changed, request async transition on message thread
    if (desiredPlay != wantTransportPlaying.load())
    {
        wantTransportPlaying.store (desiredPlay);
        triggerAsyncUpdate(); // handleAsyncUpdate() runs off the audio thread
    }

    if (refMode && readerSource != nullptr)
    {
        // Replace buffer with reference audio
        buffer.clear();
        juce::AudioSourceChannelInfo block (&buffer, 0, buffer.getNumSamples());
        transportSource.getNextAudioBlock (block);

        // === Apply gain to reference ===
        const float gainDb = referenceGainDb.load();
        const float gainLin = std::pow (10.0f, gainDb / 20.0f);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.applyGain (ch, 0, buffer.getNumSamples(), gainLin);
    }
    else
    {
        // Pass-through: leave host buffer content unchanged
    }
}

//==============================================================================
// Runs on the message thread — safe place to start/stop the transport.
void ABCompareAudioProcessor::handleAsyncUpdate()
{
    const bool shouldBePlaying = wantTransportPlaying.load();

    if (shouldBePlaying && ! transportIsPlaying.load())
    {
        transportSource.start();
        transportIsPlaying.store (true);
    }
    else if (! shouldBePlaying && transportIsPlaying.load())
    {
        transportSource.stop();
        transportIsPlaying.store (false);
    }
}

//==============================================================================
bool ABCompareAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ABCompareAudioProcessor::createEditor()
{
    return new ABCompareAudioProcessorEditor (*this);
}

//==============================================================================
void ABCompareAudioProcessor::getStateInformation (juce::MemoryBlock&) {}
void ABCompareAudioProcessor::setStateInformation (const void*, int) {}

//=== MP3 handling ==============================================================
void ABCompareAudioProcessor::loadMP3FilesFromFolder (const juce::File& folder)
{
    mp3Files.clear();
    if (! folder.isDirectory()) return;

    juce::Array<juce::File> found;
    folder.findChildFiles (found, juce::File::findFiles, false, "*.mp3");

    for (auto& f : found)
        mp3Files.add (f);
}

juce::StringArray ABCompareAudioProcessor::getAvailableMP3s() const
{
    juce::StringArray names;
    for (auto& f : mp3Files) names.add (f.getFileName());
    return names;
}

void ABCompareAudioProcessor::setSelectedMP3Index (int index)
{
    if (index >= 0 && index < mp3Files.size())
        playSelectedMP3 (index);
}

void ABCompareAudioProcessor::playSelectedMP3 (int index)
{
    if (index < 0 || index >= mp3Files.size()) return;

    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (mp3Files[index]));
    if (reader == nullptr) return;

    std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader.release(), true));
    transportSource.setSource (newSource.get(), 0, nullptr, newSource->getAudioFormatReader()->sampleRate);
    readerSource.reset (newSource.release());
    currentFileIndex = index;
    transportSource.setPosition (0.0);
    // Start/stop handled via AsyncUpdater based on host transport when refMode is ON.
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ABCompareAudioProcessor();
}

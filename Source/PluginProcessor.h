#pragma once

#include <JuceHeader.h>

//==============================================================================
class ABCompareAudioProcessor  : public juce::AudioProcessor,
                                 private juce::AsyncUpdater
{
public:
    ABCompareAudioProcessor();
    ~ABCompareAudioProcessor() override;

    // AudioProcessor
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // Info
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    // Programs
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // State
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Reference File API for editor
    juce::StringArray getAvailableRefs() const;
    void setSelectedRefIndex (int index);
    void rescanReferenceFolder();

    // A/B switch
    void setUseReference (bool shouldUse) { useReference.store (shouldUse); }
    bool getUseReference () const         { return useReference.load(); }

    // Gain
    void setReferenceGainDb (float db)    { referenceGainDb.store (db); }
    float getReferenceGainDb () const     { return referenceGainDb.load(); }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::Array<juce::File> refFiles;
    int currentRefIndex = -1;

    std::atomic<bool> useReference { false };
    std::atomic<float> referenceGainDb { 0.0f };

    std::atomic<bool> wantTransportPlaying { false };
    std::atomic<bool> transportIsPlaying   { false };

    void handleAsyncUpdate() override;
    void loadRefFilesFromFolder();
    void playSelectedRef (int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABCompareAudioProcessor)
};

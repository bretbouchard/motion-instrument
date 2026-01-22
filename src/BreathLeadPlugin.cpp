/*
 * BreathLeadPlugin.cpp
 *
 * JUCE AudioProcessor wrapper implementation
 *
 * Created: January 19, 2026
 */

#include "BreathLeadPlugin.h"

//==============================================================================
BreathLeadPlugin::BreathLeadPlugin()
    : juce::AudioProcessor(BusesProperties()
                                .withOutput("Output", juce::AudioChannelSet::stereo()))
{
    // Create parameter layout
    auto layout = makeBreathLeadParameterLayout();

    // Initialize AudioProcessorValueTreeState
    parameters_ = std::make_unique<juce::AudioProcessorValueTreeState>(
        *this, nullptr, "BreathLeadParameters", std::move(layout)
    );

    // Create synth
    synth_ = std::make_unique<BreathLeadSynth>(*parameters_);
}

BreathLeadPlugin::~BreathLeadPlugin()
{
    // Cleanup handled by unique_ptr
}

//==============================================================================
// AudioProcessor Interface
//==============================================================================

void BreathLeadPlugin::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare synth
    synth_->prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void BreathLeadPlugin::releaseResources()
{
    synth_->reset();
}

void BreathLeadPlugin::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    // Render synth with MIDI
    synth_->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void BreathLeadPlugin::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output
    buffer.clear();
}

juce::AudioProcessorEditor* BreathLeadPlugin::createEditor()
{
    // For now, return generic editor (can be replaced with custom UI later)
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
// Plugin State Management
//==============================================================================

void BreathLeadPlugin::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, false);
    parameters_->state.writeToStream(stream);
}

void BreathLeadPlugin::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<std::size_t>(sizeInBytes), false);
    auto newTree = juce::ValueTree::readFromStream(stream);
    if (newTree.isValid())
        parameters_->state = newTree;
}

//==============================================================================
// Channel Names
//==============================================================================

const juce::String BreathLeadPlugin::getInputChannelName(int channelIndex) const
{
    return "Input " + juce::String(channelIndex + 1);
}

const juce::String BreathLeadPlugin::getOutputChannelName(int channelIndex) const
{
    return "Output " + juce::String(channelIndex + 1);
}

bool BreathLeadPlugin::isInputChannelStereoPair(int index) const
{
    return getTotalNumInputChannels() == 2 && index < 2;
}

bool BreathLeadPlugin::isOutputChannelStereoPair(int index) const
{
    return getTotalNumOutputChannels() == 2 && index < 2;
}

//==============================================================================
// Create Plugin Instance
//==============================================================================

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BreathLeadPlugin();
}

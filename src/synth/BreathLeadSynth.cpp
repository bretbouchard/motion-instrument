/*
 * BreathLeadSynth.cpp
 *
 * JUCE Synthesiser implementation
 *
 * Created: January 19, 2026
 */

#include "synth/BreathLeadSynth.h"

// Simple sound that applies to all notes/channels
struct BreathLeadSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

BreathLeadSynth::BreathLeadSynth (juce::AudioProcessorValueTreeState& apvtsRef)
: apvts(apvtsRef)
{
    // Mono with optional unison (set to 1 voice for now)
    for (int i = 0; i < 1; ++i)
        addVoice (new BreathLeadVoice (apvts));

    addSound (new BreathLeadSound());
}

void BreathLeadSynth::prepare (double sampleRate, int samplesPerBlock, int numChannels)
{
    setCurrentPlaybackSampleRate (sampleRate);

    // Voice preparation happens in setCurrentPlaybackSampleRate
    juce::ignoreUnused(samplesPerBlock);
    juce::ignoreUnused(numChannels);
}

void BreathLeadSynth::reset()
{
    // Clear all voices
    for (auto* voice : voices)
        voice->stopNote(0.0f, false);
}

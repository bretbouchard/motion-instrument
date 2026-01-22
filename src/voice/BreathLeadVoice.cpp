/*
 * BreathLeadVoice.cpp
 *
 * JUCE SynthesiserVoice implementation
 *
 * Created: January 19, 2026
 */

#include "voice/BreathLeadVoice.h"
#include <cmath>
#include <algorithm>

static inline float midiToHz (int midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
}

float BreathLeadVoice::coeffFromMs (float ms) const
{
    const float tau = std::max(0.0001f, ms / 1000.0f);
    return std::exp(-1.0f / (tau * (float) sr));
}

BreathLeadVoice::BreathLeadVoice (juce::AudioProcessorValueTreeState& apvtsRef)
: apvts(apvtsRef)
{
}

bool BreathLeadVoice::canPlaySound (juce::SynthesiserSound* s)
{
    return (dynamic_cast<juce::SynthesiserSound*>(s) != nullptr);
}

void BreathLeadVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sr = newRate;
    // prepare with modest block assumption; synth will re-prepare if needed
    dsp.prepare(sr, 512, 2);
    glideCoeff = coeffFromMs(std::max(1.0f, portamentoMs));
}

void BreathLeadVoice::startNote (int midiNoteNumber, float vel, juce::SynthesiserSound*, int)
{
    targetHz = midiToHz(midiNoteNumber);
    if (! isVoiceActive())
        currentHz = targetHz;

    dsp.setGate(true);
    dsp.setVelocity(std::clamp(vel, 0.0f, 1.0f));
}

void BreathLeadVoice::stopNote (float, bool allowTailOff)
{
    dsp.setGate(false);
    if (!allowTailOff)
        clearCurrentNote();
}

void BreathLeadVoice::pitchWheelMoved (int newPitchWheelValue)
{
    // 0..16383, center 8192
    const float norm = (newPitchWheelValue - 8192) / 8192.0f;
    pitchBendNorm = std::clamp(norm, -1.0f, 1.0f);
    dsp.setPitchBendNorm(pitchBendNorm);
}

void BreathLeadVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    if (controllerNumber == 1) // modwheel
    {
        modWheel01 = std::clamp(newControllerValue / 127.0f, 0.0f, 1.0f);
        dsp.setModWheel(modWheel01);
    }
}

void BreathLeadVoice::aftertouchChanged (int newAftertouchValue)
{
    aftertouch01 = std::clamp(newAftertouchValue / 127.0f, 0.0f, 1.0f);
    dsp.setAftertouch(aftertouch01);
}

void BreathLeadVoice::updateParamsFromAPVTS()
{
    auto getF = [&](const char* id){ return apvts.getRawParameterValue(id)->load(); };
    auto getB = [&](const char* id){ return apvts.getRawParameterValue(id)->load() > 0.5f; };

    dsp.setParams(
        getF(BreathLeadParamIDs::air),
        getF(BreathLeadParamIDs::tone),
        getF(BreathLeadParamIDs::formant),
        getF(BreathLeadParamIDs::resistance),
        getF(BreathLeadParamIDs::vibratoDepth),
        getF(BreathLeadParamIDs::vibratoRateHz),
        getF(BreathLeadParamIDs::noiseColor),
        getF(BreathLeadParamIDs::sineAnchor),
        getB(BreathLeadParamIDs::motionSustain),
        getF(BreathLeadParamIDs::motionSensitivity),
        getF(BreathLeadParamIDs::attackMs),
        getF(BreathLeadParamIDs::releaseMs),
        getF(BreathLeadParamIDs::outputGainDb)
    );
}

void BreathLeadVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    updateParamsFromAPVTS();

    // glide toward target
    glideCoeff = coeffFromMs(std::max(1.0f, portamentoMs));

    for (int i = 0; i < numSamples; ++i)
    {
        currentHz = targetHz + glideCoeff * (currentHz - targetHz);
        dsp.setPitchHz(currentHz);

        // render one sample at a time (simple + stable; optimize to blocks later)
        juce::AudioBuffer<float> temp (outputBuffer.getArrayOfWritePointers(), outputBuffer.getNumChannels(), startSample + i, 1);
        dsp.render(temp, 0, 1);
    }
}

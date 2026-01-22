/*
 * BreathLeadDSP.cpp
 *
 * Physical modeling breath synthesizer implementation
 *
 * Created: January 19, 2026
 */

#include "dsp/BreathLeadDSP.h"
#include <algorithm>
#include <cmath>

static inline float dbToLin(float db) { return std::pow(10.0f, db / 20.0f); }

float BreathLeadDSP::coeffFromMs (float ms) const
{
    const float tau = std::max(0.0001f, ms / 1000.0f);
    return std::exp(-1.0f / (tau * (float) sr));
}

float BreathLeadDSP::midiToHzClamp (float hz) const
{
    return std::clamp(hz, 20.0f, 12000.0f);
}

void BreathLeadDSP::prepare (double sampleRate, int samplesPerBlock, int numChannels)
{
    sr = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) std::max(1, numChannels);

    pitchBP.reset(); form1BP.reset(); form2BP.reset();
    pitchBP.prepare(spec); form1BP.prepare(spec); form2BP.prepare(spec);

    pitchBP.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    form1BP.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    form2BP.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    hp.reset(); lp.reset();
    hp.prepare(spec); lp.prepare(spec);

    // init with safe coefficients; will be updated by setParams
    hp.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 60.0);
    lp.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, 14000.0);

    noise.reset(0x12345678u);

    meMW.prepare(sr); meAT.prepare(sr); mePB.prepare(sr); mePitch.prepare(sr);
    meMW.reset(); meAT.reset(); mePB.reset(); mePitch.reset();

    airS.reset(sr, 0.02); toneS.reset(sr, 0.02); formantS.reset(sr, 0.02); resistS.reset(sr, 0.02);
    vibrDepthS.reset(sr, 0.05); vibrRateS.reset(sr, 0.05);
    noiseColorS.reset(sr, 0.05); sineAnchorS.reset(sr, 0.05);
    motionSensS.reset(sr, 0.05);
    outGainS.reset(sr, 0.05);

    reset();
}

void BreathLeadDSP::reset()
{
    gate = false;
    phase = 0.0f;
    env = 0.0f;

    pitchBP.reset(); form1BP.reset(); form2BP.reset();
    hp.reset(); lp.reset();

    meMW.reset(); meAT.reset(); mePB.reset(); mePitch.reset();
}

void BreathLeadDSP::setPitchHz (float hz)      { pitchHz = midiToHzClamp(hz); }
void BreathLeadDSP::setGate (bool isOn)        { gate = isOn; }
void BreathLeadDSP::setVelocity (float vel01)  { velocity = std::clamp(vel01, 0.0f, 1.0f); }
void BreathLeadDSP::setModWheel (float mw01)   { modWheel = std::clamp(mw01, 0.0f, 1.0f); }
void BreathLeadDSP::setAftertouch (float at01) { aftertouch = std::clamp(at01, 0.0f, 1.0f); }
void BreathLeadDSP::setPitchBendNorm (float pbNorm) { pitchBend = std::clamp(pbNorm, -1.0f, 1.0f); }

void BreathLeadDSP::setParams (float air, float tone, float formant, float resistance,
                               float vibrDepth, float vibrRateHz,
                               float noiseColor, float sineAnchor,
                               bool motionSustain, float motionSensitivity,
                               float attackMs, float releaseMs,
                               float outputGainDb)
{
    airS.setTargetValue(std::clamp(air, 0.0f, 1.0f));
    toneS.setTargetValue(std::clamp(tone, 0.0f, 1.0f));
    formantS.setTargetValue(std::clamp(formant, 0.0f, 1.0f));
    resistS.setTargetValue(std::clamp(resistance, 0.0f, 1.0f));

    vibrDepthS.setTargetValue(std::clamp(vibrDepth, 0.0f, 1.0f));
    vibrRateS.setTargetValue(std::clamp(vibrRateHz, 0.5f, 8.0f));

    noiseColorS.setTargetValue(std::clamp(noiseColor, 0.0f, 1.0f));
    sineAnchorS.setTargetValue(std::clamp(sineAnchor, 0.0f, 1.0f));

    motionSustainEnabled = motionSustain;
    motionSensS.setTargetValue(std::clamp(motionSensitivity, 0.0f, 1.0f));

    envA = coeffFromMs(std::max(1.0f, attackMs));
    envR = coeffFromMs(std::max(5.0f, releaseMs));

    outGainS.setTargetValue(dbToLin(outputGainDb));
}

void BreathLeadDSP::render (juce::AudioBuffer<float>& out, int startSample, int numSamples)
{
    const int chs = out.getNumChannels();
    auto* ch0 = out.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i)
    {
        // smooth params
        const float air = airS.getNextValue();
        const float tone = toneS.getNextValue();
        const float form = formantS.getNextValue();
        const float resist = resistS.getNextValue();
        const float vibrDepth = vibrDepthS.getNextValue();
        const float vibrRate = vibrRateS.getNextValue();
        const float noiseColor = noiseColorS.getNextValue();
        const float sineAnchor = sineAnchorS.getNextValue();
        const float motionSens = motionSensS.getNextValue();
        const float outGain = outGainS.getNextValue();

        // vibrato (slow; depth small)
        const float vibr = std::sin(phase * 2.0f * juce::MathConstants<float>::pi) * vibrDepth;
        const float vibRateNorm = vibrRate / (float) sr;
        phase += vibRateNorm;
        if (phase >= 1.0f) phase -= 1.0f;

        // pitch with vibrato + pitchbend (±2 semitones typical)
        const float bendSemis = 2.0f * pitchBend;
        const float pitchMul = std::pow(2.0f, (bendSemis + vibr * 0.35f) / 12.0f);
        const float hz = midiToHzClamp(pitchHz * pitchMul);

        // --- Motion energy (optional) ---
        float motionE = 0.0f;
        if (motionSustainEnabled)
        {
            // feed derivatives of expressive controls; summed then softened
            const float eMW = meMW.process(modWheel, motionSens);
            const float eAT = meAT.process(aftertouch, motionSens);
            const float ePB = mePB.process(pitchBend, motionSens);
            const float eP  = mePitch.process(hz / 2000.0f, motionSens); // scaled pitch motion cue
            motionE = std::min(1.0f, (eMW + eAT + ePB + 0.5f * eP));
        }

        // Air envelope: note-on gives initial energy, modWheel sustains
        // Pressure target combines: base air + wheel + motion
        float pressureTarget = 0.0f;
        if (gate)
        {
            // velocity grants immediate "speak"
            const float velSpeak = 0.20f + 0.80f * velocity;
            const float wheelPressure = modWheel;  // CC1 sustains
            const float motionPressure = motionE;  // motion sustains (if enabled)
            pressureTarget = std::clamp(velSpeak * 0.55f + wheelPressure * 0.75f + motionPressure * 0.60f, 0.0f, 1.0f);
        }

        const float coeff = (pressureTarget > env) ? envA : envR;
        env = pressureTarget + coeff * (env - pressureTarget);

        // Excitation signal
        const float w = noise.nextWhite();
        const float p = noise.nextPink();
        const float n = (1.0f - noiseColor) * w + noiseColor * p;

        // tiny sine anchor at pitch (not a "synth osc", just intonation glue)
        static float oscPhase = 0.0f;
        oscPhase += (hz / (float) sr);
        if (oscPhase >= 1.0f) oscPhase -= 1.0f;
        const float sine = std::sin(oscPhase * 2.0f * juce::MathConstants<float>::pi);

        // resistance: higher resistance = tighter / brighter resonance, less raw noise
        const float resistanceMix = std::clamp(resist, 0.0f, 1.0f);
        const float excitation = (n * (1.0f - 0.35f * resistanceMix) + sine * (0.15f * sineAnchor));

        const float drive = air * env; // main energy
        float x = excitation * drive;

        // --- Resonance stage ---
        // Pitch bandpass
        pitchBP.setCutoffFrequency(hz);
        pitchBP.setResonance(0.7f + 0.25f * resistanceMix); // not whistly

        // Formant centers: morph between "A" and "E"-ish regions (rough but musical)
        // Use pitch-relative body so it tracks as you play
        const float f1A = 750.0f,  f2A = 1200.0f;
        const float f1E = 450.0f,  f2E = 2000.0f;
        float f1 = (1.0f - form) * f1A + form * f1E;
        float f2 = (1.0f - form) * f2A + form * f2E;

        // subtle tracking: higher notes lift formants a bit
        const float track = std::clamp(std::log2(hz / 220.0f) * 0.08f, -0.12f, 0.18f);
        f1 *= std::pow(2.0f, track);
        f2 *= std::pow(2.0f, track);

        form1BP.setCutoffFrequency(std::clamp(f1, 120.0f, 6000.0f));
        form2BP.setCutoffFrequency(std::clamp(f2, 200.0f, 8000.0f));

        form1BP.setResonance(0.55f + 0.25f * resistanceMix);
        form2BP.setResonance(0.45f + 0.20f * resistanceMix);

        // process sample through filters (channel 0 for mono)
        auto yPitch = pitchBP.processSample(0, x);
        auto yF1    = form1BP.processSample(0, x);
        auto yF2    = form2BP.processSample(0, x);

        // mix: pitch core + formant body
        float y = 0.70f * yPitch + 0.40f * yF1 + 0.30f * yF2;

        // --- Tone tilt ---
        // tone=0 dark, tone=1 bright
        const float hpHz = 40.0f + (1.0f - tone) * 120.0f;     // darker = more low cleanup
        const float lpHz = 4500.0f + tone * 11500.0f;          // brighter = higher LP

        hp.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, hpHz);
        lp.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, lpHz);

        y = hp.processSample(y);
        y = lp.processSample(y);

        // gentle saturation for "human warmth"
        y = SoftLimiter::process(y * (1.2f + 0.9f * resistanceMix));

        // output gain + safety soft clip
        y *= outGain;
        y = std::clamp(y, -1.0f, 1.0f);

        // write to all channels mono (or widen later)
        ch0[startSample + i] += y;
        for (int c = 1; c < chs; ++c)
            out.getWritePointer(c)[startSample + i] += y;
    }
}

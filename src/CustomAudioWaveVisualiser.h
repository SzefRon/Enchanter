#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/gui/juce_AudioVisualiserComponent.h>

class CustomAudioWaveVisualiser : public juce::AudioVisualiserComponent
{
public:
    CustomAudioWaveVisualiser(int initialNumChannels);
    ~CustomAudioWaveVisualiser();

    void paintChannel(juce::Graphics &g, juce::Rectangle<float> area,
        const juce::Range<float> *levels, int numLevels, int nextSample) override;

    bool flipRender = false;

private:
};


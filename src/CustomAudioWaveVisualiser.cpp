#include "CustomAudioWaveVisualiser.h"

CustomAudioWaveVisualiser::CustomAudioWaveVisualiser(int initialNumChannels)
	: juce::AudioVisualiserComponent(initialNumChannels) {}

CustomAudioWaveVisualiser::~CustomAudioWaveVisualiser()
	{}

void CustomAudioWaveVisualiser::paintChannel(juce::Graphics &g, juce::Rectangle<float> area, const juce::Range<float>*levels, int numLevels, int nextSample)
{
    juce::Path p;
    getChannelAsPath(p, levels, numLevels, nextSample);

    if (!flipRender) {
        g.strokePath(p, juce::PathStrokeType(2.0f),
            juce::AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(),
                0.0f, 1.0f, area.getX(), area.getBottom(),
                (float)numLevels, -1.0f, area.getRight(), area.getY()));
    }
    else {
        g.strokePath(p, juce::PathStrokeType(2.0f),
            juce::AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getRight(), area.getY(),
                0.0f, 1.0f, area.getRight(), area.getBottom(),
                (float)numLevels, -1.0f, area.getX(), area.getY()));
    }
}

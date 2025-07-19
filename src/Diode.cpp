#include "Diode.h"

Diode::Diode()
{
}

Diode::~Diode()
{
}

void Diode::paint(juce::Graphics &g)
{
    auto boundsScaled = getLocalBounds().toFloat().expanded(-0.5f * CustomStyle::outlineThickness);
    boundsScaled.expand(-5.0f, -5.0f);
    auto width = boundsScaled.getWidth();

    if (isOn)
        g.setColour(juce::Colour(0, 200, 0));
    else
        g.setColour(juce::Colour(200, 0, 0));
    g.fillEllipse(boundsScaled);

    g.setColour(juce::Colours::white);
    g.setOpacity(0.2f);
    g.fillEllipse(boundsScaled.expanded(-0.2f * width));
    
    g.setColour(juce::Colours::black);
    g.drawEllipse(boundsScaled, CustomStyle::outlineThickness);
}

void Diode::setState(bool on)
{
    if (on != isOn)
    {
        isOn = on;
        repaint();
    }
}
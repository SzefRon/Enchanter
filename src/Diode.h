#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "CustomStyle.h"

class Diode : public juce::Component
{
public:
    Diode();
    ~Diode();

    void paint(juce::Graphics& g) override;
    void setState(bool on);

private:
    bool isOn = false;
};


#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "BinaryData.h"

class CustomStyle : public juce::LookAndFeel_V4
{
public:

    static const juce::Font customFont;
    static const juce::Font customNumberFont;

    static const juce::Colour lightMagenta;
    static const juce::Colour magenta;
    static const juce::Colour magentaSel;
    static const juce::Colour magentaClick;
    static const juce::Colour darkMagenta;
    static const juce::Colour lightAzure;

    static void drawTextWithOutline(juce::Graphics &g, juce::Rectangle<float> b,
        const juce::Font &f, const juce::String &text, const juce::Justification &j,
        juce::Colour outlineCol, juce::Colour fillCol, bool cutEdges = true, float thickness = 6.0f);

    static void fillEllipseCentred(juce::Graphics &g, float cX, float cY, float rX, float rY);

    CustomStyle();
    ~CustomStyle();

    static const inline float roundedCoeff = 0.0f;
    static const inline float outlineThickness = 4.0f;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& tb,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Label* createComboBoxTextBox(juce::ComboBox&) override;
    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox&) override;
    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox&, juce::Label&) override;
    void positionComboBoxText(juce::ComboBox&, juce::Label&) override;

    void drawPopupMenuBackground(juce::Graphics&, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics &, const juce::Rectangle<int> &area,
        bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
        const juce::String &text, const juce::String &shortcutKeyText,
        const juce::Drawable *icon, const juce::Colour *textColour) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;

    void drawRotarySlider(juce::Graphics &, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider &) override;


private:
};

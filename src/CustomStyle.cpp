#include "CustomStyle.h"

const juce::Font CustomStyle::customFont { juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::PoppinsBlack_ttf, BinaryData::PoppinsBlack_ttfSize)) };
const juce::Font CustomStyle::customNumberFont { juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::PoppinsBold_ttf, BinaryData::PoppinsBold_ttfSize)) };

const juce::Colour CustomStyle::lightMagenta(228, 140, 255);
const juce::Colour CustomStyle::magenta(163, 10, 214);
const juce::Colour CustomStyle::magentaSel(194, 12, 255);
const juce::Colour CustomStyle::magentaClick(180, 12, 242);
const juce::Colour CustomStyle::darkMagenta(125, 8, 160);
const juce::Colour CustomStyle::lightAzure(61, 177, 255);

void CustomStyle::drawTextWithOutline(juce::Graphics &g, juce::Rectangle<float> b, const juce::Font &f, const juce::String &text, const juce::Justification &j, juce::Colour outlineCol, juce::Colour fillCol, bool cutEdges, float thickness)
{
    float thicknessNorm = (f.getHeight() / 25.0f) * thickness;
    if (cutEdges)
        b.expand(-0.5f * thicknessNorm, -0.5f * thicknessNorm);

    juce::GlyphArrangement ga;
    ga.addFittedText(f, text, b.getX(), b.getY(), b.getWidth(), b.getHeight(), j, 1);
    juce::Path p;
    ga.createPath(p);

    g.setColour(outlineCol);
    g.strokePath(p, juce::PathStrokeType(thicknessNorm,
        juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded));
    g.setColour(fillCol);
    ga.draw(g);
}

void CustomStyle::fillEllipseCentred(juce::Graphics &g, float cX, float cY, float rX, float rY)
{
    auto rect = juce::Rectangle<float>(
        cX - rX, cY - rY, 2.0f * rX, 2.0f * rY
    );
    g.fillEllipse(rect);
}

CustomStyle::CustomStyle()
    : juce::LookAndFeel_V4()
{
    setDefaultSansSerifTypeface(customFont.getTypefacePtr());
    setColour(0x1000700, juce::Colour(0, 0, 0).withAlpha(0.0f));
}

CustomStyle::~CustomStyle()
{
}

void CustomStyle::drawToggleButton(juce::Graphics &g, juce::ToggleButton &tb, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto boundsScaled = tb.getLocalBounds().toFloat().expanded(-0.5f * outlineThickness);
    float height = boundsScaled.getHeight();

    bool mouseOver = tb.isMouseOver();
    bool clicking = tb.isDown();
    bool tbState = tb.getToggleState();

    if (mouseOver)
        if (clicking)
            g.setColour(magentaClick);
        else
            g.setColour(magentaSel);
    else
        g.setColour(magenta);
    //g.setOpacity(0.6f);
    g.fillRoundedRectangle(boundsScaled, roundedCoeff);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(boundsScaled, roundedCoeff, outlineThickness);

    auto boundsTextTop = boundsScaled;
    boundsTextTop.removeFromBottom(height * 0.4f);

    auto boundsTextBottom = boundsScaled;
    boundsTextBottom.removeFromTop(height * 0.4f);

    if (tbState) {
        g.setColour(juce::Colours::black);
        g.setFont(customFont.withHeight(height * 0.45f));
        g.setOpacity(0.2f);
        g.drawText("IN", boundsTextTop, juce::Justification::centred);
        drawTextWithOutline(g, boundsTextBottom, customFont.withHeight(height * 0.45f), "OUT",
            juce::Justification::centred, juce::Colours::black, lightAzure, false);
    }
    else {
        drawTextWithOutline(g, boundsTextTop, customFont.withHeight(height * 0.45f), "IN",
            juce::Justification::centred, juce::Colours::black, lightAzure, false);
        g.setColour(juce::Colours::black);
        g.setFont(customFont.withHeight(height * 0.45f));
        g.setOpacity(0.2f);
        g.drawText("OUT", boundsTextBottom, juce::Justification::centred);
    }
}

void CustomStyle::drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto boundsScaled = b.getLocalBounds().toFloat().expanded(-0.5f * outlineThickness);

    bool mouseOver = b.isMouseOver();
    bool clicking = b.isDown();

    if (mouseOver)
        if (clicking)
            g.setColour(magentaClick);
        else
            g.setColour(magentaSel);
    else
        g.setColour(magenta);
    //g.setOpacity(0.6f);
    g.fillRoundedRectangle(boundsScaled, roundedCoeff);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(boundsScaled, roundedCoeff, outlineThickness);
}

void CustomStyle::drawButtonText(juce::Graphics &g, juce::TextButton &b, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto boundsScaled = b.getLocalBounds().toFloat().expanded(-0.5f * outlineThickness);
    float height = boundsScaled.getHeight();
    auto text = b.getButtonText();

    g.setFont(customFont.withHeight(height * 0.8f));
    drawTextWithOutline(g, boundsScaled, customFont.withHeight(height * 0.7f),
        text, juce::Justification::centred, juce::Colours::black, lightMagenta);
    //g.drawText(text, boundsScaled, juce::Justification::centred, false);
}

juce::Label *CustomStyle::createComboBoxTextBox(juce::ComboBox &cb)
{
    juce::Label *label = new juce::Label(juce::String(), juce::String());
    label->setInterceptsMouseClicks (false, false);

    label->setFont(customFont.withHeight(cb.getHeight() * 0.5f - outlineThickness));

    return label;
}

void CustomStyle::drawComboBox(juce::Graphics &g, int width, int, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &cb)
{

    auto boundsScaled = cb.getLocalBounds().toFloat().expanded(-0.5f * outlineThickness);
    float height = boundsScaled.getHeight();

    bool mouseOver = cb.isMouseOver();
    bool clicking = cb.isMouseButtonDown();
    bool open = cb.isPopupActive();

    auto boundsField = boundsScaled;
    auto boundsButton = boundsField.removeFromRight(height);

    boundsField.expand(0.0f, -0.2f * height);

    // field

    g.setColour(darkMagenta);
    //g.setOpacity(0.6f);
    g.fillRoundedRectangle(boundsField, roundedCoeff);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(boundsField, roundedCoeff, outlineThickness);

    // button

    if (mouseOver)
        if (clicking)
            g.setColour(magentaClick);
        else
            g.setColour(magentaSel);
    else
        g.setColour(magenta);
    //g.setOpacity(0.6f);
    g.fillRoundedRectangle(boundsButton, roundedCoeff);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(boundsButton, roundedCoeff, outlineThickness);

    // arrow

    auto boundsArrow = boundsButton.expanded(-0.25f * boundsButton.getHeight());
    boundsArrow.expand(0.0f, -0.25f * boundsArrow.getHeight());
    
    juce::Path p;

    if (open) {
        p.startNewSubPath(boundsArrow.getBottomLeft());
        p.lineTo(boundsArrow.getCentreX(), boundsArrow.getTopLeft().getY());
        p.lineTo(boundsArrow.getBottomRight());
    }
    else {
        p.startNewSubPath(boundsArrow.getTopLeft());
        p.lineTo(boundsArrow.getCentreX(), boundsArrow.getBottomLeft().getY());
        p.lineTo(boundsArrow.getTopRight());
    }

    g.setColour(juce::Colours::black);
    g.strokePath(p, juce::PathStrokeType(outlineThickness));
}

juce::PopupMenu::Options CustomStyle::getOptionsForComboBoxPopupMenu(juce::ComboBox &box, juce::Label &label)
{
    return juce::PopupMenu::Options().withTargetComponent(&box)
                            .withTargetScreenArea(label.localAreaToGlobal(label.getLocalBounds()))
                            .withItemThatMustBeVisible (box.getSelectedId())
                            .withInitiallySelectedItem (box.getSelectedId())
                            .withMaximumNumColumns (1)
                            .withStandardItemHeight (label.getLocalBounds().getHeight());
}

void CustomStyle::positionComboBoxText(juce::ComboBox &box, juce::Label &label)
{
    auto boundsScaled = box.getLocalBounds().toFloat();
    float height = boundsScaled.getHeight();

    auto boundsField = boundsScaled;
    boundsField.removeFromRight(height);
    boundsField.expand(0.0f, -0.2f * height);
    boundsField.setX(boundsField.getX() + 1);

    boundsField.expand(0.0f, -0.5f * outlineThickness);
    boundsField.expand(-0.1f * boundsField.getWidth(), 0.0f);

    //label.setText("FFT size", juce::dontSendNotification);

    //label.setFont(customNumberFont);

    // boundsField.setWidth(boundsField.getWidth() - 1);
    // boundsField.setY(boundsField.getY() + 1);

    label.setBounds(boundsField.toNearestInt());
}

void CustomStyle::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    g.setColour(magenta.withAlpha(0.6f));
    g.fillRect(0, 0, width, height);

    auto bounds = juce::Rectangle<float>(0, 0, width, height);
    //bounds.expand(-0.5f * outlineThickness, -0.5f * outlineThickness);

    g.setColour(juce::Colours::black);
    g.drawRect(bounds, outlineThickness);
}

void CustomStyle::drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String &text, const juce::String &shortcutKeyText, const juce::Drawable *icon, const juce::Colour *textColour)
{
    if (isHighlighted)
        g.setColour(magentaSel);
    else
        g.setColour(magenta);
    
    auto bounds = area.expanded(-outlineThickness, -0.5f * outlineThickness);

    //g.setOpacity(0.6f);
    g.fillRect(bounds);

    auto height = bounds.getHeight();
    if (isTicked) {
        drawTextWithOutline(g, bounds.toFloat(), customNumberFont.withHeight(height),
            text, juce::Justification::centred, juce::Colours::black, lightMagenta);
    }
    else {
        g.setFont(customNumberFont.withHeight(height));
        g.setColour(lightMagenta);
        g.drawText(text, bounds, juce::Justification::centred);
    }
    
}

void CustomStyle::drawLabel(juce::Graphics &g, juce::Label &l)
{
    auto area = l.getLocalBounds().toFloat();
    auto font = l.getFont();
    auto text = l.getText();

    CustomStyle::drawTextWithOutline(g, area, font, text, 
        l.getJustificationType(), juce::Colours::black, lightMagenta);
}

void CustomStyle::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &s)
{
    auto sliderProgress = sliderPosProportional;
    auto value = s.getValue();
    auto bounds = s.getLocalBounds();

    auto mouseOver = s.isMouseOver();
    auto clicking = s.isMouseButtonDown();

    float centreX = bounds.getCentreX();
    float centreY = bounds.getCentreY();

    float outlineR = 0.5f * std::fminf(bounds.getWidth(), bounds.getHeight());
    float sliderR = outlineR - outlineThickness;
    float insideR = 0.75f * sliderR;
    float insideOutlineR = insideR + outlineThickness;

    float outlineAngle = outlineThickness / outlineR;

    float beginAngle = -0.75f * juce::float_Pi;
    float endAngle = 0.75f * juce::float_Pi;

    // background outline

    juce::Path outlineArc;
    outlineArc.addCentredArc(centreX, centreY,
        outlineR, outlineR, 0.0f,
        beginAngle - outlineAngle,
        endAngle + outlineAngle, true);
    outlineArc.lineTo(bounds.getCentre().toFloat().translated(0.0f, outlineThickness));

    g.setColour(juce::Colours::black);
    g.fillPath(outlineArc);

    fillEllipseCentred(g, centreX, centreY, insideOutlineR, insideOutlineR);

    // slider background

    juce::Path sliderBackgroundArc;
    sliderBackgroundArc.addCentredArc(centreX, centreY,
        sliderR, sliderR, 0.0f, beginAngle, endAngle, true);
    sliderBackgroundArc.lineTo(bounds.getCentre().toFloat());

    g.setColour(darkMagenta);
    g.fillPath(sliderBackgroundArc);

    // slider

    float endProgressAngle = std::lerp(beginAngle, endAngle, sliderProgress);

    juce::Path sliderArc;
    sliderArc.addCentredArc(centreX, centreY,
        sliderR, sliderR, 0.0f, beginAngle, endProgressAngle, true);
    sliderArc.lineTo(bounds.getCentre().toFloat());

    g.setColour(lightMagenta);
    g.fillPath(sliderArc);

    // inside

    if (mouseOver)
        g.setColour(magentaSel);
    else
        g.setColour(magenta);
    if (clicking)
        g.setColour(magentaClick);

    fillEllipseCentred(g, centreX, centreY, insideR, insideR);
}

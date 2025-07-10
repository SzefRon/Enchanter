#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters(*this, nullptr, juce::Identifier("parameters"), 
    {
        std::make_unique<juce::AudioParameterBool>("fftMode",
            "FFT Mode",
            false),
        std::make_unique<juce::AudioParameterBool>("bypassed",
            "Bypassed",
            true),
        std::make_unique<juce::AudioParameterInt>("fftOrder",
            "FFT Order",
            7, 13, 10)
    })
{
    fftMode = dynamic_cast<juce::AudioParameterBool *>(parameters.getParameter("fftMode"));
    bypassed = dynamic_cast<juce::AudioParameterBool *>(parameters.getParameter("bypassed"));
    fftOrder = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("fftOrder"));
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    setLatencySamples(fftProcessor.getSamples());
    changeOrder(fftOrder->get());
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    std::lock_guard<std::mutex> lock(mutex);
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int sampleCount = buffer.getNumSamples();

    auto writePtrs = buffer.getArrayOfWritePointers();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (!fftMode->get()) {
        processBlockIn(writePtrs, sampleCount);
    }
    else {
        processBlockOut(writePtrs, sampleCount);
    }
}

void AudioPluginAudioProcessor::processBlockIn(float *const *&writePtrs, const int &sampleCount)
{
    for (int i = 0; i < sampleCount; i++) {
        float sample;
        int channel = !operatingChannel ? 0 : 1;

        sample = writePtrs[channel][i];

        if (bypassed->get()) {
            if (fabs(sample >= 0.1f)) {
                *bypassed = false;
                for (int j = i; j < sampleCount - 1; j++) {
                    writePtrs[0][j] = 0.0f;
                    writePtrs[1][j] = 0.0f;
                }
                writePtrs[0][sampleCount - 1] = 1.0f;
                writePtrs[1][sampleCount - 1] = 1.0f;
                break;
            }
            continue;
        }

        auto result = fftProcessor.processSampleIn(sample);
        writePtrs[0][i] = result.first;
        writePtrs[1][i] = result.second;
    }
}

void AudioPluginAudioProcessor::processBlockOut(float *const *&writePtrs, const int &sampleCount)
{

    for (int i = 0; i < sampleCount; i++) {
        float sampleL = writePtrs[0][i];
        float sampleR = writePtrs[1][i];

        if (bypassed->get()) {
            if (sampleL == 1.0f && sampleR == 1.0f) {
                *bypassed = false;
                juce::MessageManagerLock mml;
                ((AudioPluginAudioProcessorEditor *)getActiveEditor())->bypassLabel.setText("Active", juce::NotificationType::dontSendNotification);
            }
            continue;
        }

        auto result = fftProcessor.processSampleOut(sampleL, sampleR);
        writePtrs[0][i] = result;
        writePtrs[1][i] = result;
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

void AudioPluginAudioProcessor::changeOrder(const int &order)
{
    fftProcessor.changeOrder(order);
    setLatencySamples(fftProcessor.getSamples());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

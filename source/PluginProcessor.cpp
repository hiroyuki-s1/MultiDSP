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
                 
    ),parameters(*this, nullptr, juce::Identifier("DualDelayVerbParam"),
        {
            std::make_unique<juce::AudioParameterFloat>("top1",  // ID
                                                        "top1",  // name
                                                         0.0f,     // min
                                                         1.0f, // max
                                                         0.5f),// default,

            std::make_unique<juce::AudioParameterFloat>("top2",  // ID
                                                        "top2",  // name
                                                         0.0f,     // min
                                                         1.0f, // max
                                                         0.5f),// default
            std::make_unique<juce::AudioParameterFloat>("top3",  // ID
                                                        "top3",  // name
                                                         0.0f,     // min
                                                         1.0f, // max
                                                         0.5f),// default
            std::make_unique<juce::AudioParameterFloat>("bottom1",  // ID
                                                        "bottom1",  // name
                                                         0.0f,     // min
                                                         1.0f, // max
                                                         0.5f),// default
            std::make_unique<juce::AudioParameterFloat>("bottom2",  // ID
                                                        "bottom2",  // name
                                                         0.0f,     // min
                                                         1.0f, // max
                                                         0.5f),// default
            std::make_unique<juce::AudioParameterFloat>("bottom3",  // ID
                                                        "bottom3",  // name
                                                         0.0f,     // min
                                                         1.0f, // max
                                                         0.5f),// default
        })
{
    pedal_board.emplace_back(PedalBoard());
    onepole.SetFilterMode(daisysp::OnePole::FilterMode::FILTER_MODE_LOW_PASS);
    param_top1 = parameters.getRawParameterValue("top1");
    param_top2 = parameters.getRawParameterValue("top2");
    param_top3 = parameters.getRawParameterValue("top3");
    param_bottom1 = parameters.getRawParameterValue("bottom1");
    param_bottom2 = parameters.getRawParameterValue("bottom2");
    param_bottom3 = parameters.getRawParameterValue("bottom3");
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
	osc.Init(sampleRate);
	noteMidi = 0.0f;

	noteVal = 0.f;
	ratio = 2.f;
	index = 0.1f;
    currentSampleRate = static_cast<uint16_t>(sampleRate);
    LOG_INFO(std::to_string(currentSampleRate));
    //DBG(currentSampleRate);
    pedal_board[CH::A].init(&currentSampleRate);

    //delay.SetDelay(static_cast<size_t>(currentSampleRate / 2));
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
    juce::ignoreUnused (midiMessages);
    //delay.SetDelay(static_cast<size_t>(currentSampleRate / 1 + (100.0 *  e) ));

    const float lowerLimit = -0.01f; // ﾃ､ﾂｸ窶ｹﾃｩ邃｢
    const float upperLimit = 0.025f;  // ﾃ､ﾂｸﾅﾃｩ邃｢
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    float r = std::abs(repeat) > 1.0 ? 0.0 : repeat;
    float m = std::abs(mix) > 1.0 ? 0.0 : mix;
    LOG_INFO(std::to_string(*param_top1));
    pedal_board[CH::A].setParamDelay(static_cast<CHAR>(DELAY_PARAM::FEEDBACK), *param_top1);
    pedal_board[CH::A].setParamDelay(static_cast<CHAR>(DELAY_PARAM::MIX),      *param_top2);
    pedal_board[CH::A].setParamDelay(static_cast<CHAR>(DELAY_PARAM::TIME),     *param_top3);
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // Process loop
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* inputData = buffer.getReadPointer (channel);
        if (channel == 0) {
            for (size_t i = 0; i < buffer.getNumSamples(); ++i)
            {
                pedal_board[CH::A].process(inputData[i], channelData[i]);

                //float dry = inputData[i];
                //float delayed = delay.Read();
                //delayed = onepole.Process(delayed);
                //delay.Write(dry + delayed * r);
                //channelData[i] = dry + delayed * m;

                //virtual void process(const FLOAT & in, FLOAT & out) override {
                //    FLOAT delayed = delay_.Read();
                //    delay_.Write(in + delayed * feedback_);
                //    out = in + delayed * mix_;
                //}


                //float dry = inputData[i];
                //float delayedSample = delay.Read(); // ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽx・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ黷ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽT・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽv・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽﾇゑｿｽ
                //delay.Write(dry); // ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽﾝのサ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽv・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ
                //channelData[i] = delayedSample * 0.5 + dry; // ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽx・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ黷ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽT・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽv・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽo・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽﾍに設抵ｿｽ


                // float output = inputData[i] * 10.0; // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣窶壺凖｣ﾃ｣ﾂｮﾃ｣ﾂｾﾃ｣ﾂｾﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣邃｢ﾃ｣窶壺ｹﾃ･ﾂﾂｴﾃ･E
                // float output = mixedOutput; // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｫﾃ｣窶堋ｪﾃ｣窶堋ｷﾃ｣ﾆ陳ｬﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｮﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣窶壺凖｣ﾆ椎ｸﾃ｣ﾆ槌津｣窶堋ｯﾃ｣窶堋ｹﾃ｣邃｢ﾃ｣窶壺ｹﾃ･ﾂﾂｴﾃ･E

                // ﾃ｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣ﾆ・EEﾂｽEﾂｽﾃ｣ﾆ陳ｳﾃ｣窶堋ｰﾃ･窶ｰﾃｯﾂｿﾂｽEﾃ｣窶堋ｹﾃ｣窶堋ｱﾃ｣ﾆ陳ｼﾃ｣ﾆ陳ｪﾃ｣ﾆ陳ｳﾃ｣窶堋ｰ
                // float scaledOutput = (output - lowerLimit) / (upperLimit - lowerLimit) * 2.0f - 1.0f;

                // tanhﾃｩ窶督｢ﾃｦ窶｢ﾂｰﾃ｣ﾂｫﾃ｣窶塒・・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ窶壺ｹﾃ｣窶堋ｽﾃ｣ﾆ停｢ﾃ｣ﾆ塚・・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ窶堋ｯﾃ｣ﾆ陳ｪﾃ｣ﾆ・EEﾂｽE
                // scaledOutput = std::tanh(scaledOutput);

                // ﾃ｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣ﾆ・EEﾂｽEﾂｽﾃ｣ﾆ陳ｳﾃ｣窶堋ｰﾃ･ﾂｾﾅ津ｯﾂｿﾂｽEﾃ｣窶堋ｹﾃ｣窶堋ｱﾃ｣ﾆ陳ｼﾃ｣ﾆ陳ｪﾃ｣ﾆ陳ｳﾃ｣窶堋ｰ
                // output = (scaledOutput + 1.0f) / 2.0f * (upperLimit - lowerLimit) + lowerLimit;

                // channelData[i] = output * 100.0;
                // float oscOutput = osc.Process();
                // channelData[i] = inputData[i];
                // channelData[i] = inputData[i] + oscOutput; // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｫﾃ｣窶堋ｪﾃ｣窶堋ｷﾃ｣ﾆ陳ｬﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｮﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣窶壺凖｣ﾆ椎ｸﾃ｣ﾆ槌津｣窶堋ｯﾃ｣窶堋ｹ
            }
        }

    }

    // ﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣ﾅ津ｯﾂｿﾂｽEﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣窶塒・・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ窶塲ﾃ･ﾂ､ﾅ｡ﾃ｣窶榲･ﾂﾂｴﾃ･ﾋ・・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽE・ｽ竄ｬﾃ､ﾂｽ邃｢ﾃｯﾂｿﾂｽEﾃ｣ﾂｪﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣窶壺凖｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣窶堋｢
    // for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    // {
    //     buffer.clear(i, 0, buffer.getNumSamples());
    // }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

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
                       )
{
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
    currentSampleRate = sampleRate;
    std::cout << currentSampleRate << std::endl;
    //delay = daisysp::DelayLine<float, 44100 * 2>();
    delay.Init();
    delay.SetDelay((float)currentSampleRate); 
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

    const float lowerLimit = -0.01f; // ﾃ､ﾂｸ窶ｹﾃｩ邃｢
    const float upperLimit = 0.025f;  // ﾃ､ﾂｸﾅﾃｩ邃｢
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // ﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣ﾅ津ｯﾂｿﾂｽEﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣窶塒・・ｽE・ｽE・ｽE・ｽ窶塲ﾃ･ﾂ､ﾅ｡ﾃ｣窶榲･ﾂﾂｴﾃ･ﾋ・・ｽE・ｽE・ｽE・ｽ竄ｬﾃ､ﾂｽ邃｢ﾃｯﾂｿﾂｽEﾃ｣ﾂｪﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣窶壺凖｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣窶堋｢
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // MIDIﾃ･窶｡ﾂｦﾃｧE
    int time;
    juce::MidiMessage m;
    for (juce::MidiBuffer::Iterator i (midiMessages); i.getNextEvent(m, time);)
    {
        if (m.isNoteOn())
        {
            noteMidi = m.getNoteNumber();
        }
        else if (m.isController())
        {
            switch(m.getControllerNumber())
            {
            case 1:
                index = (m.getControllerValue() / 127.f);
                break;
            case 2:
            case 91: // 91 is the first CC knob on the old oxygen8 v2 I have sitting here...
                ratio = 1.f + ((m.getControllerValue() / 127.f) * 11.f);
                break;
            default:
                break;
            }
        }
    }

    float note = daisysp::fclamp(noteMidi + noteVal, 0.0, 127.0);
    osc.SetIndex(index);
    osc.SetRatio(ratio);

    // Process loop
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* inputData = buffer.getReadPointer (channel);

        // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣窶壺凖｣ﾃ｣ﾂｮﾃ｣ﾂｾﾃ｣ﾂｾﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ・EEﾂｽEﾂｽﾃ｣窶堋｡ﾃ｣ﾂｫﾃ｣窶堋ｳﾃ｣ﾆ停敕ｯﾂｿﾂｽEﾃ｣窶氾｣竄ｬﾃ｣窶堋ｪﾃ｣窶堋ｷﾃ｣ﾆ陳ｬﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｮﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣窶壺凖｣ﾆ椎ｸﾃ｣ﾆ槌津｣窶堋ｯﾃ｣窶堋ｹ
        for (size_t i = 0; i < buffer.getNumSamples(); ++i)
        {
            //float dry = inputData[i];
            //float dryed =  delay.Read();
            //delay.Write(dry);
            //float output = dryed;
            //channelData[i] = output;


            float dry = inputData[i];
            float delayedSample = delay.Read(); // 遅延されたサンプルを読む
            delay.Write(dry); // 現在のサンプルを書き込む
            channelData[i] = delayedSample; // 遅延されたサンプルを出力に設定


            // float output = inputData[i] * 10.0; // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣窶壺凖｣ﾃ｣ﾂｮﾃ｣ﾂｾﾃ｣ﾂｾﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣邃｢ﾃ｣窶壺ｹﾃ･ﾂﾂｴﾃ･E
            // float output = mixedOutput; // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｫﾃ｣窶堋ｪﾃ｣窶堋ｷﾃ｣ﾆ陳ｬﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｮﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣窶壺凖｣ﾆ椎ｸﾃ｣ﾆ槌津｣窶堋ｯﾃ｣窶堋ｹﾃ｣邃｢ﾃ｣窶壺ｹﾃ･ﾂﾂｴﾃ･E

            // ﾃ｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣ﾆ・EEﾂｽEﾂｽﾃ｣ﾆ陳ｳﾃ｣窶堋ｰﾃ･窶ｰﾃｯﾂｿﾂｽEﾃ｣窶堋ｹﾃ｣窶堋ｱﾃ｣ﾆ陳ｼﾃ｣ﾆ陳ｪﾃ｣ﾆ陳ｳﾃ｣窶堋ｰ
            // float scaledOutput = (output - lowerLimit) / (upperLimit - lowerLimit) * 2.0f - 1.0f;

            // tanhﾃｩ窶督｢ﾃｦ窶｢ﾂｰﾃ｣ﾂｫﾃ｣窶塒・・ｽE・ｽE・ｽE・ｽ窶壺ｹﾃ｣窶堋ｽﾃ｣ﾆ停｢ﾃ｣ﾆ塚・・ｽE・ｽE・ｽE・ｽ窶堋ｯﾃ｣ﾆ陳ｪﾃ｣ﾆ・EEﾂｽE
            // scaledOutput = std::tanh(scaledOutput);

            // ﾃ｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣ﾆ・EEﾂｽEﾂｽﾃ｣ﾆ陳ｳﾃ｣窶堋ｰﾃ･ﾂｾﾅ津ｯﾂｿﾂｽEﾃ｣窶堋ｹﾃ｣窶堋ｱﾃ｣ﾆ陳ｼﾃ｣ﾆ陳ｪﾃ｣ﾆ陳ｳﾃ｣窶堋ｰ
            // output = (scaledOutput + 1.0f) / 2.0f * (upperLimit - lowerLimit) + lowerLimit;

            // channelData[i] = output * 100.0;
            // float oscOutput = osc.Process();
            // channelData[i] = inputData[i];
            // channelData[i] = inputData[i] + oscOutput; // ﾃ･窶ｦﾂ･ﾃ･ﾅ窶ｺﾃ｣ﾆ停｡ﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｫﾃ｣窶堋ｪﾃ｣窶堋ｷﾃ｣ﾆ陳ｬﾃ｣ﾆ陳ｼﾃ｣窶堋ｿﾃ｣ﾂｮﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣窶壺凖｣ﾆ椎ｸﾃ｣ﾆ槌津｣窶堋ｯﾃ｣窶堋ｹ
        }
    }

    // ﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣ﾅ津ｯﾂｿﾂｽEﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣窶塒・・ｽE・ｽE・ｽE・ｽ窶塲ﾃ･ﾂ､ﾅ｡ﾃ｣窶榲･ﾂﾂｴﾃ･ﾋ・・ｽE・ｽE・ｽE・ｽ竄ｬﾃ､ﾂｽ邃｢ﾃｯﾂｿﾂｽEﾃ｣ﾂｪﾃ･窶｡ﾂｺﾃ･ﾅ窶ｺﾃ｣ﾆ・ﾃ｣ﾆ陳｣ﾃ｣ﾆ・ﾃ｣ﾆ陳ｫﾃ｣窶壺凖｣窶堋ｯﾃ｣ﾆ陳ｪﾃ｣窶堋｢
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
    return new AudioPluginAudioProcessorEditor (*this);
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

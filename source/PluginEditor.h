#pragma once

#include "PluginProcessor.h"
//#include "delay_ui.h"
#include "dual_delay_verb_ui.h"


//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
	                                     private juce::Slider::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~AudioPluginAudioProcessorEditor() override;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    DualDelayVerbUI dual_delay_verb_ui;

private:
	void sliderValueChanged(juce::Slider* slider) override; 
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
	// juce::Slider oscFreq; // [1]
    //delay_ui delayUI; // Add delay_ui component
    void initAttachment();

    juce::AudioProcessorValueTreeState& valueTreeState;//[3-1]APVTSへの参照

    std::vector<std::unique_ptr<SliderAttachment>> sl_attachment_;//[3-3]アタッチメント

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

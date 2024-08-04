#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (308, 568);

	// Setup the Slider object
	// oscFreq.setSliderStyle(juce::Slider::LinearBarVertical);
	// oscFreq.setRange(0.0, 127.0, 0.25);
	// oscFreq.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
	// oscFreq.setPopupDisplayEnabled(true, false, this);
	// oscFreq.setTextValueSuffix(" Frequency");
	// oscFreq.setValue(1.0);

	// Add Slider to the editor
	// addAndMakeVisible(&oscFreq);
	// Add Listener for slider
	// oscFreq.addListener(this);
	addAndMakeVisible(&delayUI);
	// delayUI
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
	delayUI.setLabelText((juce::String)processorRef.currentSampleRate);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	// sets the position and size of the slider with arguments (x, y, width, height)
	// oscFreq.setBounds(40, 30, 20, getHeight() - 60);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
	// processorRef.noteVal = oscFreq.getValue();
}

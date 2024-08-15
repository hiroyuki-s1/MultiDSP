#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState(vts), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setSize(420, 644);

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
	//addAndMakeVisible(&delayUI);
	addAndMakeVisible(&dual_delay_verb_ui);
	initAttachment();
}



AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::initAttachment() {

	auto itm = dual_delay_verb_ui.getChildren();
	//testSliderAttachment.reset(new SliderAttachment(valueTreeState, "param1", testSlider));
	// delayUI
	LOG_INFO("start init"); 
	sl_attachment_.clear();
	sl_attachment_.resize(KNOB_TYPE::SIZE);
	for (auto* child : itm)
	{
		juce::String name = child->getName();
		int idx = -1;

		LOG_INFO("castr");
		auto* sl = dynamic_cast<juce::Slider*>(child);
		LOG_INFO("casted");
		if (name.isEmpty()){
			name = "(unnamed)";
		}

		if (name == NAME_TOP1) {
			idx = KNOB_TYPE::TOP1;
		}
		else if (name == NAME_TOP2) {
			idx = KNOB_TYPE::TOP2;
		}
		else if (name == NAME_TOP3) {
			idx = KNOB_TYPE::TOP3;
		}
		else if (name == NAME_BOTTOM1) {
			idx = KNOB_TYPE::BOTTOM1;
		}
		else if (name == NAME_BOTTOM2) {
			idx = KNOB_TYPE::BOTTOM2;
		}
		else if (name == NAME_BOTTOM3) {
			idx = KNOB_TYPE::BOTTOM3;
		}

		if (idx != -1 && sl != nullptr) {
			LOG_INFO("ss");
			sl_attachment_[idx].reset(new SliderAttachment(valueTreeState, name.toStdString(), *sl));
			LOG_INFO("Child Component Name: " + name);
		}
	}
	LOG_INFO("complete ");
}
//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
	juce::ignoreUnused(processorRef);
	//processorRef.repeat = delayUI.repeat;
	//processorRef.mix = delayUI.mix; delayUI.tone;
	//float t = std::abs(delayUI.tone) > 1.0f ? 0.0f : delayUI.tone;
 //	processorRef.onepole.SetFrequency(0.497f * t);
	//processorRef.delay.SetDelay(static_cast<size_t>(processorRef.currentSampleRate / (1.0 + delayUI.delay_time * 10.0)));
	//delayUI.setLabelText((juce::String)delayUI.tone);

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

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include  "daisysp.h"
#include "pedal_board.h"
#include "common.h"

//==============================================================================
class AudioPluginAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	AudioPluginAudioProcessor();
	~AudioPluginAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	using AudioProcessor::processBlock;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;


	float noteVal, noteMidi;
	float ratio, index;
	uint16_t currentSampleRate = 0;


	void setDelayTime(float val) { delay_time = val; }
	void setMix(float val) { mix = val; }
	void setTone(float val) { tone = val; }
	void setRepeat(float val) { repeat = val; }
	daisysp::DelayLine<float, 88200> delay;
	daisysp::OnePole onepole;
	float delay_time;
	float mix;
	float tone;
	float repeat = 0.0f;
	std::vector<PedalBoard> pedal_board;

private:
	juce::AudioProcessorValueTreeState parameters;
	daisysp::Fm2 osc;
	std::atomic<float>* param_top1 = nullptr;
	std::atomic<float>* param_top2 = nullptr;
	std::atomic<float>* param_top3 = nullptr;
	std::atomic<float>* param_bottom1 = nullptr;
	std::atomic<float>* param_bottom2 = nullptr;
	std::atomic<float>* param_bottom3 = nullptr;
	//daisysp::DelayLine<float, 188200> delay; 
	 //double delay_time;
	 //double mix;
	 //double tone;
	 //double repeat;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};

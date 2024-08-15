/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.6

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DualDelayVerbUI  : public juce::Component,
                         public juce::Slider::Listener,
                         public juce::Button::Listener
{
public:
    //==============================================================================
    DualDelayVerbUI ();
    ~DualDelayVerbUI() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;

    // Binary resources:
    static const char* background2_png;
    static const int background2_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> juce__slider2;
    std::unique_ptr<juce::Slider> juce__slider_top1;
    std::unique_ptr<juce::Slider> juce__slider_top2;
    std::unique_ptr<juce::Slider> juce__slider_top3;
    std::unique_ptr<juce::Slider> juce__slider_bottom1;
    std::unique_ptr<juce::Slider> juce__slider_bottom2;
    std::unique_ptr<juce::Slider> juce__slider_bottom3;
    std::unique_ptr<juce::TextButton> juce__textButton_ab;
    std::unique_ptr<juce::TextButton> juce__textButton_tap;
    juce::Image cachedImage_background2_png_1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualDelayVerbUI)
};

//[EndFile] You can add extra defines here...
//[/EndFile]


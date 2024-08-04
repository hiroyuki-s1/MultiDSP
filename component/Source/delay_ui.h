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
class delay_ui  : public juce::Component,
                  public juce::Slider::Listener,
                  public juce::Button::Listener
{
public:
    //==============================================================================
    delay_ui ();
    ~delay_ui() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void setLabelText(const juce::String& newText)
    {
        if (juce__label_message != nullptr)
        {
            juce__label_message->setText(newText, juce::dontSendNotification);
        }
    }

    // Binary resources:
    static const char* background_png;
    static const int background_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> juce__slider_time;
    std::unique_ptr<juce::Slider> juce__slider_mix;
    std::unique_ptr<juce::Slider> juce__slider_repeat;
    std::unique_ptr<juce::Slider> juce__slider_tone;
    std::unique_ptr<juce::TextButton> juce__textButton_ONOFF;
    std::unique_ptr<juce::Label> juce__label;
    std::unique_ptr<juce::Label> juce__label_message;
    juce::Image cachedImage_background_png_1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (delay_ui)
};

//[EndFile] You can add extra defines here...
//[/EndFile]


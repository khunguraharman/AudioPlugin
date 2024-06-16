/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    peakFreqSliderAttachment(audioProcessor.apvts, "Peak Frequency", peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
    lowCutFreqSliderAttachment(audioProcessor.apvts, "LowCutOff Frequency", lowCutSlopeSlider),
    highCutFreqSliderAttachment(audioProcessor.apvts, "HighCutOff Frequency", highCutFreqSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutFreqSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider)

{    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.    

    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);        
    }
    setSize (600, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Welcome to Harman's Audio Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //bounding box for the component
    auto bounds = getLocalBounds();
    //dedicate some space to a response area
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    //dedicate space to low cut
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    //only 66% of the original bounds remain after executing the above line
    //dedicate space to high cut
    //remember that this function updates bounds, so to allocate 33% of the original space to high cut, you want 50% of 66%
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    ////set your sliders to be in the bounds calculated above
    //lowCutFreqSlider.setBounds(lowCutArea);
    //highCutFreqSlider.setBounds(highCutArea);
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight()*0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight()*0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    //set the freq slider at the top of the centrel column
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    //again you want 50% of 66% 
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    ////for the last 33%
    peakQualitySlider.setBounds(bounds);

}

std::vector<juce::Component*> AudioPluginAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider, &peakGainSlider, &peakQualitySlider, &lowCutFreqSlider, &highCutFreqSlider, &lowCutSlopeSlider, &highCutSlopeSlider
    };
}

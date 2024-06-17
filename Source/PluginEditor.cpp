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
    using namespace juce;

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    g.fillAll(Colours::black);

    auto bounds = getLocalBounds();

    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto w = responseArea.getWidth();

    auto& lowcut = monoChain.get<ChainPositions::Lowcut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags; //magnitudes

    //we will be calculating one magnitude per pixel and then compute the magnitude at that frequency
    mags.resize(w);
    //will use a special helper function to get the frequency that corresponds to a certain pixel

    //mags are expressed in gain units, which are multiplicitive
    for (int i = 0; i < w; i++) 
    {
        double mag = 1.0f;

        //calculate the frequency(x-axis value) that corresponds to the pixel
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if(!monoChain.isBypassed<ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if(!lowcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if(!lowcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        if (!lowcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!lowcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        mags[i] = Decibels::gainToDecibels(mag);
    }

    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();

    std::cout << outputMin << std::endl;
    std::cout << outputMax << std::endl;

    //lambda expression
    auto map = [outputMin, outputMax](double input)
    {
        //the gain ranges from -24 to +24
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); i++)
    {
        //providing x & y coordinates
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    //draw a background border around the graph
    g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.0f, 1.0f);

    //draw the path
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.0f));
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

 // Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Walker Davis (with much help from Romain Michon and Tim O'Brian
// Description: Additive synthesizer with Variable Voices

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#include "Smooth.h"


class MainContentComponent :
    public AudioAppComponent,
    private Slider::Listener,
    private ToggleButton::Listener
{
public:
    MainContentComponent() : gain (0.0), onOff (0), samplingRate(0.0)
    {
        // configuring frequency slider and adding it to the main window
        addAndMakeVisible (frequencySlider);
        frequencySlider.setRange (50.0, 5000.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0);
        frequencySlider.setValue(100); // will also set the default frequency of the sine osc
        frequencySlider.addListener (this);
        
        
        
        // configuring frequency label box and adding it to the main window
        addAndMakeVisible(frequencyLabel);
        frequencyLabel.setText ("Frequency", dontSendNotification);
        frequencyLabel.attachToComponent (&frequencySlider, true);
        
        
        // configuring gain slider and adding it to the main window
        addAndMakeVisible (gainSlider);
        gainSlider.setRange (0.0, 1.0);
        gainSlider.setValue(0.5); // will alsi set the default gain of the sine osc
        gainSlider.addListener (this);
        
        
        // configuring gain label and adding it to the main window
        addAndMakeVisible(gainLabel);
        gainLabel.setText ("Gain", dontSendNotification);
        gainLabel.attachToComponent (&gainSlider, true);
        
        
        // configuring on/off button and adding it to the main window
        addAndMakeVisible(onOffButton);
        onOffButton.addListener(this);
        
        
        // configuring on/off label and adding it to the main window
        addAndMakeVisible(onOffLabel);
        onOffLabel.setText ("On/Off", dontSendNotification);
        onOffLabel.attachToComponent (&onOffButton, true);

        // random values for the frequencies of all sine waves excluding the fundamental
        for (int i=0; i<voices-1; i++){
            ratio[i] = (rand()/RAND_MAX * 4) + 1;
        }
        
        setSize (600, 100);
        nChans = 2; // make stereo
        setAudioChannels (0, nChans); // no inputs, two output
        
        for(int i = 0; i<5; i++){
            smooth[i].setSmooth(0.9999);
        }
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        // placing the UI elements in the main window
        // getWidth has to be used in case the window is resized by the user
        const int sliderLeft = 80;
        frequencySlider.setBounds (sliderLeft, 10, getWidth() - sliderLeft - 20, 20);
        gainSlider.setBounds (sliderLeft, 40, getWidth() - sliderLeft - 20, 20);
        onOffButton.setBounds (sliderLeft, 70, getWidth() - sliderLeft - 20, 20);
    }
    
    void buttonClicked (Button* button) override
    {
        // turns audio on or off
        if(button == &onOffButton && onOffButton.getToggleState()){
            onOff = 1;
            // randomize values for sine wave frequencies
            for (int i=0; i<voices-1; i++){
                ratio[i] = ((rand()/(float)RAND_MAX) * 10) + 1;
            }
            double f0 = frequencySlider.getValue();
            sine[0].setFrequency(f0);
            for (int i=1; i<voices; i++) {
                (sine[i]).setFrequency(f0 * ratio[i-1]);
                gain = gainSlider.getValue();
            }
        }
        else{
            onOff = 0;
        }
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (samplingRate > 0.0){
            if (slider == &frequencySlider){
                double f0 = frequencySlider.getValue();
                sine[0].setFrequency(f0);
                for (int i=1; i<voices; i++) {
                    (sine[i]).setFrequency(f0 * ratio[i-1]);
                }
                
            }
            else if (slider == &gainSlider){
                gain = gainSlider.getValue();
            }
        }
    }
    
 
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        samplingRate = sampleRate;
        for (int i=0; i<voices; i++){
            sine[i].setSamplingRate(sampleRate);
            
            
            
            
            
        }
    }
    
    void releaseResources() override
    {
    }
    
   
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        
        // getting the audio output buffer to be filled
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        float* const bufferRight = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
        
        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            
            for(int i=0; i<voices; i++){
                // smooth buffer
                buffer[sample] += sine[i].tick() * smooth[1].tick(gain) * smooth[2].tick(onOff) * .25;
                bufferRight[sample] = buffer[sample];
                
            }
        }
    }
    
private:
    // UI Elements
    Slider frequencySlider;
    Slider gainSlider;
    ToggleButton onOffButton;
    
    Label frequencyLabel, gainLabel, onOffLabel;
    
    int voices = 7; //select number of voices here
    Sine sine[20]; // the sine wave oscillator
    float ratio[3];
    Smooth smooth[5];
    
    
    // Global Variables
    float onOff, gain;
    int  samplingRate, nChans;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED

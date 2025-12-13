#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <Eventually.h>
#include <Config.h>
#include <Pins.h>

class Encoder {
private:
    int encCurrent;
    int encIncrement;
    int encLowLim;
    int encHighLim;
    int lastEncoderA;
    int lastEncoderB;
    bool lastPressState;
    bool buttonPressed;

public:
    Encoder();
    
    // Initialization
    void begin();
    
    // Encoder reading
    bool readRotation();
    bool handleButtonPress();
    bool handleButtonRelease();
    
    // Getters and setters
    int getCurrentValue() const { return encCurrent; }
    void setCurrentValue(int value) { encCurrent = value; }
    void setLimits(int lowLim, int highLim);
    void setIncrement(int increment) { encIncrement = increment; }
    
    // Update encoder limits based on current menu
    void updateLimitsForMenu();
    
    // Button state
    bool wasButtonPressed() { bool pressed = buttonPressed; buttonPressed = false; return pressed; }

private:
    void updateParameters();
};

// Global encoder instance
extern Encoder encoder;

// Eventually callback functions
bool encoderReadAction(EvtListener *lstn);
bool buttonPressAction(EvtListener *lstn);
bool buttonReleaseAction(EvtListener *lstn);

#endif // ENCODER_H
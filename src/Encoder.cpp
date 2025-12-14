#include "Encoder.h"

// Global encoder instance
Encoder encoder;

Encoder::Encoder() {
    encCurrent = initialTemp;
    encIncrement = ENC_INCREMENT;
    encLowLim = minTemp;
    encHighLim = maxTemp;
    lastEncoderA = HIGH;
    lastEncoderB = HIGH;
    lastPressState = false;
    buttonPressed = false;
}

void Encoder::begin() {
    pinMode(encButton, INPUT_PULLUP);
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    
    // Initialize encoder state
    lastEncoderA = digitalRead(pinA);
    lastEncoderB = digitalRead(pinB);
    
    // Set initial limits
    updateLimitsForMenu();
}

bool Encoder::readRotation() {
    int currentA = digitalRead(pinA);
    int currentB = digitalRead(pinB);
    
    // Detect falling edge on pinA (encoder rotation)
    if (currentA == LOW && lastEncoderA == HIGH) {
        // Determine direction based on pinB state
        if (currentB == HIGH) {
            // Clockwise rotation
            if (encCurrent < encHighLim) {
                encCurrent += encIncrement;
                updateParameters();
            }
        } else {
            // Counter-clockwise rotation
            if (encCurrent > encLowLim) {
                encCurrent -= encIncrement;
                updateParameters();
            }
        }
    }
    
    lastEncoderA = currentA;
    lastEncoderB = currentB;
    return false; // Continue event chain
}

bool Encoder::handleButtonPress() {
    // Always set the flag when called from interrupt
    buttonPressed = true;
    lastPressState = true;
    return false;
}

bool Encoder::handleButtonRelease() {
    lastPressState = false;
    return false;
}

void Encoder::setLimits(int lowLim, int highLim) {
    encLowLim = lowLim;
    encHighLim = highLim;
    // Constrain current value to new limits
    encCurrent = constrain(encCurrent, encLowLim, encHighLim);
}

void Encoder::updateLimitsForMenu() {
    // Limits will be set by SystemControl based on current menu
}

void Encoder::updateParameters() {
    // This will be handled by SystemControl to avoid circular dependencies
}

void Encoder::updateFromInterrupt(int delta) {
    // Update encoder value based on interrupt delta
    int newValue = encCurrent + (delta * encIncrement);
    
    // Apply limits
    newValue = constrain(newValue, encLowLim, encHighLim);
    
    // Only update if value actually changed
    if (newValue != encCurrent) {
        encCurrent = newValue;
        updateParameters();
    }
}

// Eventually callback functions
bool encoderReadAction(EvtListener *lstn) {
    return encoder.readRotation();
}

bool buttonPressAction(EvtListener *lstn) {
    return encoder.handleButtonPress();
}

bool buttonReleaseAction(EvtListener *lstn) {
    return encoder.handleButtonRelease();
}
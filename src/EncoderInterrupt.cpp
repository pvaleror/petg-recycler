#include "EncoderInterrupt.h"

// Global encoder instance
EncoderInterrupt encoderInterrupt;

EncoderInterrupt::EncoderInterrupt() {
    encoderPosition = 0;
    buttonPressed = false;
    lastEncoderTime = 0;
    lastButtonTime = 0;
    lastButtonState = HIGH;
}

void EncoderInterrupt::begin() {
    // Configure pins with pullups
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(encButton, INPUT_PULLUP);
    
    // Attach interrupts for ultra-responsive input
    attachInterrupt(digitalPinToInterrupt(pinA), handleEncoderCLK, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encButton), handleButton, CHANGE);
}

// Static interrupt handler for encoder rotation
void IRAM_ATTR EncoderInterrupt::handleEncoderCLK() {
    uint32_t currentTime = micros();
    
    // Debounce - ignore if too soon after last change
    if (currentTime - encoderInterrupt.lastEncoderTime < ENCODER_DEBOUNCE_MICROS) {
        return;
    }
    
    encoderInterrupt.lastEncoderTime = currentTime;
    
    // Read both pins
    int clkLevel = digitalRead(pinA);
    int dtLevel = digitalRead(pinB);
    
    // Determine direction based on pin states
    if (clkLevel == LOW) {
        // CLK went low - check DT to determine direction
        if (dtLevel == HIGH) {
            encoderInterrupt.encoderPosition++;  // Clockwise
        } else {
            encoderInterrupt.encoderPosition--;  // Counter-clockwise
        }
    }
}

// Static interrupt handler for button
void IRAM_ATTR EncoderInterrupt::handleButton() {
    uint32_t currentTime = millis();
    
    // Debounce - ignore if too soon after last change
    if (currentTime - encoderInterrupt.lastButtonTime < BUTTON_DEBOUNCE_MILLIS) {
        return;
    }
    
    encoderInterrupt.lastButtonTime = currentTime;
    
    // Read button state
    bool currentButtonState = digitalRead(encButton);
    
    // Detect falling edge (button press)
    if (encoderInterrupt.lastButtonState == HIGH && currentButtonState == LOW) {
        encoderInterrupt.buttonPressed = true;
    }
    
    encoderInterrupt.lastButtonState = currentButtonState;
}

int EncoderInterrupt::getPositionDelta() {
    static int lastPosition = 0;
    int currentPos = encoderPosition;
    int delta = currentPos - lastPosition;
    lastPosition = currentPos;
    return delta;
}

bool EncoderInterrupt::wasButtonPressed() {
    bool pressed = buttonPressed;
    buttonPressed = false;  // Clear flag
    return pressed;
}

void EncoderInterrupt::resetPosition() {
    encoderPosition = 0;
}
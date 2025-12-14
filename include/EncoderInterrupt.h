#ifndef ENCODER_INTERRUPT_H
#define ENCODER_INTERRUPT_H

#include <Arduino.h>
#include <Config.h>
#include <Pins.h>

class EncoderInterrupt {
private:
    volatile int encoderPosition;
    volatile bool buttonPressed;
    volatile uint32_t lastEncoderTime;
    volatile uint32_t lastButtonTime;
    volatile bool lastButtonState;

public:
    EncoderInterrupt();
    
    // Initialization with interrupts
    void begin();
    
    // Interrupt handlers (must be static for attachInterrupt)
    static void IRAM_ATTR handleEncoderCLK();
    static void IRAM_ATTR handleButton();
    
    // Non-interrupt functions
    int getPositionDelta();
    bool wasButtonPressed();
    void resetPosition();
    
    // Getters
    int getCurrentPosition() const { return encoderPosition; }
};

// Global encoder instance
extern EncoderInterrupt encoderInterrupt;

#endif // ENCODER_INTERRUPT_H
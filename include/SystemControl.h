#ifndef SYSTEM_CONTROL_H
#define SYSTEM_CONTROL_H

#include <Arduino.h>
// FreeRTOS implementation - no Eventually needed
#include <Config.h>

class SystemControl {
private:
    unsigned long currentTime;
    bool initialized;

public:
    SystemControl();
    
    // Initialization
    bool begin();
    
    // System control
    void update();
    bool checkTemperatureSafety();
    void updateEncoderLimitsForCurrentMenu();
    void handleEncoderChange(int encoderValue);
    void handleMenuNavigation();
    
    // Eventually action functions
    bool updateDisplayAction();
    bool checkInputsAction();
    bool handleMenuAction();

    
    // Getters
    bool isInitialized() const { return initialized; }

private:
    void setupEventually();
};

// Global system control instance
extern SystemControl systemControl;

// FreeRTOS implementation - no callback functions needed


#endif // SYSTEM_CONTROL_H
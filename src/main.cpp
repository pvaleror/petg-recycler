//=============================================================================
// PET BOTTLE RECYCLER CONTROLLER
//=============================================================================
// Modular version - Refactored for better maintainability
// 
// Original project: https://www.the-diy-life.com/pet-bottle-recycler-part-1-using-an-arduino-uno-r4-to-control-a-3d-printers-hotend/
// Hardware: ESP32-C3 DevKit M-1
// 
// This version separates functionality into modules:
// - Motor: Stepper motor control
// - Thermistor: Temperature sensing
// - Display: OLED screen management  
// - Menu: User interface and navigation
// - Encoder: Rotary encoder input
// - SystemControl: Overall system coordination
//=============================================================================

#include <Arduino.h>
#include <Eventually.h>
#include <WiFi.h>
#include <esp_bt.h>

// Project modules
#include <Config.h>
#include <Pins.h>
#include <Motor.h>
#include <Thermistor.h>
#include <Display.h>
#include <Menu.h>
#include <Encoder.h>
#include <SystemControl.h>

// Global Eventually manager
EvtManager mgr;

void setup() {
    // Disable WiFi and Bluetooth to reduce interruptions for smooth motor operation
    WiFi.mode(WIFI_OFF);
    btStop();
    
    // Initialize the system
    if (!systemControl.begin()) {
        // Halt execution on critical failure
        while(1) {
            delay(1000);
        }
    }
}

void loop() {
    // AsyncStepper motor updates - PRIORITY #1
    // This must be called as frequently as possible for smooth stepping
    motor.update();
    
    // Low-frequency system tasks
    static unsigned long lastSystemUpdate = 0;
    static unsigned long lastDisplayUpdate = 0;
    static unsigned long lastEncoderRead = 0;
    
    unsigned long currentMillis = millis();
    
    // Read encoder and button every 5ms
    if (currentMillis - lastEncoderRead >= 5) {
        encoder.readRotation();
        
        // Read button state and handle press/release
        static bool lastButtonState = HIGH;
        bool currentButtonState = digitalRead(encButton);
        
        if (currentButtonState == LOW && lastButtonState == HIGH) {
            // Button pressed
            encoder.handleButtonPress();
        } else if (currentButtonState == HIGH && lastButtonState == LOW) {
            // Button released
            encoder.handleButtonRelease();
        }
        
        lastButtonState = currentButtonState;
        lastEncoderRead = currentMillis;
    }
    
    // System updates every 50ms
    if (currentMillis - lastSystemUpdate >= 50) {
        // Check button press
        if (encoder.wasButtonPressed()) {
            menu.navigate();
            systemControl.updateEncoderLimitsForCurrentMenu();
        }
        
        // Update parameters
        menu.updateParameters(encoder.getCurrentValue());
        
        // Update thermistor
        thermistor.update();
        
        // Safety checks
        systemControl.checkTemperatureSafety();
        
        lastSystemUpdate = currentMillis;
    }
    
    // Update display every 200ms
    if (currentMillis - lastDisplayUpdate >= 200) {
        systemControl.updateDisplayAction();
        lastDisplayUpdate = currentMillis;
    }
}
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
    // Initialize the system
    if (!systemControl.begin()) {
        Serial.println(F("FATAL ERROR: System initialization failed!"));
        // Halt execution on critical failure
        while(1) {
            delay(1000);
            Serial.println(F("System halted due to initialization failure"));
        }
    }
    
    Serial.println(F("=== PET Recycler Controller Ready ==="));
    Serial.println(F("Use rotary encoder to navigate menus"));
    Serial.println(F("Press encoder button to change menu"));
}

// Use Eventually's event loop instead of traditional loop()
USE_EVENTUALLY_LOOP(mgr)
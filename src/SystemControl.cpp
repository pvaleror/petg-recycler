#include "SystemControl.h"
#include "Motor.h"
#include "Thermistor.h"
#include "Display.h"
#include "Menu.h"
#include "Encoder.h"

// Global system control instance
SystemControl systemControl;

// Global Eventually manager
extern EvtManager mgr;

SystemControl::SystemControl() {
    currentTime = 0;
    initialized = false;
}

bool SystemControl::begin() {
    delay(100);
    
    // Initialize display first
    if (!display.begin()) {
        return false;
    }
    
    display.showInitMessage("Initializing...");
    delay(100); // Reduced from 500ms to 100ms
    
    // Initialize thermistor
    thermistor.begin();
    
    // Initialize motor
    motor.begin();
    
    // Initialize encoder
    encoder.begin();
    
    // Initialize menu system
    menu.begin();
    menu.setMotorReference(&motor);
    menu.setThermistorReference(&thermistor);
    
    // Set initial encoder limits
    updateEncoderLimitsForCurrentMenu();
    
    currentTime = millis();
    initialized = true;
    
    return true;
}

void SystemControl::setupEventually() {
    // Button listeners
    mgr.addListener(new EvtPinListener(encButton, ENCODER_DEBOUNCE, HIGH, (EvtAction)buttonPressAction));
    mgr.addListener(new EvtPinListener(encButton, 1, LOW, (EvtAction)buttonReleaseAction));
    
    // Encoder listener
    mgr.addListener(new EvtTimeListener(ENCODER_READ_INTERVAL, true, (EvtAction)encoderReadAction));
    
    // System update listeners
    mgr.addListener(new EvtTimeListener(EventLoopTime, true, (EvtAction)updateDisplayCallback));
    mgr.addListener(new EvtTimeListener(SensorLoopTime, true, (EvtAction)checkInputsCallback));
    mgr.addListener(new EvtTimeListener(MenuLoopTime, true, (EvtAction)handleMenuCallback));
}

void SystemControl::update() {
    if (!initialized) return;
    
    currentTime = millis();
    
    // Update subsystems (motor is updated in main loop for high frequency)
    thermistor.update();
    
    // Check safety conditions
    checkTemperatureSafety();
}

bool SystemControl::checkTemperatureSafety() {
    double currentTemp = thermistor.getCurrentTemperature();
    
    // Check for overtemperature condition
    if (currentTemp > maxTemp + tempTolerance) {
        motor.stop();
        menu.setSystemState(SYSTEM_ERROR);
        return false;
    }
    
    // Check if temperature is in safe operating range
    if (currentTemp < minTemp - tempTolerance) {
        if (menu.getSystemState() == SYSTEM_EXTRUDING) {
            motor.stop();
            menu.setSystemState(SYSTEM_IDLE);
            return false;
        }
    }
    
    return true;
}

void SystemControl::updateEncoderLimitsForCurrentMenu() {
    switch (menu.getCurrentMenu()) {
        case MENU_TEMPERATURE:
            encoder.setCurrentValue(menu.getSetpoint());
            encoder.setLimits(minTemp, maxTemp);
            break;
            
        case MENU_SPEED:
            encoder.setCurrentValue(motor.getSpeed());
            encoder.setLimits(minSpeed, maxSpeed);
            break;
            
        case MENU_EXTRUDE:
            encoder.setCurrentValue(motor.getDirection());
            encoder.setLimits(0, 2);
            break;
            
        case MENU_STATUS:
            encoder.setLimits(0, 0);
            break;
    }
}

bool SystemControl::updateDisplayAction() {
    display.clear();
    display.showLabels(menu.getCurrentMenu());
    display.showValues(
        thermistor.getCurrentTemperature(),
        menu.getSetpoint(),
        motor.getSpeed(),
        motor.getDirectionText()
    );
    
    // Show status message if motor is active (regardless of current menu)
    if (motor.isExtruding()) {
        display.showStatus(motor.getStatusMessage());
    }
    
    display.update();
    return false;
}

bool SystemControl::checkInputsAction() {
    thermistor.update();
    return false;
}

bool SystemControl::handleMenuAction() {
    // Check if button was pressed
    if (encoder.wasButtonPressed()) {
        menu.navigate();
        // Update encoder limits for new menu
        updateEncoderLimitsForCurrentMenu();
    }
    
    // Update encoder parameters
    menu.updateParameters(encoder.getCurrentValue());
    
    update();
    return false;
}



// Eventually callback functions
bool updateDisplayCallback(EvtListener *lstn) {
    return systemControl.updateDisplayAction();
}

bool checkInputsCallback(EvtListener *lstn) {
    return systemControl.checkInputsAction();
}

bool handleMenuCallback(EvtListener *lstn) {
    return systemControl.handleMenuAction();
}


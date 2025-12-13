#include "Menu.h"
#include "Motor.h"
#include "Thermistor.h"

// Global menu instance
Menu menu;

Menu::Menu() {
    currentMenu = MENU_TEMPERATURE;
    systemState = SYSTEM_IDLE;
    dataInputNo = 0;
    setpoint = initialTemp;
    motorPtr = nullptr;
    thermistorPtr = nullptr;
}

void Menu::begin() {
    currentMenu = MENU_TEMPERATURE;
    systemState = SYSTEM_IDLE;
    dataInputNo = 0;
    setpoint = initialTemp;
}

void Menu::setMotorReference(class Motor* motor) {
    motorPtr = motor;
}

void Menu::setThermistorReference(class Thermistor* thermistor) {
    thermistorPtr = thermistor;
}

void Menu::navigate() {
    // Cycle through menu states
    switch (currentMenu) {
        case MENU_TEMPERATURE:
            currentMenu = MENU_SPEED;
            dataInputNo = 1;
            break;
            
        case MENU_SPEED:
            currentMenu = MENU_EXTRUDE;
            dataInputNo = 2;
            break;
            
        case MENU_EXTRUDE:
            currentMenu = MENU_TEMPERATURE;
            dataInputNo = 0;
            break;
            
        case MENU_STATUS:
            currentMenu = MENU_TEMPERATURE;
            dataInputNo = 0;
            break;
    }
}

void Menu::updateParameters(int encoderValue) {
    // Update the parameter being edited based on current menu state
    switch (currentMenu) {
        case MENU_TEMPERATURE:
            setpoint = encoderValue;
            break;
            
        case MENU_SPEED:
            // Apply speed to motor immediately if it's running
            if (motorPtr) {
                static int lastSpeed = -1;
                if (encoderValue != lastSpeed) {
                    motorPtr->setSpeed(encoderValue);
                    lastSpeed = encoderValue;
                }
            }
            break;

        case MENU_EXTRUDE:
            // encoderValue maps to: 0=Rev, 1=Off, 2=Fwd
            encoderValue = constrain(encoderValue, 0, 2);
            if (motorPtr) {
                // Only update if value actually changed to avoid repeated calls
                static int lastDirection = -1;
                if (encoderValue != lastDirection) {
                    motorPtr->setUserSelection(encoderValue);
                    lastDirection = encoderValue;
                }
            }
            break;
            
        case MENU_STATUS:
            // Status menu is read-only, no parameter updates
            break;
    }
}

void Menu::setEncoderLimits(int& encCurrent, int& encLowLim, int& encHighLim) {
    switch (currentMenu) {
        case MENU_TEMPERATURE:
            encCurrent = setpoint;
            encLowLim = minTemp;
            encHighLim = maxTemp;
            break;
            
        case MENU_SPEED:
            encCurrent = motorPtr ? motorPtr->getSpeed() : 5;
            encLowLim = minSpeed;
            encHighLim = maxSpeed;
            break;
            
        case MENU_EXTRUDE:
            encCurrent = motorPtr ? motorPtr->getDirection() : 1;
            encLowLim = 0;
            encHighLim = 2;
            break;
            
        case MENU_STATUS:
            encCurrent = 0;
            encLowLim = 0;
            encHighLim = 0;
            break;
    }
}

void Menu::updateDisplay() {
    // This function will be called from SystemControl to avoid circular dependencies
    // Implementation moved to SystemControl::updateDisplayAction()
}
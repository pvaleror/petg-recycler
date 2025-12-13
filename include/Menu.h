#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Config.h>
#include <Types.h>

class Menu {
private:
    MenuState currentMenu;
    SystemState systemState;
    int dataInputNo;
    double setpoint;
    
    // Forward declarations to avoid circular dependencies
    class Motor* motorPtr;
    class Thermistor* thermistorPtr;

public:
    Menu();
    
    // Initialization
    void begin();
    void setMotorReference(class Motor* motor);
    void setThermistorReference(class Thermistor* thermistor);
    
    // Navigation
    void navigate();
    void updateParameters(int encoderValue);
    
    // Getters
    MenuState getCurrentMenu() const { return currentMenu; }
    SystemState getSystemState() const { return systemState; }
    double getSetpoint() const { return setpoint; }
    int getDataInputNo() const { return dataInputNo; }
    
    // Setters
    void setSystemState(SystemState state) { systemState = state; }
    
    // Display update
    void updateDisplay();

private:
    void setEncoderLimits(int& encCurrent, int& encLowLim, int& encHighLim);
};

// Global menu instance
extern Menu menu;

#endif // MENU_H
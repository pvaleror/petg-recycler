#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <AsyncStepper.hpp>
#include <Config.h>
#include <Pins.h>
#include <Types.h>

class Motor {
private:
    AsyncStepper stepper;
    bool motorEnabled;
    int motSpeed;
    int motDir;
    int selectedDirection;  // User's selected direction (0=Rev, 1=Off, 2=Fwd)
    MotorState motorState;
    bool extrusionActive;  // Tracks if extrusion is active independent of menu

public:
    Motor();
    
    // Initialization
    void begin();
    
    // Motor control
    void enable(bool enable);
    void setDirection(bool forward);
    void setUserSelection(int selection);  // 0=Rev, 1=Off, 2=Fwd
    void setSpeed(int speed);
    void stop();
    void update();
    
    // Getters
    bool isEnabled() const { return motorEnabled; }
    int getSpeed() const { return motSpeed; }
    int getDirection() const { return selectedDirection; }  // Return user selection for encoder
    MotorState getState() const { return motorState; }
    bool isExtruding() const { return extrusionActive; }
    const char* getDirectionText() const;
    const char* getStatusMessage() const;
};

// Global motor instance
extern Motor motor;

#endif // MOTOR_H
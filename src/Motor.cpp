#include "Motor.h"

// Global motor instance
Motor motor;

Motor::Motor() : stepper(stepsPerRevolution, motDirPin, motStepPin) {
    motorEnabled = false;
    motSpeed = MOTOR_DEFAULT_SPEED;
    motDir = MOTOR_OFF;
    selectedDirection = MOTOR_OFF;  // Start with OFF selected
    motorState = STOPPED;
    extrusionActive = false;
}

void Motor::begin() {
    // Configure enable pin
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, EN_ACTIVE_LOW ? HIGH : LOW); // Start disabled
    
    // Configure stepper parameters
    stepper.SetSpeed(MOTOR_MAX_SPEED);  // Maximum steps per second
    stepper.SetAcceleration(MOTOR_ACCELERATION);  // Steps per second^2
    stepper.Mode = AsyncStepper::StepperMode::Constant;  // Use constant speed mode for smoother operation
    
    // Set default speed
    setSpeed(motSpeed);
}

void Motor::enable(bool enable) {
    // Control enable pin
    digitalWrite(enablePin, EN_ACTIVE_LOW ? !enable : enable);
    motorEnabled = enable;
    extrusionActive = enable;  // Track extrusion state
    
    if (!enable) {
        motorState = STOPPED;
    }
}

void Motor::setDirection(bool forward) {
    motDir = forward ? MOTOR_FORWARD : MOTOR_REVERSE;
    
    // Update motor state immediately if enabled
    if (motorEnabled) {
        motorState = forward ? MOVING_FWD : MOVING_REV;
        
        // Start continuous movement in the desired direction
        AsyncStepper::StepperDirection dir = forward ? AsyncStepper::StepperDirection::CW : AsyncStepper::StepperDirection::CCW;
        stepper.RotateContinuous(dir);
    }
}

void Motor::setSpeed(int speed) {
    // Convert speed (1-25) to steps per second
    motSpeed = constrain(speed, minSpeed, maxSpeed);
    
    // Map speed to reasonable steps per second range
    // Speed 1 = 50 steps/sec, Speed 25 = 2000 steps/sec
    long stepsPerSecond = map(motSpeed, minSpeed, maxSpeed, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
    
    stepper.SetSpeed(stepsPerSecond);
}

void Motor::stop() {
    motorEnabled = false;
    extrusionActive = false;
    motorState = STOPPED;
    
    // Disable motor via enable pin
    digitalWrite(enablePin, EN_ACTIVE_LOW ? HIGH : LOW);
}

void Motor::update() {
    // AsyncStepper handles all the timing internally
    // This must be called frequently for smooth operation
    stepper.Update();
    
    // Update motor state based on current status
    if (motorEnabled) {
        if (stepper.State == AsyncStepper::StepperState::Running) {
            motorState = (motDir == MOTOR_FORWARD) ? MOVING_FWD : MOVING_REV;
        } else if (stepper.State == AsyncStepper::StepperState::Stopped && motDir != MOTOR_OFF) {
            // Motor should be running but stopped - restart continuous movement
            AsyncStepper::StepperDirection dir = (motDir == MOTOR_FORWARD) ? AsyncStepper::StepperDirection::CW : AsyncStepper::StepperDirection::CCW;
            stepper.RotateContinuous(dir);
        }
    }
}

void Motor::setUserSelection(int selection) {
    selectedDirection = constrain(selection, 0, 2);
    
    if (selectedDirection == 1) {
        // Off selected - disable motor
        stop();
    } else {
        // Direction selected - set direction and enable
        enable(true);
        setDirection(selectedDirection == 2);  // 2 = forward, 0 = reverse
    }
}

const char* Motor::getDirectionText() const {
    // Show the user's selection
    if (selectedDirection == MOTOR_REVERSE) return "Rev";
    if (selectedDirection == MOTOR_FORWARD) return "Fwd";
    return "Off";
}

const char* Motor::getStatusMessage() const {
    static unsigned long lastUpdate = 0;
    static const char* statusMessage = "";
    
    // Only update status message every 500ms to prevent flickering
    if (millis() - lastUpdate > 500) {
        if (extrusionActive && motDir == MOTOR_REVERSE) {
            statusMessage = "Unloading...";
        } else if (extrusionActive && motDir == MOTOR_FORWARD) {
            statusMessage = "Loading...";
        } else {
            statusMessage = "";
        }
        lastUpdate = millis();
    }
    
    return statusMessage;
}
#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Config.h>
#include <Pins.h>
#include <Types.h>

class Motor {
private:
    // Variables de control motor (volatile para FreeRTOS)
    volatile bool motorEnabled;
    volatile int motSpeed;                  // User speed (1-25)
    volatile int motDir;
    volatile int selectedDirection;         // User's selected direction (0=Rev, 1=Off, 2=Fwd)
    volatile MotorState motorState;
    volatile bool extrusionActive;
    
    // Variables de ramping (para movimiento suave)
    volatile int currentSpeedStepsPerSec;   // Velocidad actual en steps/s
    volatile int targetSpeedStepsPerSec;    // Velocidad objetivo en steps/s
    volatile float acceleration;            // Aceleración actual
    
    // Timing preciso de pasos
    volatile uint32_t lastStepTime;         // Último paso en micros()
    volatile uint32_t nextStepInterval;     // Intervalo hasta próximo paso (μs)
    
    // Task handle para FreeRTOS
    TaskHandle_t motorTaskHandle;

public:
    Motor();
    
    // Initialization
    void begin();
    
    // Motor control
    void enable(bool enable);
    void setDirection(bool forward);
    void setUserSelection(int selection);  // 0=Rev, 1=Off, 2=Fwd
    void setSpeed(int speed);              // 1-25 user speed
    void setTargetSpeedSteps(int stepsPerSec); // Direct steps/s control
    void stop();
    
    // FreeRTOS motor task (static para callback)
    static void motorTaskFunction(void* pvParameters);
    void updateMotor();  // Called from motor task
    
    // Ramping functions
    void updateRamping();
    void calculateStepInterval();
    
    // Getters
    bool isEnabled() const { return motorEnabled; }
    int getSpeed() const { return motSpeed; }
    int getDirection() const { return selectedDirection; }
    MotorState getState() const { return motorState; }
    bool isExtruding() const { return extrusionActive; }
    int getCurrentSpeedSteps() const { return currentSpeedStepsPerSec; }
    int getTargetSpeedSteps() const { return targetSpeedStepsPerSec; }
    const char* getDirectionText() const;
    const char* getStatusMessage() const;
};

// Global motor instance
extern Motor motor;

#endif // MOTOR_H
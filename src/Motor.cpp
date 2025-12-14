#include "Motor.h"

// Global motor instance
Motor motor;

Motor::Motor() {
    motorEnabled = false;
    motSpeed = MOTOR_DEFAULT_SPEED;
    motDir = MOTOR_OFF;
    selectedDirection = MOTOR_OFF;
    motorState = STOPPED;
    extrusionActive = false;
    
    // Ramping variables
    currentSpeedStepsPerSec = 0;
    targetSpeedStepsPerSec = 0;
    acceleration = MOTOR_ACCELERATION_STEPS_S2;
    
    // Timing variables
    lastStepTime = 0;
    nextStepInterval = 0;
    
    motorTaskHandle = NULL;
}

void Motor::begin() {
    // Configure pins directly - simple and efficient
    pinMode(motStepPin, OUTPUT);
    pinMode(motDirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    
    // Initialize pins
    digitalWrite(motStepPin, LOW);
    digitalWrite(motDirPin, HIGH);  // Default direction
    digitalWrite(enablePin, EN_ACTIVE_LOW ? HIGH : LOW); // Start disabled
    
    // Set default speed
    setSpeed(motSpeed);
    
    // Create FreeRTOS motor task with highest priority
    xTaskCreate(
        motorTaskFunction,      // Task function
        "MotorTask",           // Task name
        MOTOR_TASK_STACK,      // Stack size
        this,                  // Parameters (this motor instance)
        MOTOR_TASK_PRIORITY,   // Priority (highest)
        &motorTaskHandle       // Task handle
    );
}

// Static function for FreeRTOS task
void Motor::motorTaskFunction(void* pvParameters) {
    Motor* motorInstance = static_cast<Motor*>(pvParameters);
    
    // High precision timing for motor task
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t motorUpdateFreq = pdMS_TO_TICKS(MOTOR_TASK_INTERVAL);
    
    while (1) {
        motorInstance->updateMotor();
        
        // Use vTaskDelayUntil for precise timing
        vTaskDelayUntil(&lastWakeTime, motorUpdateFreq);
    }
}

void Motor::updateMotor() {
    if (!motorEnabled) return;
    
    // Update ramping (smooth acceleration/deceleration)
    updateRamping();
    
    // Calculate step interval based on current speed
    calculateStepInterval();
    
    // Generate step pulses with precise timing
    uint32_t currentMicros = micros();
    
    if (currentSpeedStepsPerSec > 0 && nextStepInterval > 0) {
        if ((currentMicros - lastStepTime) >= nextStepInterval) {
            // Generate step pulse (HIGH pulse)
            digitalWrite(motStepPin, HIGH);
            delayMicroseconds(2);  // Minimum pulse width for most drivers
            digitalWrite(motStepPin, LOW);
            
            // Update timing
            lastStepTime = currentMicros;
            
            // Update motor state
            motorState = (motDir == MOTOR_FORWARD) ? MOVING_FWD : MOVING_REV;
        }
    }
}

void Motor::updateRamping() {
    if (currentSpeedStepsPerSec != targetSpeedStepsPerSec) {
        int speedDelta = targetSpeedStepsPerSec - currentSpeedStepsPerSec;
        
        // Calculate maximum change per millisecond (acceleration limiting)
        int maxDeltaPerMs = acceleration / 1000; // Convert steps/s² to steps/s/ms
        if (maxDeltaPerMs < 1) maxDeltaPerMs = 1;
        
        // Apply ramping
        if (abs(speedDelta) > maxDeltaPerMs) {
            currentSpeedStepsPerSec += (speedDelta > 0) ? maxDeltaPerMs : -maxDeltaPerMs;
        } else {
            currentSpeedStepsPerSec = targetSpeedStepsPerSec;
        }
    }
}

void Motor::calculateStepInterval() {
    if (currentSpeedStepsPerSec > 0) {
        // Calculate microseconds between steps
        // If speed = 1000 steps/s, interval = 1,000,000 / 1000 = 1000 μs
        nextStepInterval = 1000000 / abs(currentSpeedStepsPerSec);
    } else {
        nextStepInterval = 0; // No steps
    }
}

void Motor::enable(bool enable) {
    // Control enable pin
    digitalWrite(enablePin, EN_ACTIVE_LOW ? !enable : enable);
    motorEnabled = enable;
    extrusionActive = enable;
    
    if (!enable) {
        motorState = STOPPED;
        currentSpeedStepsPerSec = 0;
        targetSpeedStepsPerSec = 0;
    }
}

void Motor::setDirection(bool forward) {
    motDir = forward ? MOTOR_FORWARD : MOTOR_REVERSE;
    
    // Set direction pin directly
    digitalWrite(motDirPin, forward ? HIGH : LOW);
    delayMicroseconds(MOTOR_DIRECTION_SETTLE_TIME);
    
    // Update motor state immediately if enabled
    if (motorEnabled) {
        motorState = forward ? MOVING_FWD : MOVING_REV;
    }
}

void Motor::setSpeed(int speed) {
    // 1) Limitar rango de entrada
    motSpeed = constrain(speed, minSpeed, maxSpeed);   // 1..25

    // 2) Normalizar a 0..1
    float xNorm = (float)(motSpeed - minSpeed) / (float)(maxSpeed - minSpeed);

    // 3) Aplicar curva exponencial suave
    const float k = 2.1155f;               // Ajusta 2.0f o 2.5f según sensación
    float xCurve = pow(xNorm, k);       // Requiere <math.h> incluido

    // 4) Mapear al rango de steps/s no linealmente
    float stepsPerSecF = MOTOR_MIN_SPEED_STEPS_SEC +
                         xCurve * (MOTOR_MAX_SPEED_STEPS_SEC - MOTOR_MIN_SPEED_STEPS_SEC);

    int stepsPerSec = (int)(stepsPerSecF + 0.5f);  // Redondeo a entero

    // 5) Aplicar al motor
    setTargetSpeedSteps(stepsPerSec);
}

void Motor::setTargetSpeedSteps(int stepsPerSec) {
    targetSpeedStepsPerSec = constrain(stepsPerSec, 0, MOTOR_MAX_SPEED_STEPS_SEC);
}

void Motor::stop() {
    motorEnabled = false;
    extrusionActive = false;
    motorState = STOPPED;
    currentSpeedStepsPerSec = 0;
    targetSpeedStepsPerSec = 0;
    
    // Disable motor via enable pin
    digitalWrite(enablePin, EN_ACTIVE_LOW ? HIGH : LOW);
}

void Motor::setUserSelection(int selection) {
    selectedDirection = constrain(selection, 0, 2);
    
    if (selectedDirection == 1) {
        // Off selected - disable motor
        stop();
    } else {
        // Direction selected - set direction and enable
        setDirection(selectedDirection == 2);  // 2 = forward, 0 = reverse
        
        // Ensure motor has a valid speed before enabling
        if (targetSpeedStepsPerSec == 0) {
            setSpeed(motSpeed);  // Use current user speed setting
        }
        
        enable(true);
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
    
    // Update status message using interval from Config.h
    if (millis() - lastUpdate > MOTOR_STATUS_UPDATE_INTERVAL) {
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
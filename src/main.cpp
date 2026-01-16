//=============================================================================
// PET BOTTLE RECYCLER CONTROLLER - FREERTOS EDITION
//=============================================================================
// Professional FreeRTOS implementation with:
// - Real multitasking with priorities
// - Hardware interrupts for encoder
// - Smooth motor ramping
// - Microsecond precision timing
// - Industrial-grade performance
//=============================================================================

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <esp_bt.h>

// Project modules
#include <Config.h>
#include <Pins.h>
#include <Motor.h>
#include <Encoder.h>
#include <EncoderInterrupt.h>
#include <Thermistor.h>
#include <Display.h>
#include <Menu.h>
#include <SystemControl.h>

// Task handles for FreeRTOS
TaskHandle_t encoderTaskHandle = NULL;
TaskHandle_t menuTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;

// ============================================================================
// FREERTOS TASK FUNCTIONS
// ============================================================================

// Encoder Task - High priority for responsive input
void encoderTask(void* pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t taskFrequency = pdMS_TO_TICKS(ENCODER_TASK_INTERVAL);
    
    while (1) {
        // Process encoder changes (captured by interrupts)
        int delta = encoderInterrupt.getPositionDelta();
        if (delta != 0) {
            // Update encoder value in main encoder object
            encoder.updateFromInterrupt(delta);
        }
        
        // Process button presses (captured by interrupts)
        if (encoderInterrupt.wasButtonPressed()) {
            encoder.handleButtonPress();
        }
        
        vTaskDelayUntil(&lastWakeTime, taskFrequency);
    }
}

// Menu Task - Medium priority for UI updates
void menuTask(void* pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t taskFrequency = pdMS_TO_TICKS(MENU_TASK_INTERVAL);
    
    while (1) {
        // Check for button press
        if (encoder.wasButtonPressed()) {
            menu.navigate();
            systemControl.updateEncoderLimitsForCurrentMenu();
            
            // Small delay to prevent multiple rapid menu changes
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        
        // Update parameters based on encoder
        menu.updateParameters(encoder.getCurrentValue());
        
        vTaskDelayUntil(&lastWakeTime, taskFrequency);
    }
}

// Sensor Task - Low priority for temperature monitoring
void sensorTask(void* pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t taskFrequency = pdMS_TO_TICKS(SENSOR_TASK_INTERVAL);
    
    while (1) {
        // Update thermistor
        thermistor.update();
        
        // Safety checks
        systemControl.checkTemperatureSafety();
        
        vTaskDelayUntil(&lastWakeTime, taskFrequency);
    }
}

// Display Task - Lowest priority for screen updates
void displayTask(void* pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t taskFrequency = pdMS_TO_TICKS(DISPLAY_TASK_INTERVAL);
    
    while (1) {
        // Update display
        systemControl.updateDisplayAction();
        
        vTaskDelayUntil(&lastWakeTime, taskFrequency);
    }
}

// ============================================================================
// SETUP AND MAIN LOOP
// ============================================================================

void setup() {
    // Disable WiFi and Bluetooth for maximum performance
    WiFi.mode(WIFI_OFF);
    btStop();
    Serial.begin(115200);
    delay(1000);              // dar tiempo al PC a enganchar el puerto
    Serial.println("Arrancando...");

    // Initialize hardware interrupt encoder first
    encoderInterrupt.begin();
    
    // Initialize the system
    if (!systemControl.begin()) {
        // Halt execution on critical failure
        while(1) {
            delay(1000);
        }
    }
    
    // Create FreeRTOS tasks with priorities
    // Motor task is created in Motor::begin() with highest priority (5)
    
    xTaskCreate(
        encoderTask,
        "EncoderTask",
        ENCODER_TASK_STACK,
        NULL,
        ENCODER_TASK_PRIORITY,  // Priority 4
        &encoderTaskHandle
    );
    
    xTaskCreate(
        menuTask,
        "MenuTask", 
        MENU_TASK_STACK,
        NULL,
        MENU_TASK_PRIORITY,     // Priority 3
        &menuTaskHandle
    );
    
    xTaskCreate(
        sensorTask,
        "SensorTask",
        SENSOR_TASK_STACK,
        NULL,
        SENSOR_TASK_PRIORITY,   // Priority 2
        &sensorTaskHandle
    );
    
    xTaskCreate(
        displayTask,
        "DisplayTask",
        DISPLAY_TASK_STACK,
        NULL,
        DISPLAY_TASK_PRIORITY,  // Priority 1
        &displayTaskHandle
    );
}

void loop() {
    // FreeRTOS handles everything - main loop can be empty or do minimal work
    // All real work is done in tasks with proper priorities
    // Optional: Add watchdog or system monitoring here
    delay(1000);  // Just to prevent watchdog timeout
}
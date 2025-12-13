#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// CONFIGURACIÓN DEL PROYECTO PET RECYCLER
// =============================================================================
// Este archivo centraliza todas las constantes y configuraciones del proyecto
// Extraído de main.cpp para mejor organización y mantenimiento

// =============================================================================
// CONFIGURACIÓN DE PANTALLA OLED
// =============================================================================
#define SCREEN_WIDTH 128                    // OLED display width, in pixels
#define SCREEN_HEIGHT 64                    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C                 // OLED display I2C address

// Posiciones de texto en pantalla
#define LINE_X1       0                     // LCD text X offset
#define LINE_X2       95                    // LCD values X offset
#define LINE_Y1       4                     // Current temperature Y position
#define LINE_Y2       16                    // Set temperature Y position
#define LINE_Y3       28                    // Extruder speed Temperature Y position
#define LINE_Y4       40                    // Extrude Y position
#define STATUS_X      15                    // Status X position
#define STATUS_Y      52                    // Status Y position

// =============================================================================
// CONFIGURACIÓN DE TEMPERATURA
// =============================================================================
#define initialTemp   220                   // Temperatura inicial por defecto (°C)
#define minTemp       180                   // Temperatura mínima permitida (°C)
#define maxTemp       245                   // Temperatura máxima permitida (°C)
#define tempTolerance 10                    // Tolerancia de temperatura (°C)

// =============================================================================
// CONFIGURACIÓN DE MOTOR
// =============================================================================
#define minSpeed      1                     // Velocidad mínima del motor
#define maxSpeed      25                    // Velocidad máxima del motor (limitada para evitar trabas)
#define stepsPerRevolution 200              // Pasos por revolución del motor

// Configuración comentada (no utilizada actualmente)
// #define initialMot    1                  // Configuración inicial del motor

// =============================================================================
// CONFIGURACIÓN DE TIMING (en milisegundos)
// =============================================================================
#define EventLoopTime    500                // Actualización de pantalla (ms) - Menos frecuente para reducir interrupciones
#define SensorLoopTime   250                // Lectura de sensores (ms) - Menos frecuente
#define MenuLoopTime     333                 // Navegación de menú (ms) - Balance entre respuesta y fluidez del motor
#define MotorLoopTime    2                  // Motor update frequency (ms) - High frequency for smooth stepping

// =============================================================================
// CONFIGURACIÓN DE TERMISTOR
// =============================================================================
#define THERMISTOR_NOMINAL    100000.0      // Resistencia a 25°C (Ohms)
#define TEMPERATURE_NOMINAL   25.0          // Temperatura nominal (°C)
#define B_COEFFICIENT         3950.0        // Coeficiente Beta del termistor
#define SERIES_RESISTOR       100000.0      // Resistor pull-up (Ohms)
#define NUM_SAMPLES           10            // Muestras para promedio
#define ADC_MAX               4095.0        // ADC de 12 bits (ESP32)

// =============================================================================
// CONFIGURACIÓN DE CONTROL PID (comentadas - no implementadas)
// =============================================================================
// Estas constantes están comentadas en main.cpp pero se incluyen para referencia
// #define PID_KP               80.0         // Constante proporcional
// #define PID_KI               35.0         // Constante integral  
// #define PID_KD               80.0         // Constante derivativa
// #define PWM_PIN              9            // Pin PWM para calentador

// =============================================================================
// CONFIGURACIÓN DE ENCODER
// =============================================================================
#define ENC_INCREMENT        1              // Incremento del encoder por paso
#define ENCODER_DEBOUNCE     50             // Debounce del botón (ms)
#define ENCODER_READ_INTERVAL 5             // Intervalo de lectura encoder (ms)

// =============================================================================
// CONFIGURACIÓN DE MOTOR PASO A PASO (AsyncStepperLib)
// =============================================================================
#define MOTOR_DEFAULT_SPEED  5              // Velocidad inicial del motor
#define MOTOR_MAX_SPEED      2000           // Máxima velocidad en pasos/segundo
#define MOTOR_MIN_SPEED      50             // Mínima velocidad en pasos/segundo
#define MOTOR_ACCELERATION   1000           // Aceleración en pasos/segundo²
#define MOTOR_DIRECTION_SETTLE_TIME 100     // Tiempo de estabilización dirección (μs)

#endif // CONFIG_H
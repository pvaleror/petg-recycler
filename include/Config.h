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
#define minTemp       200                   // Temperatura mínima permitida (°C)
#define maxTemp       260                   // Temperatura máxima permitida (°C)
#define tempTolerance 10                    // Tolerancia de temperatura (°C)

// =============================================================================
// CONFIGURACIÓN DE MOTOR
// =============================================================================
#define minSpeed      1                     // Velocidad mínima del motor
#define maxSpeed      15                    // Velocidad máxima del motor (limitada para evitar trabas)
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
#define ENCODER_READ_INTERVAL 100             // Intervalo de lectura encoder (ms)

// =============================================================================
// CONFIGURACIÓN DE FREERTOS (TAREAS Y PRIORIDADES)
// =============================================================================
#define MOTOR_TASK_INTERVAL     1           // Motor task interval (ms) - Ultra high frequency
#define ENCODER_TASK_INTERVAL   10          // Encoder task interval (ms) - Fast response
#define MENU_TASK_INTERVAL      50          // Menu task interval (ms) - UI updates
#define SENSOR_TASK_INTERVAL    100         // Sensor task interval (ms) - Temperature monitoring
#define DISPLAY_TASK_INTERVAL   200         // Display task interval (ms) - Screen updates

// Prioridades FreeRTOS (mayor número = mayor prioridad)
#define MOTOR_TASK_PRIORITY     5           // CRÍTICO - Motor fluido
#define ENCODER_TASK_PRIORITY   4           // IMPORTANTE - Input capture
#define MENU_TASK_PRIORITY      3           // MEDIO - UI responsiva
#define SENSOR_TASK_PRIORITY    2           // BAJO - Monitoreo
#define DISPLAY_TASK_PRIORITY   1           // MUY BAJO - Actualización visual

// Stack sizes para cada task
#define MOTOR_TASK_STACK        2048        // Stack para motor task
#define ENCODER_TASK_STACK      2048        // Stack para encoder task
#define MENU_TASK_STACK         2048        // Stack para menu task
#define SENSOR_TASK_STACK       2048        // Stack para sensor task
#define DISPLAY_TASK_STACK      3072        // Stack para display task (más memoria)

// =============================================================================
// CONFIGURACIÓN DE MOTOR PASO A PASO (FreeRTOS + Ramping Optimized)
// =============================================================================
#define MOTOR_DEFAULT_SPEED  5              // Velocidad inicial del motor
#define MOTOR_DIRECTION_SETTLE_TIME 100     // Tiempo de estabilización dirección (μs)

// Velocidades en steps/segundo (más intuitivo que microsegundos)
#define MOTOR_MAX_SPEED_STEPS_SEC   800    // Velocidad máxima (steps/s)
#define MOTOR_MIN_SPEED_STEPS_SEC   20      // Velocidad mínima (steps/s)
#define MOTOR_DEFAULT_SPEED_STEPS   200     // Velocidad inicial (steps/s)

// Ramping y aceleración (para movimiento suave)
#define MOTOR_ACCELERATION_STEPS_S2 1000    // Aceleración (steps/s²)
#define MOTOR_DECELERATION_STEPS_S2 1000    // Desaceleración (steps/s²)
#define MOTOR_RAMP_UPDATE_INTERVAL  1       // Actualizar ramping cada 1ms

// Timing de mensajes de estado
#define MOTOR_STATUS_UPDATE_INTERVAL 500    // Intervalo de actualización de mensajes (ms)

// Encoder debounce (para interrupciones)
#define ENCODER_DEBOUNCE_MICROS     2000    // Debounce encoder (μs)
#define BUTTON_DEBOUNCE_MILLIS      50      // Debounce botón (ms)

#endif // CONFIG_H
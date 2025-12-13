#ifndef TYPES_H
#define TYPES_H

// Menu states
enum MenuState {
    MENU_TEMPERATURE,    // Configurar temperatura objetivo
    MENU_SPEED,          // Configurar velocidad del motor
    MENU_EXTRUDE,        // Control de extrusión
    MENU_STATUS          // Estado del sistema
};

// System states
enum SystemState {
    SYSTEM_IDLE,         // Sistema en reposo
    SYSTEM_READY,        // Listo para extruir
    SYSTEM_EXTRUDING,    // Extruyendo
    SYSTEM_ERROR         // Error de seguridad
};

// Motor states
enum MotorState { 
    STOPPED, 
    MOVING_FWD, 
    MOVING_REV 
};

// Motor direction constants
enum MotorDirection {
    MOTOR_REVERSE = 0,
    MOTOR_OFF = 1,
    MOTOR_FORWARD = 2
};

#endif // TYPES_H
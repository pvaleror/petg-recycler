# Referencia de Defines del Proyecto PET Recycler

Este documento describe todos los `#define` extraídos del archivo `main.cpp` y organizados en `include/Config.h`.

## Configuración de Pantalla OLED

| Define | Valor | Descripción |
|--------|-------|-------------|
| `SCREEN_WIDTH` | 128 | Ancho de la pantalla OLED en píxeles |
| `SCREEN_HEIGHT` | 64 | Alto de la pantalla OLED en píxeles |
| `SCREEN_ADDRESS` | 0x3C | Dirección I2C de la pantalla OLED |

### Posiciones de Texto en Pantalla

| Define | Valor | Descripción |
|--------|-------|-------------|
| `LINE_X1` | 0 | Offset X para etiquetas de texto |
| `LINE_X2` | 95 | Offset X para valores numéricos |
| `LINE_Y1` | 4 | Posición Y para temperatura actual |
| `LINE_Y2` | 16 | Posición Y para temperatura objetivo |
| `LINE_Y3` | 28 | Posición Y para velocidad de extrusión |
| `LINE_Y4` | 40 | Posición Y para control de extrusión |
| `STATUS_X` | 15 | Posición X para mensajes de estado |
| `STATUS_Y` | 52 | Posición Y para mensajes de estado |

## Configuración de Temperatura

| Define | Valor | Descripción |
|--------|-------|-------------|
| `initialTemp` | 220 | Temperatura inicial por defecto (°C) |
| `minTemp` | 180 | Temperatura mínima permitida (°C) |
| `maxTemp` | 245 | Temperatura máxima permitida (°C) |
| `tempTolerance` | 10 | Tolerancia de temperatura (°C) |

### Notas sobre Temperatura
- Rango operativo: 180°C - 245°C
- Temperatura por defecto optimizada para PET (220°C)
- Tolerancia de ±10°C para control PID

## Configuración de Motor

| Define | Valor | Descripción |
|--------|-------|-------------|
| `minSpeed` | 1 | Velocidad mínima del motor |
| `maxSpeed` | 25 | Velocidad máxima del motor |
| `stepsPerRevolution` | 200 | Pasos por revolución (motor típico 1.8°) |

### Notas sobre Motor
- Rango de velocidad: 1-25 (unidades arbitrarias)
- Motor paso a paso estándar de 200 pasos/revolución
- Control de dirección: 0=Reversa, 1=Parado, 2=Adelante

## Defines Comentados/No Utilizados

| Define | Valor | Estado |
|--------|-------|--------|
| `initialMot` | 1 | Comentado - no utilizado actualmente |

## Configuración de Timing

| Define | Valor | Descripción |
|--------|-------|-------------|
| `EventLoopTime` | 500 | Actualización de pantalla (ms) |
| `SensorLoopTime` | 100 | Lectura de sensores (ms) |
| `MenuLoopTime` | 10 | Navegación de menú (ms) |

## Configuración de Termistor

| Define | Valor | Descripción |
|--------|-------|-------------|
| `THERMISTOR_NOMINAL` | 100000.0 | Resistencia a 25°C (Ohms) |
| `TEMPERATURE_NOMINAL` | 25.0 | Temperatura nominal (°C) |
| `B_COEFFICIENT` | 3950.0 | Coeficiente Beta del termistor |
| `SERIES_RESISTOR` | 100000.0 | Resistor pull-up (Ohms) |
| `NUM_SAMPLES` | 10 | Muestras para promedio |
| `ADC_MAX` | 4095.0 | ADC de 12 bits (ESP32) |

## Configuración de Encoder

| Define | Valor | Descripción |
|--------|-------|-------------|
| `ENC_INCREMENT` | 1 | Incremento del encoder por paso |
| `ENCODER_DEBOUNCE` | 50 | Debounce del botón (ms) |
| `ENCODER_READ_INTERVAL` | 5 | Intervalo de lectura encoder (ms) |

## Configuración de Motor Paso a Paso

| Define | Valor | Descripción |
|--------|-------|-------------|
| `MOTOR_DEFAULT_SPEED` | 5 | Velocidad inicial del motor |
| `MOTOR_STEP_INTERVAL_MIN` | 500 | Intervalo mínimo entre pasos (μs) |
| `MOTOR_STEP_INTERVAL_MAX` | 25000 | Intervalo máximo entre pasos (μs) |
| `MOTOR_DIRECTION_SETTLE_TIME` | 100 | Tiempo de estabilización dirección (μs) |

## Uso en el Código

Para usar estas configuraciones en otros archivos:

```cpp
#include <Config.h>

// Ejemplo de uso
display.setCursor(LINE_X1, LINE_Y1);
if (temperature > maxTemp) {
    // Manejar sobrecalentamiento
}
```

## Configuración PID (Comentada)

Estas constantes están comentadas en el código pero incluidas para referencia futura:

| Define | Valor | Descripción |
|--------|-------|-------------|
| `PID_KP` | 80.0 | Constante proporcional (comentada) |
| `PID_KI` | 35.0 | Constante integral (comentada) |
| `PID_KD` | 80.0 | Constante derivativa (comentada) |
| `PWM_PIN` | 9 | Pin PWM para calentador (comentada) |

## Modificaciones Realizadas

1. **Centralización Completa**: Todos los `#define` y constantes movidos de `main.cpp` a `include/Config.h`
2. **Organización por Funcionalidad**: Agrupados en secciones (pantalla, temperatura, motor, timing, etc.)
3. **Conversión de const a #define**: Las constantes de timing convertidas para consistencia
4. **Documentación Mejorada**: Comentarios descriptivos para cada constante
5. **Mantenibilidad**: Fácil modificación de parámetros desde un solo archivo centralizado
6. **Configuración de Hardware**: Incluye parámetros de termistor, motor y encoder

## Recomendaciones

- Considerar convertir algunas constantes a variables configurables en runtime
- Añadir validación de rangos para valores críticos de temperatura
- Implementar configuración persistente en EEPROM/NVS para parámetros de usuario
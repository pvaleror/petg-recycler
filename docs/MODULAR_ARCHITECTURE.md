# Arquitectura Modular del PET Recycler

## 🏗️ Estructura del Proyecto Refactorizado

El proyecto ha sido completamente refactorizado de un archivo monolítico (`main.cpp` de 700+ líneas) a una arquitectura modular con separación clara de responsabilidades.

## 📁 Estructura de Archivos

```
├── include/
│   ├── Config.h           # Configuración centralizada
│   ├── Pins.h            # Definición de pines
│   ├── Motor.h           # Control de motor paso a paso
│   ├── Thermistor.h      # Lectura de temperatura
│   ├── Display.h         # Manejo de pantalla OLED
│   ├── Menu.h            # Sistema de menús
│   ├── Encoder.h         # Encoder rotativo
│   └── SystemControl.h   # Control general del sistema
├── src/
│   ├── main_new.cpp      # Archivo principal simplificado
│   ├── Motor.cpp         # Implementación control motor
│   ├── Thermistor.cpp    # Implementación termistor
│   ├── Display.cpp       # Implementación pantalla
│   ├── Menu.cpp          # Implementación menús
│   ├── Encoder.cpp       # Implementación encoder
│   └── SystemControl.cpp # Implementación control sistema
└── docs/
    ├── MODULAR_ARCHITECTURE.md
    └── DEFINES_REFERENCE.md
```

## 🔧 Módulos del Sistema

### 1. **Motor Module** (`Motor.h/cpp`)
**Responsabilidad**: Control completo del motor paso a paso

**Funcionalidades**:
- Control de dirección (adelante/atrás/parado)
- Control de velocidad variable
- Habilitación/deshabilitación del motor
- Manejo de estados de movimiento
- Timing preciso de pasos
- Mensajes de estado

**API Principal**:
```cpp
motor.begin();                    // Inicialización
motor.setDirection(true);         // true=adelante, false=atrás
motor.setSpeed(15);              // Velocidad 1-25
motor.enable(true);              // Habilitar motor
motor.update();                  // Llamar en loop principal
```

### 2. **Thermistor Module** (`Thermistor.h/cpp`)
**Responsabilidad**: Lectura y procesamiento de temperatura

**Funcionalidades**:
- Lectura real del termistor con ecuación Steinhart-Hart
- Modo simulación para desarrollo/testing
- Promediado de múltiples muestras
- Verificación de rangos de seguridad
- Actualización periódica automática

**API Principal**:
```cpp
thermistor.begin();                        // Inicialización
double temp = thermistor.readTemperatureC(); // Leer temperatura
bool safe = thermistor.checkTemperature(220, 10); // Verificar rango
thermistor.setSimulationMode(false);       // Modo real/simulación
```

### 3. **Display Module** (`Display.h/cpp`)
**Responsabilidad**: Manejo completo de la pantalla OLED

**Funcionalidades**:
- Inicialización de pantalla SSD1306
- Gestión de layout y posicionamiento
- Mostrar etiquetas de menú con indicadores
- Mostrar valores numéricos
- Mensajes de estado y error

**API Principal**:
```cpp
display.begin();                           // Inicialización
display.showLabels(currentMenu);          // Mostrar menú
display.showValues(temp, setpoint, speed, dir); // Mostrar valores
display.showStatus("Loading...");         // Mensaje de estado
```

### 4. **Menu Module** (`Menu.h/cpp`)
**Responsabilidad**: Sistema de navegación y estados

**Funcionalidades**:
- Navegación entre menús (Temperatura/Velocidad/Extrusión)
- Gestión de estados del sistema
- Actualización de parámetros
- Coordinación entre módulos
- Lógica de interfaz de usuario

**API Principal**:
```cpp
menu.begin();                             // Inicialización
menu.navigate();                          // Cambiar menú
menu.updateParameters(encoderValue);      // Actualizar parámetros
menu.updateDisplay();                     // Refrescar pantalla
```

### 5. **Encoder Module** (`Encoder.h/cpp`)
**Responsabilidad**: Manejo del encoder rotativo y botón

**Funcionalidades**:
- Decodificación cuadratura del encoder
- Debounce del botón
- Límites automáticos según menú activo
- Integración con sistema Eventually
- Callbacks para eventos

**API Principal**:
```cpp
encoder.begin();                          // Inicialización
encoder.setLimits(180, 245);             // Límites min/max
int value = encoder.getCurrentValue();    // Valor actual
encoder.updateLimitsForMenu();           // Actualizar según menú
```

### 6. **SystemControl Module** (`SystemControl.h/cpp`)
**Responsabilidad**: Coordinación general y seguridad

**Funcionalidades**:
- Inicialización secuencial de todos los módulos
- Coordinación de eventos Eventually
- Verificaciones de seguridad
- Manejo de errores críticos
- Bucle principal del sistema

**API Principal**:
```cpp
systemControl.begin();                   // Inicialización completa
systemControl.update();                 // Actualización periódica
bool safe = systemControl.checkTemperatureSafety(); // Verificar seguridad
```

## 🔄 Flujo de Ejecución

### Inicialización (setup):
1. `SystemControl::begin()` coordina la inicialización
2. Cada módulo se inicializa en orden específico
3. Se configuran los listeners de Eventually
4. Sistema queda listo para operación

### Operación (loop):
1. **Eventually** maneja todos los eventos temporales
2. **Encoder** detecta rotación y botones
3. **Menu** procesa navegación y parámetros
4. **Motor** ejecuta movimientos
5. **Thermistor** lee temperatura
6. **Display** actualiza pantalla
7. **SystemControl** verifica seguridad

## ✅ Beneficios de la Refactorización

### **Mantenibilidad**
- Cada módulo tiene una responsabilidad clara
- Fácil localizar y corregir problemas
- Cambios aislados no afectan otros módulos

### **Testabilidad**
- Cada módulo se puede probar independientemente
- Interfaces bien definidas
- Modo simulación para desarrollo

### **Escalabilidad**
- Fácil añadir nuevas funcionalidades
- Módulos reutilizables en otros proyectos
- Arquitectura preparada para crecimiento

### **Legibilidad**
- Código organizado y bien documentado
- Nombres descriptivos y consistentes
- Separación clara de conceptos

### **Colaboración**
- Diferentes desarrolladores pueden trabajar en módulos distintos
- Interfaces estables entre módulos
- Menos conflictos de merge

## 🚀 Migración del Código Anterior

Para migrar del `main.cpp` anterior al nuevo sistema modular:

1. **Reemplazar** `src/main.cpp` con `src/main_new.cpp`
2. **Compilar** el proyecto para verificar que todos los módulos se enlazan correctamente
3. **Probar** cada funcionalidad individualmente
4. **Ajustar** parámetros en `Config.h` según sea necesario

## 🔧 Configuración y Personalización

Toda la configuración está centralizada en `include/Config.h`:
- Parámetros de temperatura
- Configuración de motor
- Timing del sistema
- Parámetros de pantalla
- Configuración de hardware

## 📝 Próximos Pasos

1. **Implementar control PID** real para temperatura
2. **Añadir persistencia** de configuración en NVS
3. **Implementar logging** para diagnóstico
4. **Añadir más verificaciones** de seguridad
5. **Crear tests unitarios** para cada módulo

Esta arquitectura modular proporciona una base sólida para el desarrollo continuo del proyecto PET Recycler.
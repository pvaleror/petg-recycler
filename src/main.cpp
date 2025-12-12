//LCD Wiring diagram & example https://www.electronicshub.org/interfacing-128x64-graphical-lcd-with-arduino/
//LCD 12864 schema https://reprap.org/mediawiki/images/5/51/RRD_FULL_GRAPHIC_SMART_CONTROLER_SCHEMATIC.pdf

//Project diagram https://www.the-diy-life.com/wp-content/uploads/2023/07/Schematic_Filament-Recycler-Uno_2023-08-31.png
//Project original post https://www.the-diy-life.com/pet-bottle-recycler-part-1-using-an-arduino-uno-r4-to-control-a-3d-printers-hotend/
//project blog 1 https://www.the-diy-life.com/pet-bottle-recycler-part-1-using-an-arduino-uno-r4-to-control-a-3d-printers-hotend/
//project blog 2 https://www.the-diy-life.com/i-turned-pet-bottles-into-a-3d-printed-pi-case-pet-bottle-recycler-part-2/

//original code https://github.com/mklements/PETRecyclerController/blob/main/PET2Print/PET2Print.ino

//therm custom1 https://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/
//therm custom2 https://www.build-electronic-circuits.com/arduino-thermistor/
//therm default https://github.com/miguel5612/Arduino-ThermistorLibrary

//rotary encoder usage https://europe1.discourse-cdn.com/arduino/original/4X/f/2/f/f2f7a2eab6425ac7d18267c07ffb004ae11b776d.jpeg



//Includes
#include <Arduino.h>                        //Project libraries includes
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Eventually.h>
#include <Pins.h>

// Eventually instance
EvtManager mgr;




//#include <AsyncStepper.hpp>




//#include <U8g2lib.h>                        //screen libraries
//#ifdef U8X8_HAVE_HW_SPI
//  #include <SPI.h>
//#endif
//#ifdef U8X8_HAVE_HW_I2C
//  #include <Wire.h>
//#endif
//#include <AccelStepper.h>
//#include <PID_v1.h>
//#include <thermistor.h>                     //thermistor libraries
//#include <SmoothThermistor.h>

//const int stepper_steps = 200;

//AsyncStepper stepper1(stepper_steps, motStepPin, motDirPin);


#define LINE_X1       0                           //LCD text X offset
#define LINE_X2       95                         //LCD values X offset
#define LINE_Y1       4                          //Current temperature Y position
#define LINE_Y2       16                          //set temperature Y position
#define LINE_Y3       28                          //Extruder speed Temperature Y position
#define LINE_Y4       40                          //Extrude Y position
#define STATUS_X      15                         //Status X position
#define STATUS_Y      52                         //Status Y position
//#define initialMot    1

//Variable startup values
#define initialTemp   220                     //Define parameters for each edjustable setting
#define minTemp       180                     
#define maxTemp       245
#define minSpeed      1
#define maxSpeed      25
#define tempTolerance 10

// Motor control variables
unsigned long lastStepTime = 0;
unsigned long stepInterval = 1000;           // Default step interval in microseconds
bool motorEnabled = false;
int motSpeed = 5;                            // Current motor speed (1-25)
int motDir = 1;                              // 2=Fwd, 0=Rev, 1=Off
bool stepState = false;                      // Track STEP output state

// Forward declarations
void setMotorDirection(bool forward);
void enableMotor(bool enable);

// Add near other motor variables (around line 84)
enum MotorState { STOPPED, MOVING_FWD, MOVING_REV };
volatile MotorState motorState = STOPPED;
bool directionPending = false;
bool pendingForward = false;

// Motor control function implementations
void enableMotor(bool enable) {
    // Drive enable pin according to polarity
    digitalWrite(enablePin, EN_ACTIVE_LOW ? !enable : enable);
    motorEnabled = enable;
    if (!enable) {
        digitalWrite(motStepPin, LOW);       // Ensure step pin is low when disabled
    } else {
        // Kick-start stepping timing when enabling
        stepState = false;
        digitalWrite(motStepPin, LOW);
        lastStepTime = micros();
    }
}

void setMotorDirection(bool forward) {
  if (motorState != STOPPED) {
      directionPending = true;
      pendingForward = forward;
      return;
  }
  
  digitalWrite(motDirPin, forward ? HIGH : LOW);
  delayMicroseconds(100); // Increased settling time
  motDir = forward ? 2 : 0;
  motorState = forward ? MOVING_FWD : MOVING_REV;
}

void setMotorSpeed(int speed) {
    // Convert speed (1-25) to step interval in microseconds
    // Faster speed = smaller interval
    motSpeed = constrain(speed, minSpeed, maxSpeed);
    // Invert mapping so higher speed -> smaller interval
    stepInterval = map(motSpeed, minSpeed, maxSpeed, 2000, 200);
    //if (stepInterval < 200) stepInterval = 200; // clamp to a sane minimum
}

void updateMotor() {
  if (!motorEnabled) return;

  // Handle pending direction changes
  if (directionPending && motorState == STOPPED) {
      digitalWrite(motDirPin, pendingForward ? HIGH : LOW);
      delayMicroseconds(100);
      motDir = pendingForward ? 2 : 0;
      motorState = pendingForward ? MOVING_FWD : MOVING_REV;
      directionPending = false;
  }

  unsigned long currentMicros = micros();
  if (currentMicros - lastStepTime >= stepInterval) {
      if (stepState) {
          digitalWrite(motStepPin, LOW);
          motorState = STOPPED; // Mark completion of step
      } else {
          digitalWrite(motStepPin, HIGH);
          motorState = (motDir == 2) ? MOVING_FWD : MOVING_REV;
      }
      stepState = !stepState;
      lastStepTime = currentMicros;
  }
}

void stopMotor() {
    enableMotor(false);
    digitalWrite(motStepPin, LOW);
}

void setupMotor() {
    motSpeed = 5;
    pinMode(motStepPin, OUTPUT);
    pinMode(motDirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    
    // Initialize motor in disabled state
    digitalWrite(motStepPin, LOW);
    // Set initial direction pin according to current motDir if not Off
    if (motDir != 1) {
        digitalWrite(motDirPin, motDir == 2 ? HIGH : LOW);
    } else {
        digitalWrite(motDirPin, HIGH); // default idle state
    }
    // Set disabled state according to EN polarity
    digitalWrite(enablePin, EN_ACTIVE_LOW ? HIGH : LOW);
    
    // Set default speed and direction
    setMotorSpeed(motSpeed);
}



// Menu and System States
enum MenuState {
  MENU_TEMPERATURE,    // Configurar temperatura objetivo
  MENU_SPEED,          // Configurar velocidad del motor
  MENU_EXTRUDE,        // Control de extrusión
  MENU_STATUS          // Estado del sistema
};

enum SystemState {
  SYSTEM_IDLE,         // Sistema en reposo
  SYSTEM_READY,        // Listo para extruir
  SYSTEM_EXTRUDING,    // Extruyendo
  SYSTEM_ERROR         // Error de seguridad
};

// Global state variables
MenuState currentMenu = MENU_TEMPERATURE;
SystemState systemState = SYSTEM_IDLE;
int dataInputNo = 0;                        //Data input tracking, 0 - temp, 1 - speed, 2 - motor
// Motor timing variables
unsigned long currentTime, rotaryTime, displayTime, motorTime = 0;
const unsigned long EventLoopTime = 500;
const unsigned long SensorLoopTime = 100;
const unsigned long MenuLoopTime = 10;


//Encoder rotary setup
int encCurrent = initialTemp;
int encIncrement = 1;
int encLowLim = minTemp;                    //Variables to store the encoder limits and increment
int encHighLim = maxTemp;
double setpoint = initialTemp;               //Define PID variables & establish PID loop

// Encoder state tracking (for Eventually)
static int lastEncoderA = HIGH;
static int lastEncoderB = HIGH;



#define SCREEN_WIDTH 128                    //OLED display width, in pixels
#define SCREEN_HEIGHT 64                    //OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C  // OLED display I2C address
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void enableMotor(bool enable);
void updateMotor();
void stopMotor();
void setupMotor();

double input = 25.0; // Simulated temperature
double output;
// Track last values drawn to the OLED to prevent unnecessary redraws
int lastTempDisplayed = -10000;

/*




//const int temperaturePin = A2;              //Thermistor digital pin
const int pwmPin = 9;                       //Mosfet pin
                   

double Kp = 80.0;                            //Define PID constants
double Ki = 35.0;
double Kd = 80.0;
unsigned long thermalTime = 0;
//thermistor (A2,0);         //Connect thermistor on A2
*/
//SmoothThermistor smoothThermistor(A2,              // the analog pin to read from
//                                   ADC_SIZE_10_BIT, // the ADC size
//                                   100000,           // the nominal resistance
//                                   102000,           // the series resistance
//                                   3950,            // the beta coefficient of the thermistor
//                                   25,              // the temperature for nominal resistance
//                                   10);             // the number of samples to take for each measurement
///*
////PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);
//int motMaxDelay = 100;
//*/
////int motSpeed = initialSpeed;                 //Define motor parameters
////int motDelayTime = 1000;
//
////AccelStepper motor(1, motStepPin, motDirPin);
//
//
//U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, LCDE, LCDRW, LCDRS); //LCD Full buffer CONSTRUCTOR
////U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, LCDE, LCDRW, LCDRS); //LCD page buffer CONSTRUCTOR
//
////OK //Increment the current setting if the rotary encoder is turned
//

// Thermistor constants (adjust to your components)
const double THERMISTOR_NOMINAL = 100000.0;   // Resistance at 25C (Ohms)
const double TEMPERATURE_NOMINAL = 25.0;      // 25C
const double B_COEFFICIENT = 3950.0;          // Beta coefficient
const double SERIES_RESISTOR = 100000.0;      // Pull-up resistor value (Ohms)
const int NUM_SAMPLES = 10;                   // Averaging samples
const double ADC_MAX = 4095.0;                // 12-bit ADC

double readThermistorC(){
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(THERM_PIN);
    delay(2);
  }
  double reading = (double)sum / (double)NUM_SAMPLES;
  if (reading <= 0.5) return -273.15; // avoid div by zero, return min

  double ratio = reading / ADC_MAX;                 // V/VDDA
  if (ratio >= 0.9995) ratio = 0.9995;              // clamp
  if (ratio <= 0.0005) ratio = 0.0005;

  double resistance = SERIES_RESISTOR * (ratio / (1.0 - ratio));

  double steinhart;
  steinhart = resistance / THERMISTOR_NOMINAL;      // (R/R0)
  steinhart = log(steinhart);                       // ln(R/R0)
  steinhart /= B_COEFFICIENT;                       // 1/B * ln(R/R0)
  steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);// + 1/T0
  steinhart = 1.0 / steinhart;                      // Invert
  steinhart -= 273.15;                              // K -> C
  return steinhart;
}

// Temperature check function (simplified)
boolean checkTemp() {
  // Always return true for demo
  return true;
}

// Improved parameter update function
void updateParams() {
  // Update the parameter being edited based on current menu state
  switch (currentMenu) {
    case MENU_TEMPERATURE:
      setpoint = encCurrent;
      break;
      
    case MENU_SPEED:
      // Apply speed and recompute step interval
      setMotorSpeed(encCurrent);
      break;

    case MENU_EXTRUDE:
      // encCurrent maps to: 0=Rev, 1=Off, 2=Fwd
      encCurrent = constrain(encCurrent, 0, 2);
      motDir = encCurrent;
      if (motDir == 1) {
        // Off
        enableMotor(false);
        motorState = STOPPED;
      } else {
        // Set direction then enable
        setMotorDirection(motDir == 2);
        enableMotor(true);
      }
      break;
      
    case MENU_STATUS:
      // Status menu is read-only, no parameter updates
      break;
  }

  

}

// Motor control function (disabled)
/*
void updateMotorState() {
  switch (motDir) {
    case 0: // Reverse
      systemState = SYSTEM_EXTRUDING;
      pinMode(enablePin, OUTPUT);
      stepper1.RotateAngle(270.0f, AsyncStepper::CCW);
      break;
      
    case 2: // Forward
      if (checkTemp()) {
        systemState = SYSTEM_EXTRUDING;
        pinMode(enablePin, OUTPUT);
        stepper1.RotateAngle(270.0f, AsyncStepper::CW);
      } else {
        systemState = SYSTEM_ERROR;
        pinMode(enablePin, INPUT);
        stepper1.State = AsyncStepper::Stopped;
      }
      break;
      
    default: // Stop (1)
      systemState = SYSTEM_IDLE;
      pinMode(enablePin, INPUT);
      stepper1.State = AsyncStepper::Stopped;
      break;
  }
}
*/


// Encoder reading function for Eventually
// Reads encoder state and updates encCurrent based on quadrature decoding
bool readEncoder(EvtListener *lstn) {
  int currentA = digitalRead(pinA);
  int currentB = digitalRead(pinB);
  
  // Detect falling edge on pinA (encoder rotation)
  if (currentA == LOW && lastEncoderA == HIGH) {
    // Determine direction based on pinB state
    if (currentB == HIGH) {
      // Clockwise rotation
      if (encCurrent < encHighLim) {
        encCurrent += encIncrement;
        updateParams();
      }
    } else {
      // Counter-clockwise rotation
      if (encCurrent > encLowLim) {
        encCurrent -= encIncrement;
        updateParams();
      }
    }
  }
  
  lastEncoderA = currentA;
  lastEncoderB = currentB;
  return false; // Continue event chain
}
////OK
void setStatus(const char *status){
  display.setCursor(STATUS_X, STATUS_Y);
  display.print(status);
}
//
////OK
// Update the motor direction and return status message
const char *getMotorDir(){
  static unsigned long lastUpdate = 0;
  const char *statusMessage = "";
  
  // Only update status message every 500ms to prevent flickering
  if (millis() - lastUpdate > 500) {
    if (motDir == 0) {
      statusMessage = "Unloading...";
    } else if (motDir == 2) {
      statusMessage = "Loading...";
    } else {
      statusMessage = "";
    }
    lastUpdate = millis();
  }
  
  // Set the status message if we're in the EXTRUDE menu
  if (currentMenu == MENU_EXTRUDE) {
    setStatus(statusMessage);
  }
  
  // Return the direction text
  if (motDir == 0) return "Rev";
  if (motDir == 2) return "Fwd";
  return "Off";
}

// Improved menu with proper alignment and selection indicator
void setLabels() {   
  // Header
  display.setCursor(LINE_X1, LINE_Y1);
  display.print(F(" Current Temp:"));
  
  // Menu items with proper spacing
  display.setCursor(LINE_X1, LINE_Y2);
  display.print(currentMenu == MENU_TEMPERATURE ? ">Set Temp:" : " Set Temp:");
  
  display.setCursor(LINE_X1, LINE_Y3);
  display.print(currentMenu == MENU_SPEED ? ">Extrude Speed:" : " Extrude Speed:");
  
  display.setCursor(LINE_X1, LINE_Y4);
  display.print(currentMenu == MENU_EXTRUDE ? ">Extrude:" : " Extrude:");
  
  // Update last menu state
  static MenuState lastMenu = MENU_TEMPERATURE;
  lastMenu = currentMenu;
}

void showValues(){
  int temp = input;
  int setpointInt = setpoint;
  
  // Always show temperature (even if not active)
  display.setCursor(LINE_X2, LINE_Y1);
  display.print(temp);
  display.print(F("C"));
  
  // Show setpoint
  display.setCursor(LINE_X2, LINE_Y2);
  display.print(setpointInt);
  display.print(F("C"));
  
  // Show speed
  display.setCursor(LINE_X2, LINE_Y3);
  display.print(motSpeed);
  

  
  // Show current menu value
  const char *motorDir = getMotorDir();
  display.setCursor(LINE_X2, LINE_Y4);
  display.print(motorDir);
}

// Improved display update function
void updateDataDisplay () {
  display.clearDisplay();                                 //Clear the buffer
  setLabels();
  showValues();
  display.display();    
}
///*
//

//

//
// Improved menu navigation
// Called directly from buttonPressAction when button is pressed
void navigateMenu() {
  // Cycle through menu states
  switch (currentMenu) {
    case MENU_TEMPERATURE:
      currentMenu = MENU_SPEED;
      dataInputNo = 1;
      encCurrent = motSpeed;
      encLowLim = minSpeed;
      encHighLim = maxSpeed;
      break;
      
    case MENU_SPEED:
      currentMenu = MENU_EXTRUDE;
      dataInputNo = 2;
      encCurrent = motDir;
      encLowLim = 0;
      encHighLim = 2;
      break;
      
    case MENU_EXTRUDE:
      currentMenu = MENU_TEMPERATURE;
      dataInputNo = 0;
      encCurrent = setpoint;
      encLowLim = minTemp;
      encHighLim = maxTemp;
      break;
  }
}

// Button press action for Eventually
// This is called when the button is released (LOW to HIGH transition with INPUT_PULLUP)

//
void checkInputs(){
  // Simulate temperature reading
  input = 25.0 + (millis() % 1000) / 100.0; // Simple demo value
  int tempInt = (int)input;
  if (tempInt != lastTempDisplayed) {
    lastTempDisplayed = tempInt;
  }
}
//
//
////OK
boolean updateValues = false;
// Motor function disabled
/*
void runMotor () {
  // Motor control code removed for encoder/screen only mode
}
*/
//



// Eventually action functions - must return bool
bool updateDisplayAction(EvtListener *lstn) {
  updateDataDisplay();
  return false; // Continue event chain
}

bool checkInputsAction(EvtListener *lstn) {
  checkInputs();
  return false; // Continue event chain
}

bool handleMenuAction(EvtListener *lstn) {
  currentTime = millis();
  updateMotor();
  return false; // Continue event chain
}

bool lastPressState = false;
bool buttonPressAction(EvtListener *lstn) {
  if (!lastPressState) {
    Serial.println(F(">>> MENU CHANGE <<<"));
    navigateMenu();
  }
  lastPressState = true;
  return false;
}

bool buttonReleaseAction(EvtListener *lstn) {
  lastPressState = false;
  return false;
}




//// OK
void setup() {
  Serial.begin(115200);                                    //Initialize serial communication (match monitor)
  delay(100);                                              // Wait for serial monitor
  
  Serial.println(F("Initializing OLED display..."));
  
  // Initialize I2C on ESP32-C3
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);                                              // Wait for I2C to stabilize
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("ERROR: SSD1306 allocation failed!"));
    for(;;); // Don't proceed, loop forever
  }
  
  Serial.println(F("OLED display initialized successfully!"));
  
  display.setTextSize(1);                                   //Set the text size
  display.setTextColor(SSD1306_WHITE);                      //Draw white text
  
  // Clear and show initial display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Initializing..."));
  display.display();
  delay(500);
    
    
  //  smoothThermistor.useAREF(false);
  //  
  //
  //  //pinMode(pwmPin, OUTPUT);                                //Configure PWM pin
  //  //pid.SetMode(AUTOMATIC);                                 //Set the PID parameters
  //  //pid.SetOutputLimits(0, 255);
  //  //input = therm1.analog2temp();                           //Read and set the initial input value
  pinMode(THERM_PIN, INPUT);
  analogReadResolution(12);


  //Encoder config
  pinMode(encButton, INPUT_PULLUP);                       //Set the encoder button as an input, pulled HIGH to the logic voltage
  pinMode(pinA, INPUT_PULLUP);                            //Set pinA as an input, pulled HIGH to the logic voltage
  pinMode(pinB, INPUT_PULLUP);                            //Set pinB as an input, pulled HIGH to the logic voltage
  
  // Initialize encoder state
  lastEncoderA = digitalRead(pinA);
  lastEncoderB = digitalRead(pinB);
  
  // Setup Eventually listeners for button and encoder
  // Button listener: detects button release (LOW to HIGH transition with INPUT_PULLUP)
  // Debounce delay of 50ms, triggers on HIGH (button release)
  mgr.addListener(new EvtPinListener(encButton, 50, HIGH, (EvtAction)buttonPressAction));
  mgr.addListener(new EvtPinListener(encButton, 1, LOW,  (EvtAction)buttonReleaseAction));
    
  // Encoder listener: reads encoder state periodically (every 5ms for responsive detection)
  mgr.addListener(new EvtTimeListener(5, true, (EvtAction)readEncoder));


  //lastEncoded = (digitalRead(pinA) << 1) | digitalRead(pinB);

  //Motor config
  setupMotor();
  currentTime = millis();     //Record the start time for the loop
//  //u8g2.setBufferedMode(false);
//
//  
//  
//  
  // Initialize menu and system states
  currentMenu = MENU_TEMPERATURE;
  systemState = SYSTEM_IDLE;
  dataInputNo = 0;
  encCurrent = setpoint;
  encLowLim = minTemp;
  encHighLim = maxTemp;

  

  //stepper1.SetSpeedRpm(motSpeed);
  //stepper1.SetAcceleration(100);
  //stepper1.State = AsyncStepper::Stopped;

	//stepper1.RotateContinuous(AsyncStepper::CCW);

  // Initialize Eventually - schedule periodic tasks using EvtTimeListener
  // Update display every EventLoopTime ms (500ms), continuously
  mgr.addListener(new EvtTimeListener(EventLoopTime, true, (EvtAction)updateDisplayAction));
  
  // Update inputs every SensorLoopTime ms (100ms), continuously
  mgr.addListener(new EvtTimeListener(SensorLoopTime, true, (EvtAction)checkInputsAction));
  
  // Handle menu navigation and motor updates frequently (every 10ms)
  mgr.addListener(new EvtTimeListener(MenuLoopTime, true, (EvtAction)handleMenuAction));

  
  Serial.println(F("Setup complete"));                       //Write to serial monitor to indicate the setup function is complete
  Serial.println(F("Display should now be showing content"));
}
//
//
//
////OK

#define stepsPerRevolution 200



// Use Eventually's event loop instead of traditional loop()
// This replaces the standard Arduino loop() function
USE_EVENTUALLY_LOOP(mgr)



//  //input = therm1.analog2temp(); //Read the temperature
//  
//  //input = .analog2tempEADC(1024); //Read the temperature
//
//  //pid.Compute();  //Compute the PID output  
//  //analogWrite(pwmPin, output);    //Update the PWM output
//  //int temp = input;
//  
//  //Print the temperature and PWM output
//  //Serial.print("Temperature: ");
//  //Serial.print(temp);
//  //Serial.print(" \u00B0C");
//  //Serial.print("\tPWM Output: ");
//  //Serial.print(output); 
//  //Serial.print("\tEncoder: ");
//  //Serial.println(encCurrent);
//
//  
//  //for(int x = 0; x < 600; x++) {
//  //  digitalWrite(motStepPin,HIGH); 
//  //  delayMicroseconds(1000); 
//  //  digitalWrite(motStepPin,LOW); 
//  //  delayMicroseconds(1000); 
//  //}
//  //if( millis() - currentTime >= loopTime ) {
//    //
//  //}
///*
//  if( millis() - currentTime >= loopTime ) {
//    updateDataDisplay();  //Update the OLED display
//    currentTime = millis();
//    
//    updateValues(); //Set the parameter being edited equal to the current encoder position
//  }*/
//  //while( millis() < currentTime + loopTime ) { //Check for input on the pushbutton
//    //if( )
//    //updateDataDisplay();  //Update the OLED display
//  //  byte buttonState = digitalRead (encButton);
//  //  if (buttonState != oldButtonState) {
//  //    if (millis() - buttonPressTime >= debounceTime) {    //Debounce button
//  //      buttonPressTime = millis ();                        //Time when button was pushed
//  //      oldButtonState =  buttonState;                      //Remember button state for next time
//  //      if (buttonState == LOW) {
//  //        pressed = true;
//  //      } else{
//  //        isPressed(); 
//  //      }
//  //    }
//  //  }
//  //  checkPushButton(); //Check for input on the pushbutton
//  //  updateValues(); //Set the parameter being edited equal to the current encoder position
//  //}

//}
///////////////////////////////////////
// FULL STATE MACHINE IMPLEMENTATION //
///////////////////////////////////////

#include "HX711.h"

// ------------------------------
// Load cell stuff 
// ------------------------------

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 7;
const int LOADCELL_SCK_PIN = 8;

unsigned long lastLoadCellRead = 0;

float liveForce = 0;
float peakBackdriveForce = 0; 
float peakPushForce = 0; 

const unsigned long LOADCELL_INTERVAL = 30;   // ms

const float SCALE_FACTOR = -40896.506; 

HX711 scale;



// ------------------------------
// L298N Motor Pins
// ------------------------------

// Motor 1
const int enA = 3;
const int in1 = 5;
const int in2 = 6;

// Motor 2
const int enB = 11  ;
const int in3 = 9;
const int in4 = 10;

// Potentiometers
const int pot1 = A5;
const int pot2 = A3;

// ------------------------------
// Stall detection parameters
// ------------------------------
const int STALL_LIMIT = 10;           // minimum pot change to count as movement
const unsigned long STALL_TIMEOUT = 3000;  // 3 seconds

// ------------------------------
// Motor state tracking
// ------------------------------
int testerMotorTarget = 0;
int helperMotorTarget = 0;
bool testerMotorActive = false;
bool helperMotorActive = false;
int testerMotorSpeed = 0; 
int helperMotorSpeed = 0; 

int lastPos1 = 0;
unsigned long lastMoveTime1 = 0;
bool firstRun1 = true;

int lastPos2 = 0;
unsigned long lastMoveTime2 = 0;
bool firstRun2 = true;

int backdriveInitialPos = 0; 
int backdriveFinalPos = 0; 

int BACKDRIVE_PWM = 125; 

String backdriveTestStatus = "UNTESTED"; 
String pushTestStatus = "UNTESTED"; 


// ------------------------------
// Serial command handling
// ------------------------------
String inputCommand = "";
bool commandReady = false;


bool stepInitialized = false;
// ------------------------------
// System State Machine
// ------------------------------
enum SystemState {
  IDLE,
  PUSH_TEST_STEP1,
  PUSH_TEST_STEP2,
  PUSH_TEST_STEP3, 
  BACKDRIVE_TEST_STEP1,
  BACKDRIVE_TEST_STEP2, 
  BACKDRIVE_TEST_STEP3, 
  BACKDRIVE_TEST_STEP4, 
  BACKDRIVE_TEST_STEP5, 
  UNLOAD_STEP_1, 
  UNLOAD_STEP_2, 
};

SystemState systemState = IDLE;

// ------------------------------
// Setup
// ------------------------------
void setup() {
  Serial.begin(9600);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  scale.tare();
  scale.set_scale(SCALE_FACTOR);

  // Motor pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Pot pins
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);

  stopMotor(1);
  stopMotor(2);

  Serial.println("Ready. Type commands: push test, backdrive test, STOP");
}

// ------------------------------
// Main loop
// ------------------------------
void loop() {

  readSerialCommand();
  handleCommand();


  // Update load cell
  updateLoadCell(); 

  // Update motors
  updatetesterMotor();
  updatehelperMotor(); 

  // Run sequence state machine
  runStateMachine();
}

// ------------------------------
// Read Serial command
// ------------------------------
void readSerialCommand() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputCommand.trim();
      commandReady = true;
      return;
    }
    inputCommand += c;
  }
}

// ------------------------------
// Handle Serial command
// ------------------------------
void handleCommand() {


  if (!commandReady) return;
  commandReady = false;

  Serial.println(inputCommand); 
  
  if (inputCommand == "push test") {
    systemState = PUSH_TEST_STEP1;
    Serial.println("Starting PUSH TEST");
  }
  else if (inputCommand == "backdrive test") {
    systemState = BACKDRIVE_TEST_STEP1;
    Serial.println("Starting BACKDRIVE TEST");
  }
  else if (inputCommand == "STOP") {
    stopMotor(1);
    stopMotor(2);
    testerMotorActive = helperMotorActive = false;
    systemState = IDLE;
    Serial.println("!! SYSTEM STOPPED !!");
  }else if(inputCommand == "UNLOAD" || inputCommand == "LOAD"){
    systemState =  UNLOAD_STEP_1; 
    Serial.println("Moving to LOAD/UNLOAD position");  
  }else if (inputCommand.substring(0, 18) == "SET_BACKDRIVE_PWM:") {
        BACKDRIVE_PWM = inputCommand.substring(18).toInt();
        Serial.print("BACKDRIVE_PWM_UPDATED:");
        Serial.println(BACKDRIVE_PWM);
  } else {
    Serial.println("Unknown command");
  }

  inputCommand = "";
}

// ------------------------------
// State machine sequences
// ------------------------------

void runStateMachine() {
    switch(systemState) { 

        /////////////////////////
        // PUSH TEST SEQUENCE //
        ////////////////////////

        case PUSH_TEST_STEP1:
            if (!stepInitialized) {
                helperMotorSpeed = 255; 
                helperMotorTarget = 0;
                helperMotorActive = true;
                stepInitialized = true;  // only do this once
                pushTestStatus = "UNTESTED"; 
                Serial.println("PUSH TEST STARTED");
            }

            if (!helperMotorActive) {  // motor finished
                systemState = PUSH_TEST_STEP2;
                stepInitialized = false; // reset for next step
            }
            break;
        
        case PUSH_TEST_STEP2:
            if (!stepInitialized) {
                testerMotorSpeed = 255; 
                testerMotorTarget = 750;
                testerMotorActive = true;
                stepInitialized = true;  // only do this once
            }

            if (!testerMotorActive) {  // motor finished
                systemState = PUSH_TEST_STEP3;
                stepInitialized = false; // reset for next step
            }

            
            if(liveForce > peakPushForce){
              peakPushForce = liveForce; 
            }
            
            break;

        case PUSH_TEST_STEP3:
            
            systemState = IDLE;
            
            Serial.println("PUSH_FORCE_FINAL: " + String(liveForce)); 
            Serial.println("PUSH_FORCE_PEAK: " + String(peakPushForce)); 

            if(peakPushForce > 60 && liveForce > 60 ){
              pushTestStatus = "PASS"; 
            }else{
               pushTestStatus = "FAIL"; 
            }

            Serial.println("PUSH TEST COMPLETED");
            Serial.println("PUSH_TEST_STATUS: " + pushTestStatus);
            
            peakPushForce = 0; 
            break; 



        /////////////////////////////
        // BACKDRIVE TEST SEQUENCE //
        /////////////////////////////

        case BACKDRIVE_TEST_STEP1:

            if (!stepInitialized) {
                  helperMotorSpeed = 255; 
                  helperMotorTarget = 0;
                  helperMotorActive = true;
                  stepInitialized = true;  // only do this once
            }

            if (!helperMotorActive) {  // motor finished
                  systemState = BACKDRIVE_TEST_STEP2;
                  stepInitialized = false; // reset for next step
            }
            
            break;
          
        case BACKDRIVE_TEST_STEP2:

          if (!stepInitialized) {
                testerMotorTarget = 700;
                testerMotorSpeed = 150; 
                testerMotorActive = true;
                stepInitialized = true;  // only do this once
          }

          if (!testerMotorActive) {  // motor finished
                systemState = BACKDRIVE_TEST_STEP3;
                stepInitialized = false; // reset for next step
                backdriveInitialPos = analogRead(pot1); 
                Serial.println("Starting Pot Value" + String(backdriveInitialPos)); 
          }
          
          break;
        
        case BACKDRIVE_TEST_STEP3:
                Serial.println("POT 2 VALUE" + String(analogRead(pot2))); 
                systemState = BACKDRIVE_TEST_STEP4;
                stepInitialized = false; // reset for next step       
          break;
        
        
        case BACKDRIVE_TEST_STEP4:

          if (!stepInitialized) {
                helperMotorSpeed = BACKDRIVE_PWM; 
                helperMotorTarget = 700;
                helperMotorActive = true;
                stepInitialized = true;  // only do this once
          }

          if (!helperMotorActive) {  // motor finished
                systemState = BACKDRIVE_TEST_STEP5;
                stepInitialized = false; // reset for next step
          }

          if(liveForce > peakBackdriveForce){
            peakBackdriveForce = liveForce; 
          }

          break;

        case BACKDRIVE_TEST_STEP5:
            systemState = IDLE; 

            backdriveFinalPos = analogRead(pot1); 
            Serial.println("Ending Pot Value" + String(backdriveFinalPos)); 


            Serial.println("BACKDRIVE_FORCE_PEAK: " + String(peakBackdriveForce)); 
            Serial.println("BACKDRIVE TEST COMPLETE"); 
            
            if(abs(backdriveFinalPos - backdriveInitialPos) > 20){
              backdriveTestStatus = "FAIL"; 
            }else{
              backdriveTestStatus = "PASS"; 
            }

            Serial.println("BACKDRIVE_TEST_STATUS: " + backdriveTestStatus);

            backdriveFinalPos = 0; 
            backdriveInitialPos = 0; 
            peakBackdriveForce = 0; 
            
            break;

        case UNLOAD_STEP_1:
            if (!stepInitialized) {
                helperMotorSpeed = 255; 
                helperMotorTarget = 0;
                helperMotorActive = true;
                stepInitialized = true;  // only do this once
                Serial.println("UNLOAD/LOAD STARTED");
            }

            if (!helperMotorActive) {  // motor finished
                systemState = UNLOAD_STEP_2;
                stepInitialized = false; // reset for next step
            }
            break;
        
        case UNLOAD_STEP_2:
            if (!stepInitialized) {
                testerMotorSpeed = 255; 
                testerMotorTarget = 0;
                testerMotorActive = true;
                stepInitialized = true;  // only do this once
            }

            if (!testerMotorActive) {  // motor finished
                Serial.println("UNLOAD/LOAD COMPLETE"); 
                systemState = IDLE;
                stepInitialized = false; // reset for next step
            }
            break;
        

        default:
            stepInitialized = false;
            break;
    }
}

// ------------------------------
// Update motor 1 (non-blocking)
// ------------------------------
void updatetesterMotor() {
  if (!testerMotorActive) return;

  int pos = analogRead(pot1);

  // Stall detection
  if (firstRun1) {
    lastPos1 = pos;
    lastMoveTime1 = millis();
    firstRun1 = false;
  }

  if (abs(pos - lastPos1) > STALL_LIMIT) {
    lastMoveTime1 = millis();
    lastPos1 = pos;
  }

  if (millis() - lastMoveTime1 > STALL_TIMEOUT) {
    stopMotor(1);
    testerMotorActive = false;
    firstRun1 = true;
    Serial.println("STALL M1");
    return;
  }

  // Normal position control
  if (abs(pos - testerMotorTarget) < 15) {
    stopMotor(1);
    testerMotorActive = false;
    firstRun1 = true;
    return;
  }

  if (pos < testerMotorTarget) {
    analogWrite(in3, testerMotorSpeed);
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(in3, LOW);
    analogWrite(in4, testerMotorSpeed);
  }

  analogWrite(enB, testerMotorSpeed);
}

// ------------------------------
// Update motor 2 (non-blocking)
// ------------------------------
void updatehelperMotor() {
  if (!helperMotorActive) return;

  int pos = analogRead(pot2);

  // Stall detection
  if (firstRun2) {
    lastPos2 = pos;
    lastMoveTime2 = millis();
    firstRun2 = false;
  }

  if (abs(pos - lastPos2) > STALL_LIMIT) {
    lastMoveTime2 = millis();
    lastPos2 = pos;
  }

  if (millis() - lastMoveTime2 > STALL_TIMEOUT) {
    stopMotor(2);
    helperMotorActive = false;
    firstRun2 = true;
    Serial.println("STALL M2");
    return;
  }

  // Normal position control
  if (abs(pos - helperMotorTarget) < 20) {
    stopMotor(2);
    helperMotorActive = false;
    firstRun2 = true;
    return;
  }

  if (pos < helperMotorTarget) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }

  analogWrite(enA, helperMotorSpeed);
}

// ------------------------------
// Stop motor helper
// ------------------------------
void stopMotor(int motor) {
  if (motor == 1) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enB, 0);
  }
  else if (motor == 2) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0);
  }
}


void updateLoadCell() {
    if (!scale.is_ready()) return;

    if (millis() - lastLoadCellRead >= LOADCELL_INTERVAL) {
        lastLoadCellRead = millis();
        liveForce = scale.get_units(3);   // faster reading
    }
}



///// FEATURES TO IMPLEMENT POST LUNCH 

// continous stream of load cell readings 
// add slide bar to UI for force control 
// improving logging to excel sheet 
// continue running test with 3 actuators 
// test without screws 
//////////////////////////////////
//// SIMPLE MOTOR TESTER CODE ////
//////////////////////////////////


// // Motor 1 (A)
// const int enA = 11;
// const int in1 = 10;
// const int in2 = 9;

// // Motor 2 (B)
// const int enB = 3;
// const int in3 = 6;
// const int in4 = 5;

// void setup() {
//   Serial.begin(9600);

//   pinMode(enA, OUTPUT);
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);

//   pinMode(enB, OUTPUT);
//   pinMode(in3, OUTPUT);
//   pinMode(in4, OUTPUT);

//   stopMotor(1);
//   stopMotor(2);

//   Serial.println("Enter command: (e.g. '1: forward', '2: backward')");
// }

// void loop() {


//   if (Serial.available()) {
//     String input = Serial.readStringUntil('\n');
//     input.trim();
//     input.toLowerCase();

//     int motorNum = -1;
//     String direction = "";

//     // Parse format "X: direction"
//     int colonIndex = input.indexOf(':');
//     if (colonIndex != -1) {
//       motorNum = input.substring(0, colonIndex).toInt();
//       direction = input.substring(colonIndex + 1);
//       direction.trim();
//     }

//     if ((motorNum == 1 || motorNum == 2) &&
//         (direction == "forward" || direction == "backward")) 
//     {
//       Serial.print("Motor ");
//       Serial.print(motorNum);
//       Serial.print(" running ");
//       Serial.println(direction);

//       runMotor(motorNum, direction);
//       delay(1000);
//       stopMotor(motorNum);
//     } 
//     else {
//       Serial.println("Invalid command.");
//     }

//     Serial.println("Enter next command:");
//   }
// }

// void runMotor(int motor, String dir) {
//   if (motor == 1) {
//     if (dir == "forward") {
//       digitalWrite(in1, HIGH);
//       digitalWrite(in2, LOW);
//     } else {
//       digitalWrite(in1, LOW);
//       digitalWrite(in2, HIGH);
//     }
//     analogWrite(enA, 255);
//   }

//   if (motor == 2) {
//     if (dir == "forward") {
//       digitalWrite(in3, HIGH);
//       digitalWrite(in4, LOW);
//     } else {
//       digitalWrite(in3, LOW);
//       digitalWrite(in4, HIGH);
//     }
//     analogWrite(enB, 255);
//   }
// }


// void stopMotor(int motor) {
//   if (motor == 1) {
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, LOW);
//     analogWrite(enA, 0);
//   }

//   if (motor == 2) {
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, LOW);
//     analogWrite(enB, 0);
//   }
// }










//////////////////////////////////
// SIMPLE WORKING STATE MACHINE //
//////////////////////////////////

// // Motor 1 (A)
// const int enA = 11;
// const int in1 = 10;
// const int in2 = 9;

// // Motor 2 (B)
// const int enB = 3;
// const int in3 = 6;
// const int in4 = 5;

// const int pot1 = A5; 
// const int pot2  = A3; 

// int step = 0; 

// const int STALL_LIMIT = 5;      
// const unsigned long STALL_TIMEOUT = 2000;  

// int lastPos1 = 0;
// unsigned long lastMoveTime1 = 0;
// bool firstRun1 = true;

// int lastPos2 = 0;
// unsigned long lastMoveTime2 = 0;
// bool firstRun2 = true;

// void setup() {
//   Serial.begin(9600);

//   pinMode(enA, OUTPUT);
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);

//   pinMode(enB, OUTPUT);
//   pinMode(in3, OUTPUT);
//   pinMode(in4, OUTPUT);

//   pinMode(pot1, INPUT); 
//   pinMode(pot2, INPUT); 

//   stopMotor(1);
//   stopMotor(2);

//   // Serial.println("Enter command: (e.g. '1: forward', '2: backward')");
// }

// void loop() {

//   switch(step) {

//     case 0:
//       if(motor1ToPosition(1200)) {
//         step = 1;   // move to next step
//         Serial.println("Motion 1 Complete"); 
//       }
//       break;

//     /// READ SENSOR 
//     case 1:
//       if(motor1ToPosition(300)) {
//         step = 2;
//         Serial.println("Motion 2 Complete"); 
//       }
//       break;

//   //   case 2:
//   //     if(motor1ToPosition(0)) {
//   //       step = 3;
//   //       Serial.println("Motion 3 Complete"); 
//   //     }
//   //     break;

//   //   case 3:
//   //     Serial.println("Sequence complete!");
//   //     // stop or restart
//   //     // step = 0;   // optional: repeat sequence
//   //     break;
//   }

// }

// bool  motor1ToPosition(int targetPosition){

//   int pot1Position = analogRead(pot1); 

//   if (firstRun1) {
//     lastPos1 = pot1Position;
//     lastMoveTime1 = millis();
//     firstRun1 = false;
//   }

//   if (abs(pot1Position - lastPos1) > STALL_LIMIT) {
//       lastMoveTime1 = millis();  
//       lastPos1 = pot1Position;
//   }

//   if (millis() - lastMoveTime1 > STALL_TIMEOUT) {
//       stopMotor(1);
//       Serial.println("STALL M1");
//       delay(3000); 
//       firstRun1 = true; 
//       return true;      
//   }

//   if(targetPosition > pot1Position){
//     digitalWrite(in1, HIGH);
//     digitalWrite(in2, LOW);
//   }else if(targetPosition < pot1Position){
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, HIGH);
//   }

//   int error = abs(targetPosition - pot1Position); 

//   if(error > 10){
//       analogWrite(enA, 100);
//       return false; 
//   }
//   else{
//     stopMotor(1); 
//     return true; 
//   }
// }

// bool motor2ToPosition(int targetPosition){

//   int pot2Position = analogRead(pot2); 

//   if(targetPosition > pot2Position){
//     digitalWrite(in3, HIGH);
//     digitalWrite(in4, LOW);
//   }else if(targetPosition < pot2Position){
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, HIGH);
//   }

//   int error = abs(targetPosition - pot2Position); 
//   Serial.print("Error: "); 
//   Serial.println(error); 
  
//   if(error > 10){
//       analogWrite(enB, 100);
//       return false; 
//   }else{
//      stopMotor(2); 
//      return true; 
//   }

// }

// void runMotor(int motor, String dir) {
//   if (motor == 1) {
//     if (dir == "forward") {
//       digitalWrite(in1, HIGH);
//       digitalWrite(in2, LOW);
//     } else {
//       digitalWrite(in1, LOW);
//       digitalWrite(in2, HIGH);
//     }
//     analogWrite(enA, 255);
//   }

//   if (motor == 2) {
//     if (dir == "forward") {
//       digitalWrite(in3, HIGH);
//       digitalWrite(in4, LOW);
//     } else {
//       digitalWrite(in3, LOW);
//       digitalWrite(in4, HIGH);
//     }
//     analogWrite(enB, 255);
//   }
// }



// void stopMotor(int motor) {
//   if (motor == 1) {
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, LOW);
//     analogWrite(enA, 0);
//   }

//   if (motor == 2) {
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, LOW);
//     analogWrite(enB, 0);
//   }
// }



///////////////////////////////////////
// FULL STATE MACHINE IMPLEMENTATION //
///////////////////////////////////////

// ------------------------------
// L298N Motor Pins
// ------------------------------
// Motor 1

const int enA = 11;
const int in1 = 10;
const int in2 = 9;

// Motor 2
const int enB = 3;
const int in3 = 6;
const int in4 = 5;

// Potentiometers
const int pot1 = A5;
const int pot2 = A3;

// ------------------------------
// Stall detection parameters
// ------------------------------
const int STALL_LIMIT = 5;           // minimum pot change to count as movement
const unsigned long STALL_TIMEOUT = 2000;  // 2 seconds

// ------------------------------
// Motor state tracking
// ------------------------------
int testerMotorTarget = 0;
int helperMotorTarget = 0;
bool testerMotorActive = false;
bool helperMotorActive = false;

int lastPos1 = 0;
unsigned long lastMoveTime1 = 0;
bool firstRun1 = true;

int lastPos2 = 0;
unsigned long lastMoveTime2 = 0;
bool firstRun2 = true;

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
  UNLOAD_STEP_1, 
  UNLOAD_STEP_2 
};

SystemState systemState = IDLE;

// ------------------------------
// Setup
// ------------------------------
void setup() {
  Serial.begin(9600);

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
  }
  else {
    Serial.println("Unknown command");
  }

  inputCommand = "";
}

// ------------------------------
// State machine sequences
// ------------------------------

void runStateMachine() {
    switch(systemState) {

        case PUSH_TEST_STEP1:
            if (!stepInitialized) {
                helperMotorTarget = 0;
                helperMotorActive = true;
                stepInitialized = true;  // only do this once
                Serial.println("PUSH TEST STARTED");
            }

            if (!helperMotorActive) {  // motor finished
                systemState = PUSH_TEST_STEP2;
                stepInitialized = false; // reset for next step
            }
            break;
        
        case PUSH_TEST_STEP2:
            if (!stepInitialized) {
                testerMotorTarget = 950;
                testerMotorActive = true;
                stepInitialized = true;  // only do this once
            }

            if (!testerMotorActive) {  // motor finished
                systemState = PUSH_TEST_STEP3;
                stepInitialized = false; // reset for next step
            }
            break;

        case PUSH_TEST_STEP3:
            systemState = IDLE; 
            Serial.println("Implement Logic Here to Read Load Cell"); 
            Serial.println("PUSH TEST COMPLETED"); 
            Serial.println("PUSH_FORCE:70"); 
            break;

        case BACKDRIVE_TEST_STEP1:
            if (!stepInitialized) {
                testerMotorTarget = 600;
                testerMotorActive = true;
                stepInitialized = true;  // only do this once
                Serial.println("BACKDRIVE TEST STARTED");
            }

            if (!testerMotorActive) {  // motor finished
                systemState = BACKDRIVE_TEST_STEP2;
                stepInitialized = false; // reset for next step
            }
            break;


        case BACKDRIVE_TEST_STEP2:
          if (!stepInitialized) {
                helperMotorTarget = 900;
                helperMotorActive = true;
                stepInitialized = true;  // only do this once
          }

          if (!helperMotorActive) {  // motor finished
                systemState = BACKDRIVE_TEST_STEP3;
                stepInitialized = false; // reset for next step
          }
          
          break;

        case BACKDRIVE_TEST_STEP3:
            systemState = IDLE; 
            Serial.println("Implement Logic Here to Read Load Cell"); 
            Serial.println("BACKDRIVE TEST COMPLETE"); 
            Serial.println("BACKDRIVE_FORCE: 60"); 
            break;

        case UNLOAD_STEP_1:
            if (!stepInitialized) {
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
  if (abs(pos - testerMotorTarget) < 5) {
    stopMotor(1);
    testerMotorActive = false;
    firstRun1 = true;
    return;
  }

  if (pos < testerMotorTarget) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

  analogWrite(enB, 250);
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
  if (abs(pos - helperMotorTarget) < 5) {
    stopMotor(2);
    helperMotorActive = false;
    firstRun2 = true;
    return;
  }

  if (pos < helperMotorTarget) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }

  analogWrite(enA, 250);
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


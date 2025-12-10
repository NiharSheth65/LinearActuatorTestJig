////////////////////////////////
// LINEAR ACTUATOR TESTER ////// 
// L298N + POT + PID ///////////
////////////////////////////////

// Motor 1 (A)
const int enA = 3;
const int in1 = 5;
const int in2 = 6;

// Motor 2 (B)
const int enB = 11;
const int in3 = 9; 
const int in4 = 10;

// Potentiometers
int pot1 = A3;
int pot2 = A5;  // optional second actuator

// PID parameters (tune as needed)
float Kp = 2.0;
float Ki = 0.0;
float Kd = 0.5;

int targetMotor = 0;
int targetPos = 0;

float integral = 0;
float lastError = 0;

void setup() {
  Serial.begin(9600);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);

  stopMotor(1);
  stopMotor(2);

  Serial.println("Enter command: motor:position (e.g. 1:500)");
}

void loop() {
  // --- Read serial commands ---
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int colonIndex = input.indexOf(':');
    if (colonIndex != -1) {
      targetMotor = input.substring(0, colonIndex).toInt();
      targetPos = input.substring(colonIndex + 1).toInt();
      integral = 0;
      lastError = 0;
      Serial.print("Moving motor ");
      Serial.print(targetMotor);
      Serial.print(" to position ");
      Serial.println(targetPos);
    } else {
      Serial.println("Invalid command format. Use motor:position");
    }
  }

  // --- PID control loop ---
  int currentPos = (targetMotor == 1) ? analogRead(pot1) : analogRead(pot2);
  float error = targetPos - currentPos;
  integral += error;
  float derivative = error - lastError;
  float output = Kp * error + Ki * integral + Kd * derivative;
  lastError = error;

  int pwmOutput = constrain(abs(output), 0, 255);

  // --- Apply motor control ---
  if (targetMotor == 1) {
    if (error > 0) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    } else {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
    analogWrite(enA, pwmOutput);
  } else if (targetMotor == 2) {
    if (error > 0) {
      analogWrite(in3, pwmOutput);
      digitalWrite(in4, LOW);
    } else {
      digitalWrite(in3, LOW);
      analogWrite(in4, pwmOutput);
    }
    analogWrite(enB, pwmOutput);
  }

  // Stop motor if within tolerance
  if (abs(error) < 3) {  // adjust tolerance
    stopMotor(targetMotor);
  }

  // Optional: print debug info
  Serial.print("Motor ");
  Serial.print(targetMotor);
  Serial.print(" | Pos: ");
  Serial.print(currentPos);
  Serial.print(" | Error: ");
  Serial.print(error);
  Serial.print(" | PWM: ");
  Serial.println(pwmOutput);

  delay(20);  // loop delay, adjust for responsiveness
}

void stopMotor(int motor) {
  if (motor == 1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0);
  } else if (motor == 2) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enB, 0);
  }
}

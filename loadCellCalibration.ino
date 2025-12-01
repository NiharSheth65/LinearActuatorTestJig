#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 7;
const int LOADCELL_SCK_PIN = 8;

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Initializing scale...");

  // Tare the scale (set the current reading as 0)
  scale.tare();

  // Apply your calibration factor (kg)
  scale.set_scale(-40896.506);
  
  Serial.println("Scale ready.");
}

void loop() {
  if (scale.is_ready()) {
    float kg = scale.get_units(10);   // average of 10 readings
    Serial.print("Weight (kg): ");
    Serial.println(kg, 1);            // print to 3 decimal places
  } else {
    Serial.println("HX711 not found.");
  }

  delay(1000);
}

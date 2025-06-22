#include <OneWire.h>
#include <DallasTemperature.h>

// Corrected pins based on your wiring
#define DS18B20_PIN 4
#define MOISTURE_SENSOR_PIN 0  // A0 on ESP32-C3
#define MQ135_SENSOR_PIN 1      // A1 on ESP32-C3

OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);

// Thresholds
const float TEMP_LOW = 35.5;   // Hypothermia
const float TEMP_HIGH = 37.8;  // Rash/fever

const int MOISTURE_THRESHOLD = 1800; // Calibrate as needed
const int GAS_THRESHOLD = 3000;      // Calibrate after test

void setup() {
  Serial.begin(115200);
  delay(1000);
  tempSensor.begin();
  Serial.println("Smart Diaper Monitor Initialized.");
}

void loop() {
  // --- Temperature Reading ---
  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempCByIndex(0);
  Serial.print("Body Temp: ");
  Serial.print(tempC);
  Serial.println(" °C");

  if (tempC < TEMP_LOW) {
    Serial.println(" Warning: Body temp too low.");
  } else if (tempC > TEMP_HIGH) {
    Serial.println(" Warning: High temp detected (rash/fever risk).");
  }

  // --- Moisture Sensor Reading ---
  int moisture = analogRead(MOISTURE_SENSOR_PIN);
  Serial.print("Moisture: ");
  Serial.println(moisture);
  if (moisture < MOISTURE_THRESHOLD) {
    Serial.println(" Diaper is wet. Change needed.");
  }

  // --- MQ135 Gas Sensor Reading ---
  int gas = analogRead(MQ135_SENSOR_PIN);
  Serial.print("Gas Level: ");
  Serial.println(gas);
  if (gas > GAS_THRESHOLD) {
    Serial.println(" Elevated ammonia: UTI/rash risk.");
  }

  Serial.println("-----------------------------");
  delay(5000);
}

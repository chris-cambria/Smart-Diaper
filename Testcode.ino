#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Provide the token generation process info.

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials

#define WIFI_SSID 
#define WIFI_PASSWORD 
#define API_KEY 
#define DATABASE_URL 

#define DS18B20_PIN     3   // GPIO3
#define MOISTURE_PIN    0   // GPIO0
#define GAS_SENSOR_PIN  1   // GPIO1

OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);

// Firebase objects

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int moisture = 0;
int gas=0;
float tempC = 0.0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the API key and database URL
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if(Firebase.signUp(&config,&auth,"","")){
    Serial.println("signUp OK");
    signupOK =  true;
  }
  else{
      Serial.printf("%s\n",config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config,&auth);
  Firebase.reconnectWiFi(true);
  tempSensor.begin();
}

void loop() {
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    moisture = analogRead(MOISTURE_PIN);
    gas = analogRead(GAS_SENSOR_PIN);
    tempSensor.requestTemperatures();
    tempC = tempSensor.getTempCByIndex(0);
    if(Firebase.RTDB.setInt(&fbdo, "Sensor/moisture", moisture)){
      Serial.println(); Serial.print(moisture);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.print(" (" + fbdo.dataType() + ")");
    }
    else{
      Serial.println("Failed: "+fbdo.errorReason());
    }


    if(Firebase.RTDB.setInt(&fbdo, "Sensor/gas", gas)){
      Serial.println(); Serial.print(gas);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.print(" (" + fbdo.dataType() + ")");
    }
    else{
      Serial.println("Failed: "+fbdo.errorReason());
    }


    if(Firebase.RTDB.setFloat(&fbdo, "Sensor/tempC", tempC)){
      Serial.println(); Serial.print(tempC);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.print(" (" + fbdo.dataType() + ")");
    }
    else{
      Serial.println("Failed: "+fbdo.errorReason());
    }
  }
}


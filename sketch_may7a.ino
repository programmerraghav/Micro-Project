#include <WiFi.h>
#include <FirebaseESP32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Wi-Fi credentials
const char* ssid = "Galaxy M31526A";
const char* password = "raghav1234";

// Firebase credentials
#define FIREBASE_HOST "https://smart-cup-coaster-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "W5HwRP44SLGrPiTWV1NV3BZNnFANbYvfD53g18mx"

// I/O pins
#define ONE_WIRE_BUS 4
#define LED_PIN 5
#define LEDCOLD_PIN 2
#define RELAY_PIN 23
#define RELAYCOLD_PIN 22

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// DS18B20 setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Hysteresis
const float hysteresis = 2.0;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LEDCOLD_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAYCOLD_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LEDCOLD_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(RELAYCOLD_PIN, LOW);

  sensors.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Firebase setup
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int targetTemp = 0;
  bool isHot = true;
  static bool heatingOn = false;
  static bool coolingOn = false;

  // Fetch temperature threshold from Firebase
  while (true) {
    Serial.println("Fetching target temperature...");
    if (Firebase.RTDB.getInt(&fbdo, "/user1/temperature")) {
      targetTemp = fbdo.intData();
      Serial.print("Target Temp: ");
      Serial.println(targetTemp);
      break;
    } else {
      Serial.print("Fetch failed: ");
      Serial.println(fbdo.errorReason());
      delay(2000);
    }
  }

  // Fetch isHot value
  if (Firebase.RTDB.getBool(&fbdo, "/user1/isHot")) {
    isHot = fbdo.boolData();
    Serial.print("isHot (from Firebase): ");
    Serial.println(isHot ? "true" : "false");
  } else {
    Serial.print("Failed to fetch isHot: ");
    Serial.println(fbdo.errorReason());
  }

  // Read temperature
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Temperature sensor not found!");
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LEDCOLD_PIN, LOW);
    delay(2000);
    return;
  }

  Serial.print("Current Temp: ");
  Serial.print(tempC);
  Serial.println(" °C");

  // Upload current temperature to Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "/user2/currentTemperature", tempC)) {
    Serial.println("Temperature uploaded to Firebase.");
  } else {
    Serial.print("Failed to upload temperature: ");
    Serial.println(fbdo.errorReason());
  }

  // Control Logic with Hysteresis
  if (isHot) {
    if (tempC < (targetTemp - hysteresis) && !heatingOn) {
      heatingOn = true;
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
    } else if (tempC >= targetTemp && heatingOn) {
      heatingOn = false;
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
    }
    // Ensure cooling is off
    if (coolingOn) {
      coolingOn = false;
      digitalWrite(RELAYCOLD_PIN, LOW);
      digitalWrite(LEDCOLD_PIN, LOW);
    }
  } else {
    if (tempC > (targetTemp + hysteresis) && !coolingOn) {
      coolingOn = true;
      digitalWrite(RELAYCOLD_PIN, HIGH);
      digitalWrite(LEDCOLD_PIN, HIGH);
    } else if (tempC <= targetTemp && coolingOn) {
      coolingOn = false;
      digitalWrite(RELAYCOLD_PIN, LOW);
      digitalWrite(LEDCOLD_PIN, LOW);
    }
    // Ensure heating is off
    if (heatingOn) {
      heatingOn = false;
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
    }
  }

  delay(2000); // Delay for stability
}

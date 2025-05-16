#define RELAY_PIN 19
#define RELAYCOLD_PIN 18

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAYCOLD_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Test both HIGH and LOW
  digitalWrite(RELAYCOLD_PIN, HIGH);
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(RELAY_PIN, LOW); // Try LOW for active-low relays
  digitalWrite(RELAYCOLD_PIN, LOW);
  delay(2000);
  Serial.println("Relay OFF");
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(RELAYCOLD_PIN, HIGH);
  delay(2000);
}
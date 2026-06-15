#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// ================= WiFi Credentials =================
const char* ssid = "chase";
const char* password = "123456789";

// ================= ThingSpeak ======================
unsigned long myChannelNumber = 3211878;   // <-- Your Channel ID (NO quotes)
const char* myWriteAPIKey = "UOQS87Z88ZTCRRUE";

WiFiClient client;

// ================= HC-SR04 Pins ====================
#define TRIG_PIN D5   // GPIO14
#define ECHO_PIN D6   // GPIO12

void setup() {
  Serial.begin(9600);
  delay(2000);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // -------- WiFi Connection --------
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
}

void loop() {
  long duration;
  float distance;

  // Trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout

  if (duration == 0) {
    Serial.println("❌ No echo received");
    delay(2000);
    return;
  }

  // Distance calculation (cm)
  distance = (duration * 0.034) / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Send to ThingSpeak
  ThingSpeak.setField(1, distance);

  int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (status == 200) {
    Serial.println("✅ Data sent to ThingSpeak");
  } else {
    Serial.print("❌ ThingSpeak error: ");
    Serial.println(status);
  }

  delay(15000); // ThingSpeak minimum interval
}

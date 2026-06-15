#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// -------- USER CONFIG --------
#define WIFI_SSID     "chase"
#define WIFI_PASSWORD "123456789"

#define BOT_TOKEN "8270512776:AAEN9F7sl-tFg1D05_-TM4u9vD6Tm81DuvQ"
#define CHAT_ID  "5277555980"

// -------- CORIANDER THRESHOLDS --------
#define PUMP_ON_THRESHOLD   25   // %
#define PUMP_OFF_THRESHOLD  55   // %

#define PUMP_FLOW_RATE 1.2       // liters per minute

// -------- PIN CONFIG --------
#define SOIL_PIN   A0
#define RELAY_PIN  D1
#define LED_PIN    D4   // Built-in LED (Active LOW)

// -------- OBJECTS --------
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// -------- VARIABLES --------
int rawValue;
int moisturePercent;
String soilStatus;

bool pumpState = false;
unsigned long pumpStartTime = 0;
float totalWaterUsed = 0.0;

unsigned long lastTelegramCheck = 0;

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);  // Pump OFF
  digitalWrite(LED_PIN, HIGH);    // LED OFF

  // ---- WiFi connect with LED blink ----
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    delay(250);
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    Serial.print(".");
  }

  digitalWrite(LED_PIN, LOW); // LED ON solid
  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());

  client.setInsecure();
  configTime(0, 0, "pool.ntp.org");

  bot.sendMessage(CHAT_ID,
    "🌿 Smart Irrigation Started\nPlant: Coriander 🌿",
    "");
}

// -------- READ SOIL --------
void readSoil() {
  rawValue = analogRead(SOIL_PIN);

  moisturePercent = map(rawValue, 1023, 300, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  if (moisturePercent < PUMP_ON_THRESHOLD)
    soilStatus = "DRY";
  else if (moisturePercent > PUMP_OFF_THRESHOLD)
    soilStatus = "WET";
  else
    soilStatus = "MODERATE";
}

// -------- CONTROL PUMP --------
void controlPump() {
  if (soilStatus == "DRY" && !pumpState) {
    digitalWrite(RELAY_PIN, LOW); // Pump ON
    pumpState = true;
    pumpStartTime = millis();
  }

  if (soilStatus == "WET" && pumpState) {
    digitalWrite(RELAY_PIN, HIGH); // Pump OFF
    pumpState = false;

    unsigned long pumpTimeMs = millis() - pumpStartTime;
    float minutes = pumpTimeMs / 60000.0;
    float waterUsed = minutes * PUMP_FLOW_RATE;
    totalWaterUsed += waterUsed;

    sendReport(waterUsed);
  }
}

// -------- TELEGRAM REPORT (MATCHES YOUR IMAGE) --------
void sendReport(float cycleWater) {
  String msg = "🌱 *Soil Moisture Report*\n\n";
  msg += "Plant     : Coriander 🌿\n";
  msg += "Raw Value : " + String(rawValue) + "\n";
  msg += "Moisture  : " + String(moisturePercent) + " %\n";
  msg += "Status    : " + soilStatus;

  if (soilStatus == "DRY") {
    msg += " 😟\n\n🚨 *Soil Condition Dry*\n🚰 Watering Started";
  }
  else if (soilStatus == "MODERATE") {
    msg += " 🙂\n\n✅ *Soil Condition Normal*";
  }
  else {
    msg += " 💧\n\n🌿 *Soil Condition Wet*";
  }

  msg += "\n\n💧 Water Used (Cycle): " + String(cycleWater, 2) + " L";
  msg += "\n📊 Total Water Used : " + String(totalWaterUsed, 2) + " L";

  bot.sendMessage(CHAT_ID, msg, "Markdown");
}

// -------- TELEGRAM COMMAND --------
void handleTelegram() {
  if (millis() - lastTelegramCheck < 3000) return;

  int newMessages = bot.getUpdates(bot.last_message_received + 1);
  while (newMessages) {
    for (int i = 0; i < newMessages; i++) {
      String text = bot.messages[i].text;

      if (text == "/status") {
        String msg = "🌱 *System Status*\n\n";
        msg += "Plant     : Coriander 🌿\n";
        msg += "Moisture  : " + String(moisturePercent) + " %\n";
        msg += "Status    : " + soilStatus + "\n";
        msg += "Pump      : " + String(pumpState ? "ON" : "OFF") + "\n";
        msg += "💧 Total Water Used: " + String(totalWaterUsed, 2) + " L";

        bot.sendMessage(CHAT_ID, msg, "Markdown");
      }
    }
    newMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  lastTelegramCheck = millis();
}

// -------- LOOP --------
void loop() {
  readSoil();
  controlPump();
  handleTelegram();

  Serial.print("Moisture=");
  Serial.print(moisturePercent);
  Serial.print("% Pump=");
  Serial.println(pumpState ? "ON" : "OFF");

  delay(2000);
}

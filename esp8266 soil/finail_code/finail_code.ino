#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// -------- USER CONFIG --------
#define WIFI_SSID     "chase"
#define WIFI_PASSWORD "123456789"

#define BOT_TOKEN  "8270512776:AAEN9F7sl-tFg1D05_-TM4u9vD6Tm81DuvQ"
#define CHAT_ID  "5277555980"

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

unsigned long lastTelegramCheck = 0;
const unsigned long telegramInterval = 3000;

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);  // Pump OFF
  digitalWrite(LED_PIN, HIGH);    // LED OFF

  // ---- WiFi connect with LED blink ----
  WiFi.mode(WIFI_STA);
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

  bot.sendMessage(CHAT_ID, "🌱 Smart Irrigation System Started", "");
}

// -------- READ SOIL --------
void readSoil() {
  rawValue = analogRead(SOIL_PIN);

  // Calibrate if needed
  moisturePercent = map(rawValue, 1023, 300, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  if (moisturePercent < 30)
    soilStatus = "DRY";
  else if (moisturePercent < 70)
    soilStatus = "MODERATE";
  else
    soilStatus = "WET";
}

// -------- CONTROL PUMP --------
void controlPump() {
  if (soilStatus == "DRY" && !pumpState) {
    digitalWrite(RELAY_PIN, LOW);
    pumpState = true;
    sendSoilReport();
  }

  if (soilStatus == "WET" && pumpState) {
    digitalWrite(RELAY_PIN, HIGH);
    pumpState = false;
    sendSoilReport();
  }
}

// -------- TELEGRAM MESSAGE FORMAT (MATCH IMAGE) --------
void sendSoilReport() {
  String message = "🌱 *Soil Moisture Report*\n\n";
  message += "Raw Value : " + String(rawValue) + "\n";
  message += "Moisture  : " + String(moisturePercent) + " %\n";
  message += "Status    : " + soilStatus;

  if (soilStatus == "DRY") {
    message += " 😟\n\n🚨 *Soil Condition Dry*\n🚰 Water Required";
  }
  else if (soilStatus == "MODERATE") {
    message += " 🙂\n\n✅ *Soil Condition Normal*";
  }
  else {
    message += " 💧\n\n🌿 *Soil Condition Wet*";
  }

  bot.sendMessage(CHAT_ID, message, "Markdown");
}

// -------- TELEGRAM COMMAND --------
void handleTelegram() {
  if (millis() - lastTelegramCheck > telegramInterval) {
    int newMessages = bot.getUpdates(bot.last_message_received + 1);

    while (newMessages) {
      for (int i = 0; i < newMessages; i++) {
        String text = bot.messages[i].text;

        if (text == "/status") {
          sendSoilReport();
        }
      }
      newMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTelegramCheck = millis();
  }
}

// -------- LOOP --------
void loop() {
  readSoil();
  controlPump();
  handleTelegram();

  Serial.print("Raw=");
  Serial.print(rawValue);
  Serial.print(" Moisture=");
  Serial.print(moisturePercent);
  Serial.print("% Pump=");
  Serial.println(pumpState ? "ON" : "OFF");

  delay(2000);
}

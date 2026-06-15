#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define BOT_TOKEN "8270512776:AAEN9F7sl-tFg1D05_-TM4u9vD6Tm81DuvQ"
#define CHAT_ID "5277555980"

const char* ssid = "chase";
const char* password = "123456789";

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void setup() {
  Serial.begin(9600);

  client.setInsecure();   // Required for Telegram HTTPS

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  bot.sendMessage(CHAT_ID, "🌱 Soil Moisture Monitoring Started", "");
}

void loop() {

  int rawValue = analogRead(A0);

  // Calibrate these values if needed
  int moisturePercent = map(rawValue, 1023, 300, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  String status;
  String emoji;

  if (moisturePercent < 30) {
    status = "DRY";
    emoji = "⚠️";
  } else if (moisturePercent < 70) {
    status = "MODERATE";
    emoji = "🙂";
  } else {
    status = "WET";
    emoji = "💧";
  }

  String message = "🌱 *Soil Moisture Report*\n\n";
  message += "Raw Value : " + String(rawValue) + "\n";
  message += "Moisture  : " + String(moisturePercent) + " %\n";
  message += "Status    : " + status + " " + emoji + "\n\n";

  if (status == "DRY") {
    message += "🚰 *Action:* Water Required";
  } else {
    message += "✅ Soil Condition Normal";
  }

  bot.sendMessage(CHAT_ID, message, "Markdown");

  Serial.println(message);
  Serial.println("-------------------------");

  delay(10000);  // Send every 10 seconds
}

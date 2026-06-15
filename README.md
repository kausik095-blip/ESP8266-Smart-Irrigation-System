 

## Smart Irrigation System using ESP8266

This project is a fully automated IoT-based irrigation system developed using ESP8266 NodeMCU and a soil moisture sensor. The system continuously monitors soil moisture levels in grow bags and automatically controls a water pump based on plant-specific moisture thresholds.

### Features

* 🌱 Automatic soil moisture monitoring
* 🚰 Automatic water pump ON/OFF control
* 📱 Telegram Bot integration
* 💧 Water usage calculation
* 🌿 Plant-specific irrigation thresholds
* 🔋 Battery or Power Bank powered
* 📊 Real-time moisture reporting
* 🪴 Optimized for grow bag cultivation
* ⏱ Automatic monitoring every 2 minutes
* 💡 Wi-Fi status LED indication

### Hardware Requirements

* ESP8266 NodeMCU
* Soil Moisture Sensor
* 1-Channel Relay Module
* 5V Mini Water Pump
* Grow Bag
* Drip Irrigation Pipe
* Power Bank / 18650 Battery
* Jumper Wires

### Software Requirements

* Arduino IDE
* ESP8266 Board Package
* UniversalTelegramBot Library
* WiFiClientSecure Library

### Telegram Commands

```text
/status
```

Displays:

* Moisture Percentage
* Soil Status
* Pump Status
* Total Water Usage

### Moisture Thresholds (Coriander)

| Condition | Moisture (%) |
| --------- | ------------ |
| Dry       | < 25%        |
| Normal    | 25% - 55%    |
| Wet       | > 55%        |

### System Workflow

```text
Soil Sensor
     ↓
ESP8266
     ↓
Moisture Analysis
     ↓
Pump ON/OFF
     ↓
Telegram Notification
     ↓
Grow Bag Irrigation
```

### Project Cost

Approximate Project Cost: **₹1,400**

### Future Enhancements

* Google Sheets Logging
* Solar Power Integration
* Multiple Grow Bag Support
* Mobile App Dashboard
* AI-based Water Prediction

---

# 🏷 GitHub Topics

```text
esp8266
arduino
iot
smart-irrigation
soil-moisture-sensor
telegram-bot
automation
agriculture
nodeMCU
grow-bag
smart-farming
water-management
embedded-systems
```

---

# 📸 Screenshot Caption (for your Telegram output image)

**Figure: Real-time soil moisture monitoring and irrigation status notification received through Telegram Bot.**

 

# Intelligent Water Quality Monitoring System

An IoT-based smart water quality monitoring system developed using ESP32 and Arduino IDE to monitor water parameters such as pH, TDS, turbidity, and temperature in real time. The system also predicts Water Quality Index (WQI) using a Random Forest Regressor machine learning model.

---

## Features

- Real-time monitoring of water quality parameters
- Wireless IoT monitoring using ESP32 Wi-Fi
- Cloud integration using ThingSpeak/Blynk
- LCD display using I2C communication
- Threshold-based automation using a solenoid valve
- Water Quality Index (WQI) prediction using Machine Learning
- Random Forest Regressor for accurate WQI estimation

---

## Technologies Used

### Hardware
- ESP32 Development Board
- pH Sensor
- TDS Sensor
- Turbidity Sensor
- Temperature Sensor
- 16x2 I2C LCD
- Solenoid Valve

### Software
- Arduino IDE
- Embedded C
- Python
- Random Forest Regressor
- ThingSpeak
- Blynk
- MQTT Protocol

---

## System Architecture

1. Sensors collect water quality parameters
2. ESP32 reads sensor data using ADC
3. Data is processed and sent to cloud platforms
4. Machine learning model predicts WQI
5. Solenoid valve is controlled automatically based on thresholds

---

## Machine Learning Model

The project uses a Random Forest Regressor to predict Water Quality Index (WQI) based on sensor values.

### Input Parameters
- pH
- TDS
- Turbidity
- Temperature

### ML Workflow
- Data preprocessing
- Feature selection
- Model training
- WQI prediction
- Accuracy evaluation

---

## Project Structure

```text
Arduino_Code/       -> ESP32 firmware
ML_Model/           -> Random Forest model and dataset
Circuit_Diagram/    -> Circuit images
Images/              -> Prototype and dashboard images
Documentation/      -> Project reports
```

---

## Hardware Connections

| Sensor | ESP32 Pin |
|--------|-----------|
| pH Sensor | GPIO34 |
| TDS Sensor | GPIO35 |
| Turbidity Sensor | GPIO32 |
| Temperature Sensor | GPIO33 |

---

## How to Run

### Arduino IDE Setup

1. Install Arduino IDE
2. Install ESP32 Board Manager
3. Install required libraries:
   - WiFi.h
   - ThingSpeak
   - LiquidCrystal_I2C
   - OneWire
   - DallasTemperature

4. Upload code to ESP32

---

## Cloud Dashboard

The system sends real-time sensor data to:
- ThingSpeak
- Blynk

Users can monitor water quality remotely through dashboards.

---

## Results

- Accurate real-time water monitoring
- Successful cloud communication
- Reliable WQI prediction using ML
- Automated water quality control

---

## Future Improvements

- Mobile application integration
- AI-based anomaly detection
- GSM/GPS support
- Solar-powered deployment
- Edge AI implementation on ESP32

---

## 👨‍💻 Author

**Siddhant Babanagar**
🎓 Electronics & Telecommunication Engineer (2025)
💡 Interested in Embedded Systems & Low-Level Programming

---

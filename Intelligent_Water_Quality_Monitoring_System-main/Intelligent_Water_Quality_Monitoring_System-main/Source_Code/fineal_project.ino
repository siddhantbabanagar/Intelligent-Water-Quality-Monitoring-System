#define BLYNK_TEMPLATE_ID "TMPL3IrLgo1-Q"
#define BLYNK_TEMPLATE_NAME "Water Quality Monitoring"
#define BLYNK_AUTH_TOKEN "T1KLTQMbiE5mMcppifayyaRbgdj_ZpRx"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "abc";
char pass[] = "123456789";

const char* server = "http://api.thingspeak.com";
String apiKey = "YV1NNR5LNT8M7S4M";

// Pins
#define TdsSensorPin 32
#define TURBIDITY_SENSOR_PIN 35
#define PH_SENSOR_PIN 34
#define SOLENOID_PIN 26
#define ONE_WIRE_BUS 4

float calibrationFactor = 0.5;
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int valveState = 0;

float calibration_slope = -5.70;
float calibration_intercept = 21.34;

// Sensor value storage
float lastTDS = 0, lastTurbidity = 0, lastpH = 0, lastTemp = 0;
int lcdState = 0;

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Reconnecting WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 10) {
      delay(500);
      Serial.print(".");
      attempt++;
    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected!" : "Failed!");
  }
}

void sendSensorData() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  const int samples = 30;
  int adcSum = 0;
  for (int i = 0; i < samples; i++) {
    adcSum += analogRead(TdsSensorPin);
    delay(10);
  }
  float avgADC = adcSum / float(samples);
  float voltage = avgADC * 3.3 / 4095.0;

  float ec = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * 0.5;
  float tdsValue = ec / (1.0 + 0.02 * (temperature - 25.0));

  int turbidityRaw = analogRead(TURBIDITY_SENSOR_PIN);
  float turbidityVoltage = turbidityRaw * (3.3 / 4095.0);
  float turbidity_NTU = 1120.4 * pow(turbidityVoltage, 2) + 5742.3 * turbidityVoltage - 4352.9;
  turbidity_NTU = max(turbidity_NTU / 1500 / 10, 0.0f);

  int phRaw = analogRead(PH_SENSOR_PIN);
  float phVoltage = phRaw * (3.3 / 4095.0);
  float phValue = calibration_slope * phVoltage + calibration_intercept;
  phValue = constrain(phValue, 0, 14);

  // Store values globally for LCD display
  lastTemp = temperature;
  lastTDS = tdsValue;
  lastTurbidity = turbidity_NTU;
  lastpH = phValue;

  Serial.print("Temp: "); Serial.print(temperature, 1);
  Serial.print(" °C | TDS: "); Serial.print(tdsValue, 2);
  Serial.print(" ppm | Turbidity: "); Serial.print(turbidity_NTU, 2);
  Serial.print(" NTU | pH: "); Serial.println(phValue, 2);

  // Send to Blynk if connected
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, tdsValue);
    Blynk.virtualWrite(V1, turbidity_NTU);
    Blynk.virtualWrite(V2, phValue);
    Blynk.virtualWrite(V4, temperature);
    Blynk.virtualWrite(V5, valveState);
  }

  // Send to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String url = server + String("/update?api_key=") + apiKey +
                 "&field1=" + String(tdsValue) +
                 "&field2=" + String(turbidity_NTU) +
                 "&field3=" + String(phValue) +
                 "&field4=" + String(temperature);
    http.begin(client, url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("ThingSpeak Update Success: " + String(httpCode));
    } else {
      Serial.println("ThingSpeak Error: " + http.errorToString(httpCode));
    }
    http.end();
  }
}

void updateLCD() {
  lcd.clear();
  switch (lcdState) {
    case 0:
      lcd.setCursor(0, 0); lcd.print("TDS: "); lcd.print(lastTDS, 0); lcd.print("ppm");
      break;
    case 1:
      lcd.setCursor(0, 0); lcd.print("Turb: "); lcd.print(lastTurbidity, 1); lcd.print(" NTU");
      break;
    case 2:
      lcd.setCursor(0, 0); lcd.print("pH: "); lcd.print(lastpH, 2);
      break;
    case 3:
      lcd.setCursor(0, 0); lcd.print("Temp: "); lcd.print(lastTemp, 1); lcd.print(" C");
      break;
    case 4:
      lcd.setCursor(0, 0); lcd.print("Valve Status:");
      lcd.setCursor(0, 1); lcd.print(valveState ? "ON" : "OFF");
      break;
  }
  lcdState = (lcdState + 1) % 5;
}

BLYNK_WRITE(V3) {
  valveState = param.asInt();
  digitalWrite(SOLENOID_PIN, valveState == 1 ? LOW : HIGH);
  Serial.println(valveState ? "Solenoid ON" : "Solenoid OFF");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valve Status:");
  lcd.setCursor(0, 1);
  lcd.print(valveState ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(SOLENOID_PIN, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  sensors.begin();

  WiFi.begin(ssid, pass);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    Serial.print(".");
    attempt++;
  }
  lcd.setCursor(0, 1);
  lcd.print(WiFi.status() == WL_CONNECTED ? "WiFi Connected!" : "WiFi Failed!");
  Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi Connected!" : "\nWiFi Failed!");

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(5000L, sendSensorData);
  timer.setInterval(2000L, updateLCD); // Separate timer for LCD
}

void loop() {
  Blynk.run();
  timer.run();
}

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <LittleFS.h>

// OLED Includes
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUZZER_PIN D7
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ESP8266WebServer server(80);

unsigned long previousMillis = 0;
const long interval = 30000; // 30 seconds

float temperature = 0.0;
float humidity = 0.0;
const char* failedDataFile = "/failed_data.txt";
const char* serverPath = "http://192.168.4.2/Wsense/index.php";

void handleDataRequest() {
    String jsonResponse = "{ \"temperature\": " + String(temperature, 1) + 
                          ", \"humidity\": " + String(humidity, 1) + " }";
    server.send(200, "application/json", jsonResponse);
}

void sendDataToServer() {
    HTTPClient http;
    WiFiClient client;
    
    String postData = "temperature=" + String(temperature, 1) + "&humidity=" + String(humidity, 1);

    http.begin(client, serverPath);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
        Serial.print("Server Response: ");
        Serial.println(http.getString());
        resendStoredData(); // Try resending any stored data
    } else {
        Serial.println("Error sending data. Storing for later.");
        saveFailedData(postData);
    }

    http.end();
}

void saveFailedData(const String& data) {
    File file = LittleFS.open(failedDataFile, "a");
    if (!file) {
        Serial.println("Failed to open file for writing!");
        return;
    }
    file.println(data);
    file.close();
    Serial.println("Failed data stored in LittleFS.");
}

void resendStoredData() {
    if (!LittleFS.exists(failedDataFile)) return;

    File file = LittleFS.open(failedDataFile, "r");
    if (!file) {
        Serial.println("Failed to open file for reading!");
        return;
    }

    String line;
    while (file.available()) {
        line = file.readStringUntil('\n');

        HTTPClient http;
        WiFiClient client;
        http.begin(client, serverPath);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int httpResponseCode = http.POST(line);
        http.end();

        if (httpResponseCode > 0) {
            Serial.println("Resent stored data successfully!");
        } else {
            Serial.println("Failed to resend stored data. Keeping it for later.");
            file.close();
            return;
        }
    }

    file.close();
    LittleFS.remove(failedDataFile);
    Serial.println("All stored data sent and deleted.");
}

void setup() {
    Serial.begin(115200);

    pinMode(BUZZER_PIN, OUTPUT);
    for (int i = 0; i < 2; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }

    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed!");
    } else {
        Serial.println("LittleFS mounted successfully.");
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 OLED init failed!");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 15);
    display.print("Initializing...");
    display.display();
    delay(2000);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("WeatherSense_AP", "12345678");

    Serial.println("Access Point Started");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/index.php", HTTP_GET, handleDataRequest);
    server.begin();
    Serial.println("Web server started!");

    dht.begin();
}

void updateOLED() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 10);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.print(" C");

    display.setCursor(0, 20);
    display.print("Humidity: ");
    display.print(humidity, 1);
    display.print(" %");

    display.display();
}

void loop() {
    server.handleClient();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        float newTemperature = dht.readTemperature();
        float newHumidity = dht.readHumidity();

        if (!isnan(newTemperature) && !isnan(newHumidity)) {
            temperature = newTemperature;
            humidity = newHumidity;

            Serial.printf("Temp: %.1f°C | Humidity: %.1f%%\n", temperature, humidity);

            updateOLED();
            sendDataToServer();
        } else {
            Serial.println("Failed to read from DHT sensor!");
        }
    }
}

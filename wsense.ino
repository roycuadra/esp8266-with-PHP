#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);  // Web server running on port 80

unsigned long previousMillis = 0;
const long interval = 60000; // 1 minute

float temperature = 0.0;
float humidity = 0.0;

// Function to handle incoming HTTP requests
void handleDataRequest() {
    String jsonResponse = "{ \"temperature\": " + String(temperature, 1) + 
                          ", \"humidity\": " + String(humidity, 1) + " }";
    server.send(200, "application/json", jsonResponse);
}

// Function to send data to local PHP server
void sendDataToServer() {
    HTTPClient http;
    WiFiClient client;
    
    String serverPath = "http://192.168.4.2/Wsense/index.php";  

    String postData = "temperature=" + String(temperature, 1) + 
                      "&humidity=" + String(humidity, 1);

    http.begin(client, serverPath);  // Start HTTP connection
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);  // Send the POST request

    // Only print response on error
    if (httpResponseCode <= 0) {
        Serial.print("Error sending data: ");
        Serial.println(httpResponseCode);
    } else {
        Serial.print("Server Response: ");
        Serial.println(http.getString());
    }

    http.end();  // End HTTP connection
}

void setup() {
    Serial.begin(115200);

    // Set ESP8266 to AP mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP("WeatherSense_AP", "12345678");  // AP SSID & Password

    Serial.println("Access Point Started");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Start Web Server
    server.on("/store_data.php", HTTP_GET, handleDataRequest);
    server.begin();
    Serial.println("Web server started!");

    // Initialize DHT sensor
    dht.begin();
}

void loop() {
    server.handleClient();  // Handle incoming HTTP requests

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Read temperature and humidity values once per minute
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();

        if (!isnan(temperature) && !isnan(humidity)) {
            Serial.print("Temp: ");
            Serial.print(temperature);
            Serial.print(" °C | Humidity: ");
            Serial.print(humidity);
            Serial.println(" %");

            // Send data to local server
            sendDataToServer();
        } else {
            Serial.println("Failed to read from DHT sensor!");
        }
    }
}

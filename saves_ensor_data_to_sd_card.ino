#include <SPI.h>
#include <SD.h>
#include "DHT.h"

// Pin configuration
#define DHTPIN 4        // Connect DHT11 signal pin to GPIO 4
#define DHTTYPE DHT11   // Change to DHT22 if using that sensor

const int CS = 5;       // Chip select pin for SD card

DHT dht(DHTPIN, DHTTYPE);
File myFile;

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT11 sensor initialized.");

  // Initialize SD card
  if (!SD.begin(CS)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized.");

  // Create CSV file with header if it doesn't exist
  if (!SD.exists("/data.csv")) {
    myFile = SD.open("/data.csv", FILE_WRITE);
    if (myFile) {
      myFile.println("Time(ms),Temperature(C),Humidity(%)");
      myFile.close();
      Serial.println("CSV file created with header.");
    } else {
      Serial.println("Error creating data.csv");
    }
  }
}

void loop() {
  logDHTData();
  delay(2000); // Log every 2 seconds
}

void logDHTData() {
  unsigned long currentTime = millis();

  // Read humidity and temperature from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Validate readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Open file for append
  myFile = SD.open("/data.csv", FILE_APPEND);
  if (myFile) {
    myFile.print(currentTime);
    myFile.print(",");
    myFile.print(temperature);
    myFile.print(",");
    myFile.println(humidity);
    myFile.close();

    Serial.print("Logged: ");
    Serial.print(currentTime);
    Serial.print(", ");
    Serial.print(temperature);
    Serial.print("C, ");
    Serial.print(humidity);
    Serial.println("%");
  } else {
    Serial.println("Error opening data.csv");
  }
}

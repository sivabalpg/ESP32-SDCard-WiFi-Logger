#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <WebServer.h>

const char* ssid = "wifiname";
const char* password = "wifipassword@1";

IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

WebServer server(80);
const int CS = 5;

// Root HTML page (responsive)
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CSV Viewer</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        margin: 0;
        padding: 20px;
        background-color: #f4f4f4;
      }
      h2 {
        font-size: 1.8em;
        margin-bottom: 20px;
      }
      button {
        font-size: 1.2em;
        padding: 12px 24px;
        background-color: #0078D7;
        color: white;
        border: none;
        border-radius: 8px;
        cursor: pointer;
      }
      button:hover {
        background-color: #005bb5;
      }
      /* Responsive adjustments */
      @media (max-width: 600px) {
        h2 { font-size: 1.4em; }
        button { width: 80%; font-size: 1em; }
      }
    </style>
  </head>
  <body>
    <h2>Data Logger</h2>
    <a href="/download" download="data.csv">
      <button>Download file</button>
    </a>
  </body>
  </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  File file = SD.open("/data.csv");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  String content;
  while (file.available()) content += (char)file.read();
  file.close();
  server.send(200, "text/plain", content);
}

void handleDownload() {
  File file = SD.open("/data.csv");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  server.streamFile(file, "text/csv");
  file.close();
}

void setup() {
  Serial.begin(115200);

  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! Static IP: ");
  Serial.println(WiFi.localIP());

  if (!SD.begin(CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/download", handleDownload);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}

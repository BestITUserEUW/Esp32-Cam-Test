#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>

#include "esp_camera.h"
#include "WifiKeys.h"
#include "Logger.h"
#include "OV2640.h"

#define WIFI_CONNECTION_TIMEOUT 200 // connection timeout in millis
#define DEFAULT_BAUDRATE 115200

WebServer server(80);
OV2640 cam;

void handleJpgStream(void)
{
    LOG_DEBUG("client connected");
    WiFiClient client = server.client();
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    server.sendContent(response);

    while (1)
    {
        cam.run();
        if (!client.connected()){
            LOG_DEBUG("client disconnected");
            break;
        }
        response = "--frame\r\n";
        response += "Content-Type: image/jpeg\r\n\r\n";
        server.sendContent(response);

        client.write((char *)cam.getfb(), cam.getSize());
        server.sendContent("\r\n");
        if (!client.connected()){
            LOG_DEBUG("client disconnected");
            break;
        }
    }
}

void handleNotFound()
{
    String message = "Server is running!\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    server.send(200, "text/plain", message);
}

// connect to the wifi
void connectWifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    LOG_INFO("connecting to ssid: %s", ssid);
    uint32_t notConnectedCounter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        notConnectedCounter++;
        // Reset board if not connected after 15s
        if (notConnectedCounter > WIFI_CONNECTION_TIMEOUT) {
            LOG_ERROR("reached wifi connection timeout %dms resetting", WIFI_CONNECTION_TIMEOUT);
            ESP.restart();
        }
    }
    LOG_DEBUG("successfully connected to Wifi");
    IPAddress ip = WiFi.localIP();
    LOG_DEBUG("ip: %s", ip.toString().c_str());
}

void setup() {
  Serial.begin(DEFAULT_BAUDRATE);
  while (!Serial)
  {
      ;
  }
  LOG_INFO("booting");
  connectWifi();
  cam.init(esp32CamAiThinkerConfig);
  server.on("/", HTTP_GET, handleJpgStream);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient(); 
}
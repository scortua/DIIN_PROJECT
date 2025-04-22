#include <WiFi.h>        // Include the Wi-Fi library

// Configuración de la WiFi generada
const char *ssid = "ESP32 C6 - DON HONORIO";
const char *password = "1234567890";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

void setup() {
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
}

void loop() { 
  Serial.print("Iniciado AP\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
  Serial.println("\n");
  delay(2000);
}
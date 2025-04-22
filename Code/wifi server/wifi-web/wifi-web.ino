#include <WiFi.h>         // Include the wi-fi library
#include <WebServer.h>    // Include the web server library

// Configuracion de la wifi generada
const char *ssid = "ESP32C6 - DIIN";  // Enter SSID
const char *password = "1234567890";  // Enter password

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80); // web server on port 80

// Global variables for temperature and humidity
float temperature = 25.0;
float humidity = 60.0;

void variables(); // function prototype
void handle_OnConnect(); // function prototype
void handle_NotFound(); // function prototype
String SendHTML(float temperature, float humidity); // updated prototype

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  // server connection
  server.on("/", handle_OnConnect); // handle root url)
  server.onNotFound(handle_NotFound); // handle not found url
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  // Here you would update temperature and humidity from sensors
  variables();
}

void variables() {
  // Simulate reading temperature and humidity
  temperature = random(20, 30); // Random temperature between 20 and 30
  humidity = random(40, 70);    // Random humidity between 40 and 70
  delay(5000); // Simulate a delay for sensor reading
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(temperature, humidity)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature, float humidity){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Plant Environment Monitor</title>\n";
  ptr +="<style>\n";
  ptr +="html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: #f0f7e9; }\n";
  ptr +="body { margin-top: 50px; }\n";
  ptr +="h1 { color: #2e7d32; margin: 50px auto 30px; }\n";
  ptr +="h3 { color: #558b2f; margin-bottom: 30px; }\n";
  ptr +=".dashboard { display: flex; justify-content: center; flex-wrap: wrap; gap: 20px; margin: 30px auto; max-width: 600px; }\n";
  ptr +=".card { background-color: white; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); padding: 20px; width: 200px; }\n";
  ptr +=".temperature { border-left: 5px solid #ff7043; }\n";
  ptr +=".humidity { border-left: 5px solid #42a5f5; }\n";
  ptr +=".value { font-size: 38px; font-weight: bold; margin: 15px 0; }\n";
  ptr +=".temperature .value { color: #e65100; }\n";
  ptr +=".humidity .value { color: #0d47a1; }\n";
  ptr +=".label { color: #7cb342; font-size: 18px; font-weight: bold; }\n";
  ptr +=".unit { font-size: 16px; color: #757575; }\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>MONITOR DE CUIDADOR DE PLANTULAS</h1>\n";
  ptr +="<h3>Medidas en tiempo real de las plantulas</h3>\n";
  
  ptr +="<div class='dashboard'>\n";
  ptr +="  <div class='card temperature'>\n";
  ptr +="    <div class='label'>Temperature</div>\n";
  ptr +="    <div class='value'>" + String(temperature) + "<span class='unit'>°C</span></div>\n";
  ptr +="  </div>\n";
  ptr +="  <div class='card humidity'>\n";
  ptr +="    <div class='label'>Humidity</div>\n";
  ptr +="    <div class='value'>" + String(humidity) + "<span class='unit'>%</span></div>\n";
  ptr +="  </div>\n";
  ptr +="</div>\n";
  
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

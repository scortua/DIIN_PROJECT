#include <WiFi.h>
#include <WebServer.h>

// Access Point credentials
const char* ssid = "LED_Control_AP";
const char* password = "12345678";

// Define the LED pins
const int LED1 = 2;
const int LED2 = 4;
const int LED3 = 16;
const int LED4 = 17;

// LED status variables
bool led1Status = false;
bool led2Status = false;
bool led3Status = false;
bool led4Status = false;

// Create web server object on port 80
WebServer server(80);

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    
    // Initialize LED pins as outputs
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    
    // Set LEDs initial state to OFF
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    
    // Start Access Point
    WiFi.softAP(ssid, password);
    
    // Print Access Point IP address
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    // Define server routes
    server.on("/", handleRoot);
    server.on("/led1", handleLED1);
    server.on("/led2", handleLED2);
    server.on("/led3", handleLED3);
    server.on("/led4", handleLED4);
    server.on("/allOn", handleAllOn);
    server.on("/allOff", handleAllOff);
    
    // Start the server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    // Handle client requests
    server.handleClient();
}

// Function to handle root page
void handleRoot() {
    String html = "<!DOCTYPE html><html>";
    html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    html += "<style>";
    html += "body { font-family: Arial; text-align: center; margin: 20px; }";
    html += ".button { display: inline-block; background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 8px; }";
    html += ".button.off { background-color: #f44336; }";
    html += ".button.all { background-color: #2196F3; }";
    html += "</style></head><body>";
    html += "<h1>ESP32 LED Control</h1>";
    
    // LED 1 button
    html += "<p>LED 1: ";
    if (led1Status) {
        html += "<a href=\"/led1\" class=\"button off\">Turn OFF</a>";
    } else {
        html += "<a href=\"/led1\" class=\"button\">Turn ON</a>";
    }
    html += "</p>";
    
    // LED 2 button
    html += "<p>LED 2: ";
    if (led2Status) {
        html += "<a href=\"/led2\" class=\"button off\">Turn OFF</a>";
    } else {
        html += "<a href=\"/led2\" class=\"button\">Turn ON</a>";
    }
    html += "</p>";
    
    // LED 3 button
    html += "<p>LED 3: ";
    if (led3Status) {
        html += "<a href=\"/led3\" class=\"button off\">Turn OFF</a>";
    } else {
        html += "<a href=\"/led3\" class=\"button\">Turn ON</a>";
    }
    html += "</p>";
    
    // LED 4 button
    html += "<p>LED 4: ";
    if (led4Status) {
        html += "<a href=\"/led4\" class=\"button off\">Turn OFF</a>";
    } else {
        html += "<a href=\"/led4\" class=\"button\">Turn ON</a>";
    }
    html += "</p>";
    
    // All LEDs controls
    html += "<p><a href=\"/allOn\" class=\"button all\">All ON</a>&nbsp;";
    html += "<a href=\"/allOff\" class=\"button all off\">All OFF</a></p>";
    
    html += "</body></html>";
    server.send(200, "text/html", html);
}

// Handle LED1 toggle
void handleLED1() {
    led1Status = !led1Status;
    digitalWrite(LED1, led1Status);
    server.sendHeader("Location", "/");
    server.send(303);
}

// Handle LED2 toggle
void handleLED2() {
    led2Status = !led2Status;
    digitalWrite(LED2, led2Status);
    server.sendHeader("Location", "/");
    server.send(303);
}

// Handle LED3 toggle
void handleLED3() {
    led3Status = !led3Status;
    digitalWrite(LED3, led3Status);
    server.sendHeader("Location", "/");
    server.send(303);
}

// Handle LED4 toggle
void handleLED4() {
    led4Status = !led4Status;
    digitalWrite(LED4, led4Status);
    server.sendHeader("Location", "/");
    server.send(303);
}

// Handle turning all LEDs on
void handleAllOn() {
    led1Status = true;
    led2Status = true;
    led3Status = true;
    led4Status = true;
    
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    
    server.sendHeader("Location", "/");
    server.send(303);
}

// Handle turning all LEDs off
void handleAllOff() {
    led1Status = false;
    led2Status = false;
    led3Status = false;
    led4Status = false;
    
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    
    server.sendHeader("Location", "/");
    server.send(303);
}
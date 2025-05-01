#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "VIVERO DON HONORIO"; // Replace with your network SSID
const char* password = "1234567890"; // Replace with your network password

IPAddress local_IP(192, 168, 1, 1); // Set a static IP address
IPAddress gateway(192, 168, 1, 1); // Set the gateway IP address
IPAddress subnet(255, 255, 255, 0); // Set the subnet mask

WebServer server(80); // Create a web server object that listens for HTTP requests on port 80

#define in1 2 // Pin for the coil1
#define in2 4 // Pin for the coil2
#define in3 16 // Pin for the coil3
#define in4 17 // Pin for the coil4
String step_state = "stop"; // Variable to store the stepper motor state
int ms_step = 2.5; // Delay between steps in milliseconds

#define m1 18 // Pin for the motor1
#define m11 19 // Pin for the motor1
String motorX_state = "stop"; // Variable to store the motorX state

#define m2 3 // Pin for the motor2
#define m22 1 // Pin for the motor2
String motorY_state = "stop"; // Variable to store the motorY state

void pines();
void detener();
void step(int , int , int , int );
void motorDC(int, int);
void handle_OnConnect();
void handle_stepmotor_stop();
void handle_stepmotor_forward();
void handle_stepmotor_backward();
void handle_motorX_stop();
void handle_motorX_forward();
void handle_motorX_backward();
void handle_motorY_stop();
void handle_motorY_forward();
void handle_motorY_backward();
void handle_NotFound();
String sendHTML();

void setup(){
  Serial.begin(115200);
  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  delay(100);
  // Configurar pines
  pines();
  detener();
  // server on
  server.on("/", handle_OnConnect);
  // server on stepper motor
  server.on("/stepmotor/stop", handle_stepmotor_stop);
  server.on("/stepmotor/forward", handle_stepmotor_forward);
  server.on("/stepmotor/backward", handle_stepmotor_backward);
  // server on motorX
  server.on("/motorX/stop", handle_motorX_stop);
  server.on("/motorX/forward", handle_motorX_forward);
  server.on("/motorX/backward", handle_motorX_backward);
  // server on motorY
  server.on("/motorY/stop", handle_motorY_stop);
  server.on("/motorY/forward", handle_motorY_forward);
  server.on("/motorY/backward", handle_motorY_backward);
  // server on not found
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");  
}

void loop(){
  server.handleClient(); // Handle incoming client requests
  if (step_state == "forward") {
    step(in1, in2, in3, in4);
  } else if (step_state == "backward") {
    step(in4, in3, in2, in1);
  } else if (step_state == "stop") {
    detener();
  } 
  if (motorX_state == "forward") {
    motorDC(18, 19);
  } else if (motorX_state == "backward") {
    motorDC(19, 18);
  } else if (motorX_state == "stop") {
    detener();
  }
  if (motorY_state == "forward") {
    motorDC(3, 1);
  } else if (motorY_state == "backward") {
    motorDC(1, 3);
  } else if (motorY_state == "stop") {
    detener();
  }
  if (step_state == "stop" && motorX_state == "stop" && motorY_state == "stop") {
    Serial.println("All motors stopped");
  } else {
    Serial.println("Motors are running");
  }
  delay(100); // Delay to prevent excessive CPU usage
}

void pines() {
  // Configurar pines para el motor de paso
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // Configurar pines para el motor DC X
  pinMode(m1, OUTPUT);
  pinMode(m11, OUTPUT);
  // Configurar pines para el motor DC Y
  pinMode(m2, OUTPUT);
  pinMode(m22, OUTPUT);
}

void detener() {
  // Detener el motor de paso
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  // Detener el motor DC X
  digitalWrite(m1, LOW);
  digitalWrite(m11, LOW);
  // Detener el motor DC Y
  digitalWrite(m2, LOW);
  digitalWrite(m22, LOW);
}

void step(int pin1, int pin2, int pin3, int pin4) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(ms_step);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  delay(ms_step);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  delay(ms_step);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delay(ms_step);
}

void motorDC(int pin1, int pin2) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
}

void handle_OnConnect(){
  Serial.println("Client connected");
  server.send(200, "text/html", sendHTML());
}
/* ------------------------Motor de paso------------------------- */
void handle_stepmotor_stop(){
  step_state = "stop";
  Serial.println("Motor stopped");
  server.send(200, "text/html", sendHTML());
}
void handle_stepmotor_forward(){
  step_state = "forward";
  Serial.println("Motor forward");
  server.send(200, "text/html", sendHTML());
}
void handle_stepmotor_backward(){
  step_state = "backward";
  Serial.println("Motor backward");
  server.send(200, "text/html", sendHTML());
}
/*-------------------------Motor dc x------------------------- */
void handle_motorX_stop(){
  step_state = "stop";
  Serial.println("MotorX stopped");
  server.send(200, "text/html", sendHTML());
}
void handle_motorX_forward(){
  step_state = "forward";
  Serial.println("MotorX forward");
  server.send(200, "text/html", sendHTML());
}
void handle_motorX_backward(){
  step_state = "backward";
  Serial.println("MotorX backward");
  server.send(200, "text/html", sendHTML());
}
/*-------------------------Motor dc y------------------------- */
void handle_motorY_stop(){
  step_state = "stop";
  Serial.println("MotorY stopped");
  server.send(200, "text/html", sendHTML());
}
void handle_motorY_forward(){
  step_state = "forward";
  Serial.println("MotorY forward");
  server.send(200, "text/html", sendHTML());
}
void handle_motorY_backward(){
  step_state = "backward";
  Serial.println("MotorY backward");
  server.send(200, "text/html", sendHTML());
}

void handle_NotFound(){
  server.send(404, "text/plain", "404: Not found");
}

String sendHTML(){
  String html = "<!DOCTYPE html><html>";
  html += "<head><style>";
  html += "body { font-family: Arial; text-align: center; margin-top: 50px; }";
  html += ".motor-status { font-size: 24px; font-weight: bold; margin: 20px; padding: 15px; border-radius: 10px; }";
  html += ".running { background-color: #4CAF50; color: white; }";
  html += ".stopped { background-color: #f44336; color: white; }";
  html += "button { background-color: #2196F3; color: white; padding: 10px 20px; border: none; border-radius: 5px; margin: 10px; cursor: pointer; }";
  html += "button:hover { opacity: 0.8; }";
  html += "</style>";
  html += "<script>";
  html += "function toggleMotor(direction) {";
  html += "  var currentState = document.getElementById('motorState').textContent;";
  html += "  var newState = '';";
  html += "  if (currentState.includes(direction)) {";
  html += "    window.location.href = '/motor/stop';";
  html += "  } else {";
  html += "    window.location.href = '/motor/' + direction;";
  html += "  }";
  html += "}";
  html += "</script></head>";
  html += "<body><h1>ESP32 Motor Control</h1>";
  
  // Motor status display
  if (step_state == "stop") {
    html += "<div class='motor-status stopped'>";
    html += "<span id='motorState'>Motor is STOPPED</span>";
    html += "</div>";
  } else if (step_state == "forward") {
    html += "<div class='motor-status running'>";
    html += "<span id='motorState'>Motor is FORWARD</span>";
    html += "</div>";
  } else if (step_state == "backward") {
    html += "<div class='motor-status running'>";
    html += "<span id='motorState'>Motor is BACKWARD</span>";
    html += "</div>";
  }
  
  // Control buttons
  html += "<div>";
  html += "<p>Motor Forward: ";
  if (step_state == "forward") {
    html += "ON</p><a class=\"button button-off\" href=\"/stepmotor/stop\">OFF</a>\n";
  } else {
    html += "OFF</p><a class=\"button button-on\" href=\"/stepmotor/forward\">ON</a>\n";
  }
  
  html += "<p>Motor Backward: ";
  if (step_state == "backward") {
    html += "ON</p><a class=\"button button-off\" href=\"/stepmotor/stop\">OFF</a>\n";
  } else {
    html += "OFF</p><a class=\"button button-on\" href=\"/stepmotor/backward\">ON</a>\n";
  }
  html += "</div>";
  
  html += "</body></html>";
  return html;
}

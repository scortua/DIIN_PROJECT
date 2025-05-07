#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "VIVERO DON HONORIO"; // Replace with your network SSID
const char* password = "1234567890"; // Replace with your network password

IPAddress local_IP(192, 168, 1, 1); // Set a static IP address
IPAddress gateway(192, 168, 1, 1); // Set the gateway IP address
IPAddress subnet(255, 255, 255, 0); // Set the subnet mask

WebServer server(80); // Create a web server object that listens for HTTP requests on port 80

#define DHTPIN 23 // Pin where the DHT22 is connected
#define DHTTYPE DHT22 // Define the type of DHT sensor
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object
float tempHistory[10] = {10,20,30,22,25,20,30,14,14,12}; // Array to store temperature history
float humHistory[10] = {50,30,70,80,50,30,70,80,20,50}; // Array to store humidity history
float maxtemp = 0, mintemp = 100, maxhum = 0, minhum = 100, avgTemp = 0, avgHum = 0; // Variables to store max and min values

#define in1 15 // Pin for the coil1
#define in2 2 // Pin for the coil2
#define in3 4 // Pin for the coil3
#define in4 16 // Pin for the coil4
String step_state = "stop"; // Variable to store the stepper motor state
int ms_step = 2.5; // Delay between steps in milliseconds

#define m1 22 // Pin for the motor1
#define m11 23 // Pin for the motor1
String motorX_state = "stop"; // Variable to store the motorX state

#define m2 18 // Pin for the motor2
#define m22 19 // Pin for the motor2
String motorY_state = "stop"; // Variable to store the motorY state

#define pump 13
String pump_state = "stop"; // Variable to store the pump state

void pines();
void detener_step();
void detener_m1();
void detener_m2();
void step(int , int , int , int );
void motorDC(int, int);
void variables();
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
void handle_ia();
void handle_NotFound();
String sendHTML();

void setup(){
  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  delay(100);
  dht.begin(); // Initialize the DHT sensor
  // Configurar pines
  pines();
  detener_step();
  detener_m1();
  detener_m2();
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
  // server on IA
  server.on("/ia", handle_ia);
  // server on not found
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");  
}

void loop(){
  server.handleClient(); // Handle incoming client requests
  variables(); // Read DHT sensor data
  if (step_state == "forward") {
    step(in1, in2, in3, in4);
  } else if (step_state == "backward") {
    step(in4, in3, in2, in1);
  } else if (step_state == "stop") {
    detener_step();
  } 
  if (motorX_state == "forward") {
    motorDC(m1, m11);
  } else if (motorX_state == "backward") {
    motorDC(m11, m1);
  } else if (motorX_state == "stop") {
    detener_m1();
  }
  if (motorY_state == "forward") {
    motorDC(m2, m22);
  } else if (motorY_state == "backward") {
    motorDC(m22, m2);
  } else if (motorY_state == "stop") {
    detener_m2();
  }
  if (pump_state == "go") {
    digitalWrite(pump, HIGH);
  } else if (pump_state == "stop") {
    digitalWrite(pump, LOW);
  }
  delay(10); // Delay to prevent excessive CPU usage
}
/*-------------------------Funciones de manejo de pines------------------------- */
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
  // Configurar pump
  pinMode(pump, OUTPUT);
}
/*-------------------------Funciones de manejo de motores------------------------- */
void detener_step() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
void detener_m1() {
  digitalWrite(m1, LOW);
  digitalWrite(m11, LOW);
}
void detener_m2() {
  digitalWrite(m2, LOW);
  digitalWrite(m22, LOW);
}
/*-------------------------Funciones de manejo de motores------------------------- */
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
/*-------------------------Funciones de manejo de motores------------------------- */
void motorDC(int pin1, int pin2) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
}
/*-------------------------Funciones de manejo de variables------------------------- */
void variables() {
  // Read temperature and humidity from DHT sensor
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();
  float h = random(30, 80); // Simulated humidity
  float t = random(5, 30); // Simulated temperature
  // Store the readings in history arrays
  for (int i = 9; i > 0; i--) {
    tempHistory[i] = tempHistory[i - 1];
    humHistory[i] = humHistory[i - 1];
  }
  tempHistory[0] = t;
  humHistory[0] = h;
  for (int i=0; i<10; i++){
    if (tempHistory[i] > maxtemp) {
      maxtemp = tempHistory[i];
    }
    if (tempHistory[i] < mintemp) {
      mintemp = tempHistory[i];
    }
    if (humHistory[i] > maxhum) {
      maxhum = humHistory[i];
    }
    if (humHistory[i] < minhum) {
      minhum = humHistory[i];
    }
  }
  avgTemp = (tempHistory[0] + tempHistory[1] + tempHistory[2] + tempHistory[3] + tempHistory[4] + tempHistory[5] + tempHistory[6] + tempHistory[7] + tempHistory[8] + tempHistory[9]) / 10;
  avgHum = (humHistory[0] + humHistory[1] + humHistory[2] + humHistory[3] + humHistory[4] + humHistory[5] + humHistory[6] + humHistory[7] + humHistory[8] + humHistory[9]) / 10;
}
/*-------------------------Funciones de manejo de peticiones------------------------- */
void handle_OnConnect(){
  server.send(200, "text/html", sendHTML());
}
/* ------------------------Motor de paso------------------------- */
void handle_stepmotor_stop(){
  step_state = "stop";
  server.send(200, "text/html", sendHTML());
}
void handle_stepmotor_forward(){
  step_state = "forward";
  server.send(200, "text/html", sendHTML());
}
void handle_stepmotor_backward(){
  step_state = "backward";
  server.send(200, "text/html", sendHTML());
}
/*-------------------------Motor dc x------------------------- */
void handle_motorX_stop(){
  motorX_state = "stop";
  server.send(200, "text/html", sendHTML());
}
void handle_motorX_forward(){
  motorX_state = "forward";
  server.send(200, "text/html", sendHTML());
}
void handle_motorX_backward(){
  motorX_state = "backward";
  server.send(200, "text/html", sendHTML());
}
/*-------------------------Motor dc y------------------------- */
void handle_motorY_stop(){
  motorY_state = "stop";
  server.send(200, "text/html", sendHTML());
}
void handle_motorY_forward(){
  motorY_state = "forward";
  server.send(200, "text/html", sendHTML());
}
void handle_motorY_backward(){
  motorY_state = "backward";
  server.send(200, "text/html", sendHTML());
}
/*-------------------------IA------------------------- */
void handle_ia(){
  // Toggle pump state between "go" and "stop"
  if (pump_state == "go") {
    pump_state = "stop";
  } else {
    pump_state = "go";
  }
  server.send(200, "text/html", sendHTML());
}
/*-------------------------Not found------------------------- */
void handle_NotFound(){
  server.send(404, "text/plain", "404: Not found");
}
/*-------------------------HTML------------------------- */
String sendHTML(){
  String html = "<!DOCTYPE html>\n";
  html += "<html lang=\"es\">";
  html += "<meta charset='UTF-8'>\n";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>\n";
  html += "<head>\n";
  html += "<title>Vivero Don Honorio</title>\n";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>\n";
  html += "<style>\n";
  html += "body { font-family: Arial, sans-serif; margin: 20px; text-align: center; background-image: url('data:image/svg+xml,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100\" height=\"100\" viewBox=\"0 0 100 100\"><text x=\"10\" y=\"50\" font-size=\"40\" opacity=\"0.5\">🪴</text></svg>'); background-repeat: repeat;}\n";
  html += "h1 { color: #4CAF50; }\n";
  html += "h2 { color: #2196F3; }\n";
  html += ".section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 10px; }\n";
  html += ".data { font-size: 18px; margin: 10px 0; }\n";
  html += ".data-section { display: inline-block; width: 45%; vertical-align: top; margin: 10px; }\n";
  html += ".data-title { font-weight: bold; font-size: 20px; margin-bottom: 10px; color: #2196F3; }\n";
  html += ".bold-data { font-weight: bold; }\n";
  html += ".control-panel { display: flex; flex-wrap: wrap; justify-content: center; gap: 15px; margin-bottom: 20px; }\n";
  html += "button { background-color: #4CAF50; color: white; padding: 15px 25px; border: none; border-radius: 8px; cursor: pointer; font-size: 18px; font-weight: bold; width: 120px; }\n";
  html += "button:hover { background-color: #45a049; transform: scale(1.05); }\n";
  html += ".stop { background-color: #f44336; }\n";
  html += ".stop:hover { background-color: #d32f2f; }\n";
  html += ".forward { background-color: #2196F3; }\n";
  html += ".backward { background-color: #ff9800; }\n";
  html += ".water { background-color: #00bcd4; }\n";
  html += ".go { background-color: #00bcd4; }\n";
  html += "</style>\n";
  html += "</head>\n";
  html += "<body>\n";
  
  html += "<h1>Vivero Don Honorio</h1>\n";
  
  // Future section placeholder
  html += "<div class='section'>\n";
  html += "<h2>Información del Sistema</h2>\n";
  html += "<div style='width:100%; height:200px; position:relative; margin-bottom:20px;'>\n";
  html += "<canvas id='chart' width='100%' height='200'></canvas>\n";
  html += "</div>\n";
  html += "<script>\n";
  html += "var ctx = document.getElementById('chart').getContext('2d');\n";
  html += "var tempData = [" + String(tempHistory[0]);
  for(int i=1; i<10; i++) {
    html += "," + String(tempHistory[i]);
  }
  html += "];\n";
  html += "var humData = [" + String(humHistory[0]);
  for(int i=1; i<10; i++) {
    html += "," + String(humHistory[i]);
  }
  html += "];\n";
  html += "var maxValue = Math.max(...tempData, ...humData) + 10;\n";
  html += "ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);\n";
  html += "var width = ctx.canvas.width;\n";
  html += "var height = ctx.canvas.height;\n";
  html += "var stepX = width / 9;\n";
  html += "var stepY = height / maxValue;\n";
  
  // Draw grid lines
  html += "ctx.strokeStyle = '#ddd';\n";
  html += "ctx.beginPath();\n";
  html += "for(var i=0; i<=10; i++) {\n";
  html += "  ctx.moveTo(0, i*height/10);\n";
  html += "  ctx.lineTo(width, i*height/10);\n";
  html += "}\n";
  html += "for(var i=0; i<=10; i++) {\n";
  html += "  ctx.moveTo(i*width/10, 0);\n";
  html += "  ctx.lineTo(i*width/10, height);\n";
  html += "}\n";
  html += "ctx.stroke();\n";
  
  // Draw temperature line
  html += "ctx.strokeStyle = '#FF9800';\n"; // Orange for temperature
  html += "ctx.lineWidth = 3;\n";
  html += "ctx.beginPath();\n";
  html += "ctx.moveTo(0, height - tempData[9] * stepY);\n";
  html += "for(var i=8; i>=0; i--) {\n";
  html += "  ctx.lineTo((9-i) * stepX, height - tempData[i] * stepY);\n";
  html += "}\n";
  html += "ctx.stroke();\n";
  
  // Draw humidity line
  html += "ctx.strokeStyle = '#795548';\n"; // Brown for humidity
  html += "ctx.beginPath();\n";
  html += "ctx.moveTo(0, height - humData[9] * stepY);\n";
  html += "for(var i=8; i>=0; i--) {\n";
  html += "  ctx.lineTo((9-i) * stepX, height - humData[i] * stepY);\n";
  html += "}\n";
  html += "ctx.stroke();\n";
  
  // Add legend
  html += "ctx.font = '12px Arial';\n";
  html += "ctx.fillStyle = '#FF9800';\n";
  html += "ctx.fillRect(10, 10, 15, 10);\n";
  html += "ctx.fillStyle = '#000';\n";
  html += "ctx.fillText('Temperatura', 30, 20);\n";
  html += "ctx.fillStyle = '#795548';\n";
  html += "ctx.fillRect(120, 10, 15, 10);\n";
  html += "ctx.fillStyle = '#000';\n";
  html += "ctx.fillText('Humedad', 140, 20);\n";
  html += "</script>\n";
  html += "</div>\n";
  
  // Environmental data section
  html += "<div class='section'>\n";
  html += "<h2>Datos Ambientales</h2>\n";
  
  // Temperature data section
  html += "<div class='data-section'>\n";
  html += "<div class='data-title'>Temperatura</div>\n";
  html += "<div class='data'><span class='bold-data'>Actual:</span> " + String(tempHistory[0]) + " °C</div>\n";
  html += "<div class='data'><span class='bold-data'>Máxima:</span> " + String(maxtemp) + " °C</div>\n";
  html += "<div class='data'><span class='bold-data'>Mínima:</span> " + String(mintemp) + " °C</div>\n";
  html += "<div class='data'><span class='bold-data'>Promedio:</span> " + String(avgTemp) + " °C</div>\n";
  html += "</div>\n";
  
  // Humidity data section
  html += "<div class='data-section'>\n";
  html += "<div class='data-title'>Humedad</div>\n";
  html += "<div class='data'><span class='bold-data'>Actual:</span> " + String(humHistory[0]) + " %</div>\n";
  html += "<div class='data'><span class='bold-data'>Máxima:</span> " + String(maxhum) + " %</div>\n";
  html += "<div class='data'><span class='bold-data'>Mínima:</span> " + String(minhum) + " %</div>\n";
  html += "<div class='data'><span class='bold-data'>Promedio:</span> " + String(avgHum) + " %</div>\n";
  html += "</div>\n";
  
  html += "</div>\n";
  
  // Control panel section
  html += "<div class='section'>\n";
  html += "<h2>Panel de Control</h2>\n";
  
  // X-axis controls
  html += "<div class='control-panel'>\n";
  html += "<h3>Eje X:</h3>\n";
  html += "<a href=\"/motorX/backward\"><button class=\"backward\">Atrás</button></a>\n";
  html += "<a href=\"/motorX/stop\"><button class=\"stop\">Detener</button></a>\n";
  html += "<a href=\"/motorX/forward\"><button class=\"forward\">Adelante</button></a>\n";
  html += "</div>\n";
  
  // Y-axis controls
  html += "<div class='control-panel'>\n";
  html += "<h3>Eje Y:</h3>\n";
  html += "<a href=\"/motorY/backward\"><button class=\"backward\">Atrás</button></a>\n";
  html += "<a href=\"/motorY/stop\"><button class=\"stop\">Detener</button></a>\n";
  html += "<a href=\"/motorY/forward\"><button class=\"forward\">Adelante</button></a>\n";
  html += "</div>\n";
  
  // Stepper motor (Z-axis) controls
  html += "<div class='control-panel'>\n";
  html += "<h3>Eje Z:</h3>\n";
  html += "<a href=\"/stepmotor/backward\"><button class=\"backward\">Bajar</button></a>\n";
  html += "<a href=\"/stepmotor/stop\"><button class=\"stop\">Detener</button></a>\n";
  html += "<a href=\"/stepmotor/forward\"><button class=\"forward\">Subir</button></a>\n";
  html += "</div>\n";
  
  // Water control
  html += "<div class='control-panel'>\n";
  html += "<h3>Riego:</h3>\n";
  if (pump_state == "go") {
    html += "<a href=\"/ia\"><button class=\"stop\">Detener Riego</button></a>\n";
  } else {
    html += "<a href=\"/ia\"><button class=\"go\">Iniciar Riego</button></a>\n";
  }
  html += "</div>\n";
  
  html += "</div>\n";
  
  html += "</body>\n";
  // Footer section with contact info
  html += "<style>\n";
  html += ".section { background-color: #f5f5f5; } /* Light gray background for sections */\n";
  html += ".footer-container { margin-top: 20px; padding: 15px; background-color: #4CAF50; color: white; border-radius: 10px; }\n";
  html += ".footer-title { font-weight: bold; font-size: 22px; margin-bottom: 15px; color: white; }\n";
  html += ".footer-content { display: flex; flex-wrap: wrap; justify-content: space-around; gap: 15px; margin-bottom: 15px; }\n";
  html += ".footer-item { margin: 5px 0; }\n";
  html += ".footer-copyright { font-size: 14px; color: #e8f5e9; margin-top: 15px; }\n";
  html += "</style>\n";
  
  html += "<div class='footer-container'>\n";
  html += "  <div class='footer-title'>CONTACTO</div>\n";
  html += "  <div class='footer-content'>\n";
  html += "    <div class='footer-item'>\n";
  html += "      <div style='font-weight: bold;'>Desarrollado por:</div>\n";
  html += "      <div>Grupo Vivero Don Honorio</div>\n";
  html += "    </div>\n";
  html += "    <div class='footer-item'>\n";
  html += "      <div style='font-weight: bold;'>Correo:</div>\n";
  html += "      <div>contacto@viverodonhonorio.com</div>\n";
  html += "    </div>\n";
  html += "    <div class='footer-item'>\n";
  html += "      <div style='font-weight: bold;'>Teléfono:</div>\n";
  html += "      <div>+57 301 234 5678</div>\n";
  html += "    </div>\n";
  html += "  </div>\n";
  html += "  <div class='footer-copyright'>\n";
  html += "    © 2025 Vivero Don Honorio - Todos los derechos reservados\n";
  html += "  </div>\n";
  html += "</div>\n";
  
  html += "</html>\n";
  
  return html;
}

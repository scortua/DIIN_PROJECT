#include <WiFi.h>         // Include the wi-fi library
#include <WebServer.h>    // Include the web server library

// Configuracion de la wifi generada
const char *ssid = "ESP32C6 - DIIN";  // Enter SSID
const char *password = "1234567890";  // Enter password

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80); // web server on port 80

// Arrays to store historical data (10 elements each)
float tempHistory[10] = {22, 23, 25, 24, 22, 26, 27, 25, 24, 23};
float humHistory[10] = {65, 70, 68, 72, 75, 65, 62, 64, 68, 72};
float maxTemp = 0, minTemp = 100, avgTemp = 0;
float maxHum = 0, minHum = 100, avgHum = 0;

void variables(); // function prototype
void handle_OnConnect(); // function prototype
void handle_NotFound(); // function prototype
String SendHTML(); // updated prototype

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
  // Generate random temperature and humidity values
  float temperature = random(5, 30); // Random temperature between 20 and 30
  float humidity = random(30, 80);    // Random humidity between 40 and 70
  
  // Shift values in the temperature history array
  for (int i = 9; i > 0; i--) {
    tempHistory[i] = tempHistory[i-1];
  }
  tempHistory[0] = temperature;  // Add new value at position 0
  
  // Shift values in the humidity history array
  for (int i = 9; i > 0; i--) {
    humHistory[i] = humHistory[i-1];
  }
  humHistory[0] = humidity;  // Add new value at position 0
  // Calculate max, min, and average for temperature
  for (int i = 0; i < 10; i++) {
    if (tempHistory[i] > maxTemp) {
      maxTemp = tempHistory[i];
    }
    if (tempHistory[i] < minTemp) {
      minTemp = tempHistory[i];
    }
    avgTemp += tempHistory[i];
  }
  avgTemp /= 10; // Calculate average temperature
  // Calculate max, min, and average for humidity
  for (int i = 0; i < 10; i++) {
    if (humHistory[i] > maxHum) {
      maxHum = humHistory[i];
    }
    if (humHistory[i] < minHum) {
      minHum = humHistory[i];
    }
    avgHum += humHistory[i];
  }
  avgHum /= 10; // Calculate average humidity
  // Print values to Serial Monitor
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("\t");
  Serial.print(maxTemp);
  Serial.print("°C-\t");
  Serial.print(minTemp);
  Serial.print("°C-\t");
  Serial.print(avgTemp);
  Serial.print("°C\t\tHum: ");
  Serial.print(humidity);
  Serial.print("\t");
  Serial.print(maxHum);
  Serial.print("%-\t");
  Serial.print(minHum);
  Serial.print("%-\t");
  Serial.print(avgHum);
  Serial.println("%");
  delay(5000); // Simulate a delay for sensor reading
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
  String ptr = "<!DOCTYPE html>\n";
  ptr += "<html lang=\"es\">\n";
  ptr += "<head>\n";
  ptr += "    <meta charset=\"UTF-8\">\n";
  ptr += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  ptr += "    <title>Vivero Don Honorio - Sistema de Monitoreo</title>\n";
  ptr += "    <style>\n";
  ptr += "        body {\n";
  ptr += "            font-family:'Arial Black', sans-serif;\n";
  ptr += "            background-color: #f0f8f0;\n";
  ptr += "            margin: 0;\n";
  ptr += "            padding: 0;\n";
  ptr += "            background-image: url('data:image/svg+xml,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100\" height=\"100\" viewBox=\"0 0 100 100\"><text x=\"10\" y=\"50\" font-size=\"40\" opacity=\"0.5\">🪴</text></svg>');\n";
  ptr += "            background-repeat: repeat;\n";
  ptr += "        }\n";
  ptr += "        .container {\n";
  ptr += "            max-width: 800px;\n";
  ptr += "            margin: 20px auto;\n";
  ptr += "        }\n";
  ptr += "        header {\n";
  ptr += "            display: flex;\n";
  ptr += "            justify-content: space-between;\n";
  ptr += "            align-items: center;\n";
  ptr += "            background-color: #e0e0e0;\n";
  ptr += "            padding: 10px 15px;\n";
  ptr += "            border-radius: 10px 10px 0 0;\n";
  ptr += "        }\n";
  ptr += "        .header-title {\n";
  ptr += "            text-align: right;\n";
  ptr += "            flex-grow: 1;\n";
  ptr += "            font-weight: bold;\n";
  ptr += "            color: #333;\n";
  ptr += "        }\n";
  ptr += "        .switch {\n";
  ptr += "            position: relative;\n";
  ptr += "            display: inline-block;\n";
  ptr += "            width: 60px;\n";
  ptr += "            height: 30px;\n";
  ptr += "        }\n";
  ptr += "        .switch input {\n";
  ptr += "            opacity: 0;\n";
  ptr += "            width: 0;\n";
  ptr += "            height: 0;\n";
  ptr += "        }\n";
  ptr += "        .slider {\n";
  ptr += "            position: absolute;\n";
  ptr += "            cursor: pointer;\n";
  ptr += "            top: 0;\n";
  ptr += "            left: 0;\n";
  ptr += "            right: 0;\n";
  ptr += "            bottom: 0;\n";
  ptr += "            background-color: #ccc;\n";
  ptr += "            transition: .4s;\n";
  ptr += "            border-radius: 34px;\n";
  ptr += "        }\n";
  ptr += "        .slider:before {\n";
  ptr += "            position: absolute;\n";
  ptr += "            content: \"\";\n";
  ptr += "            height: 22px;\n";
  ptr += "            width: 22px;\n";
  ptr += "            left: 4px;\n";
  ptr += "            bottom: 4px;\n";
  ptr += "            background-color: white;\n";
  ptr += "            transition: .4s;\n";
  ptr += "            border-radius: 50%;\n";
  ptr += "        }\n";
  ptr += "        input:checked + .slider {\n";
  ptr += "            background-color: #009688;\n";
  ptr += "        }\n";
  ptr += "        input:checked + .slider:before {\n";
  ptr += "            transform: translateX(30px);\n";
  ptr += "        }\n";
  ptr += "        .title-bar {\n";
  ptr += "            background-color: #009688;\n";
  ptr += "            color: white;\n";
  ptr += "            text-align: center;\n";
  ptr += "            padding: 10px;\n";
  ptr += "            font-weight: bold;\n";
  ptr += "        }\n";
  ptr += "        .panel {\n";
  ptr += "            background-color: white;\n";
  ptr += "            border-radius: 10px;\n";
  ptr += "            margin-bottom: 20px;\n";
  ptr += "            padding: 15px;\n";
  ptr += "            box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
  ptr += "        }\n";
  ptr += "        .dashboard {\n";
  ptr += "            display: flex;\n";
  ptr += "            justify-content: space-between;\n";
  ptr += "        }\n";
  ptr += "        .chart-container {\n";
  ptr += "            width: 48%;\n";
  ptr += "            border: 1px solid #009688;\n";
  ptr += "            border-radius: 5px;\n";
  ptr += "            padding: 5px;\n";
  ptr += "        }\n";
  ptr += "        .data-container {\n";
  ptr += "            width: 48%;\n";
  ptr += "        }\n";
  ptr += "        .data-section {\n";
  ptr += "            background-color: #009688;\n";
  ptr += "            color: white;\n";
  ptr += "            margin-bottom: 10px;\n";
  ptr += "            border-radius: 5px;\n";
  ptr += "            padding: 5px;\n";
  ptr += "        }\n";
  ptr += "        .data-section-title {\n";
  ptr += "            font-weight: bold;\n";
  ptr += "            text-align: center;\n";
  ptr += "            margin-bottom: 5px;\n";
  ptr += "        }\n";
  ptr += "        .data-line {\n";
  ptr += "            background-color: white;\n";
  ptr += "            margin: 5px 0;\n";
  ptr += "            border-radius: 3px;\n";
  ptr += "            padding: 5px;\n";
  ptr += "            color: #333;\n";
  ptr += "            font-size: 14px;\n";
  ptr += "        }\n";
  ptr += "        .actions {\n";
  ptr += "            text-align: center;\n";
  ptr += "        }\n";
  ptr += "        .action-title {\n";
  ptr += "            background-color: #009688;\n";
  ptr += "            color: white;\n";
  ptr += "            padding: 8px;\n";
  ptr += "            border-radius: 5px;\n";
  ptr += "            margin-bottom: 15px;\n";
  ptr += "        }\n";
  ptr += "        .action-buttons {\n";
  ptr += "            display: flex;\n";
  ptr += "            justify-content: space-around;\n";
  ptr += "            align-items: center;\n";
  ptr += "        }\n";
  ptr += "        .action-button {\n";
  ptr += "            display: flex;\n";
  ptr += "            flex-direction: column;\n";
  ptr += "            align-items: center;\n";
  ptr += "            margin-bottom: 10px;\n";
  ptr += "        }\n";
  ptr += "        .circle-button {\n";
  ptr += "            width: 45px;\n";
  ptr += "            height: 45px;\n";
  ptr += "            border-radius: 50%;\n";
  ptr += "            border: none;\n";
  ptr += "            margin-bottom: 5px;\n";
  ptr += "            cursor: pointer;\n";
  ptr += "        }\n";
  ptr += "        .circle-button.large {\n";
  ptr += "            width: 50px;\n";
  ptr += "            height: 50px;\n";
  ptr += "        }\n";
  ptr += "        .button-text {\n";
  ptr += "            background-color: #009688;\n";
  ptr += "            color: white;\n";
  ptr += "            padding: 5px 15px;\n";
  ptr += "            border-radius: 15px;\n";
  ptr += "            font-size: 12px;\n";
  ptr += "        }\n";
  ptr += "        .footer-lines {\n";
  ptr += "            background-color: #e0e0e0;\n";
  ptr += "            height: 10px;\n";
  ptr += "            margin: 5px 0;\n";
  ptr += "            border-radius: 3px;\n";
  ptr += "        }\n";
  ptr += "    </style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "    <div class=\"container\">\n";
  ptr += "        <!-- Header section -->\n";
  ptr += "        <header>\n";
  ptr += "            <label class=\"switch\">\n";
  ptr += "                <input type=\"checkbox\">\n";
  ptr += "                <span class=\"slider\"></span>\n";
  ptr += "            </label>\n";
  ptr += "            <div class=\"header-title\">VIVERO DON HONORIO</div>\n";
  ptr += "        </header>\n";
  ptr += "        <!-- Main title -->\n";
  ptr += "        <div class=\"title-bar\">DATOS DEL VIVERO</div>\n";
  ptr += "        <!-- Top panel - Dashboard -->\n";
  ptr += "        <div class=\"panel\">\n";
  ptr += "            <div class=\"dashboard\">\n";
  ptr += "                <!-- Chart section on the left -->\n";
  ptr += "                <div class=\"chart-container\">\n";
  ptr += "                    <canvas id=\"dataChart\" width=\"370\" height=\"200\"></canvas>\n";
  ptr += "                </div>\n";
  ptr += "                <!-- Data section on the right -->\n";
  ptr += "                <div class=\"data-container\">\n";
  ptr += "                    <!-- Measurements section -->\n";
  ptr += "                    <div class=\"data-section\">\n";
  ptr += "                        <div class=\"data-section-title\">MEDIDAS</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"temperatura\">Temperatura = " + String(tempHistory[0]) + "°C</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"humedad\">Humedad = " + String(humHistory[0]) + "%</div>\n";
  ptr += "                    </div>\n";
  ptr += "                    <!-- Statistics section -->\n";
  ptr += "                    <div class=\"data-section\">\n";
  ptr += "                        <div class=\"data-section-title\">ESTADÍSTICAS</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"tempStats\" style=\"text-align: center; font-family: 'Arial Black', sans-serif; font-size: 18px;\"><strong>Temperatura</strong></div>\n";
  ptr += "                        <div class=\"data-line\" id=\"tempMaxStats\" style=\"text-align: center;\">📈Máx=" + String(maxTemp) + "</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"tempMinStats\" style=\"text-align: center;\">📉Mín=" + String(minTemp) + "</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"tempAvgStats\" style=\"text-align: center;\">🌡️Prom=" + String(avgTemp) + "</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"humStats\" style=\"text-align: center; font-family: 'Verdana', sans-serif; font-size: 18px;\"><strong>Humedad</strong></div>\n";
  ptr += "                        <div class=\"data-line\" id=\"humMaxStats\" style=\"text-align: center;\">📈Máx=" + String(maxHum) + "</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"humMinStats\" style=\"text-align: center;\">📉Mín=" + String(minHum) + "</div>\n";
  ptr += "                        <div class=\"data-line\" id=\"humAvgStats\" style=\"text-align: center;\">💧Prom=" + String(avgHum) + "</div>\n";
  ptr += "                    </div>\n";
  ptr += "                </div>\n";
  ptr += "            </div>\n";
  ptr += "        </div>\n";
  ptr += "        <!-- Bottom panel - Actions -->\n";
  ptr += "        <div class=\"panel actions\">\n";
  ptr += "            <div class=\"action-title\">ACCIONES</div>\n";
  ptr += "            <div class=\"action-buttons\">\n";
  ptr += "                <!-- Yellow button -->\n";
  ptr += "                <div class=\"action-button\">\n";
  ptr += "                    <button class=\"circle-button\" style=\"background-color: yellow;\"></button>\n";
  ptr += "                    <div class=\"button-text\">TEMPERATURA</div>\n";
  ptr += "                </div>\n";
  ptr += "                <!-- Blue button -->\n";
  ptr += "                <div class=\"action-button\">\n";
  ptr += "                    <button class=\"circle-button\" style=\"background-color: #00b0ff;\"></button>\n";
  ptr += "                    <div class=\"button-text\">HUMEDAD</div>\n";
  ptr += "                </div>\n";
  ptr += "                <!-- Red button (larger) -->\n";
  ptr += "                <div class=\"action-button\">\n";
  ptr += "                    <button class=\"circle-button large\" style=\"background-color: red;\"></button>\n";
  ptr += "                    <div class=\"button-text\">DETENER</div>\n";
  ptr += "                </div>\n";
  ptr += "                <!-- Orange button -->\n";
  ptr += "                <div class=\"action-button\">\n";
  ptr += "                    <button class=\"circle-button\" style=\"background-color: orange;\"></button>\n";
  ptr += "                    <div class=\"button-text\">CONEXION</div>\n";
  ptr += "                </div>\n";
  ptr += "                <!-- Green button -->\n";
  ptr += "                <div class=\"action-button\">\n";
  ptr += "                    <button class=\"circle-button\" style=\"background-color: lime;\"></button>\n";
  ptr += "                    <div class=\"button-text\">CALIBRAR</div>\n";
  ptr += "                </div>\n";
  ptr += "            </div>\n";
  ptr += "        </div>\n";
  ptr += "        <!-- Footer with contact info -->\n";
  ptr += "        <div class=\"panel\" style=\"background-color: #009688; color: white;\">\n";
  ptr += "            <div style=\"text-align: center; padding: 10px; font-size: 18px; font-weight: bold;\">\n";
  ptr += "            CONTACTO\n";
  ptr += "            </div>\n";
  ptr += "            <div style=\"display: flex; justify-content: space-around; flex-wrap: wrap;\">\n";
  ptr += "            <div style=\"margin: 10px; text-align: center;\">\n";
  ptr += "                <div style=\"font-weight: bold;\">Desarrollado por:</div>\n";
  ptr += "                <div>Grupo Vivero Don Honorio</div>\n";
  ptr += "            </div>\n";
  ptr += "            <div style=\"margin: 10px; text-align: center;\">\n";
  ptr += "                <div style=\"font-weight: bold;\">Correo:</div>\n";
  ptr += "                <div>contacto@viverodonhonorio.com</div>\n";
  ptr += "            </div>\n";
  ptr += "            <div style=\"margin: 10px; text-align: center;\">\n";
  ptr += "                <div style=\"font-weight: bold;\">Teléfono:</div>\n";
  ptr += "                <div>+57 301 234 5678</div>\n";
  ptr += "            </div>\n";
  ptr += "            </div>\n";
  ptr += "            <div style=\"text-align: center; font-size: 14px; margin-top: 10px;\">\n";
  ptr += "            © 2025 Vivero Don Honorio - Todos los derechos reservados\n";
  ptr += "            </div>\n";
  ptr += "        </div>\n";
  ptr += "    </div>\n";
  ptr += "    <script>\n";
  ptr += "        // Datos de muestra para el gráfico (10 temperaturas y 10 humedades)\n";
  ptr += "        const temperaturas = [" + String(tempHistory[0]) + "," + String(tempHistory[1]) + "," + String(tempHistory[2]) + "," + String(tempHistory[3]) + "," + String(tempHistory[4]) + "," + String(tempHistory[5]) + "," + String(tempHistory[6]) + "," + String(tempHistory[7]) + "," + String(tempHistory[8]) + "," + String(tempHistory[9]) + "];\n";
  ptr += "        const humedades = [" + String(humHistory[0]) + "," + String(humHistory[1]) + "," + String(humHistory[2]) + "," + String(humHistory[3]) + "," + String(humHistory[4]) + "," + String(humHistory[5]) + "," + String(humHistory[6]) + "," + String(humHistory[7]) + "," + String(humHistory[8]) + "," + String(humHistory[9]) + "];\n";
  ptr += "        \n";
  ptr += "        // Make canvas responsive\n";
  ptr += "        function resizeCanvas() {\n";
  ptr += "            const canvas = document.getElementById('dataChart');\n";
  ptr += "            const container = canvas.parentElement;\n";
  ptr += "            canvas.width = container.clientWidth - 10;  // Subtract small padding\n";
  ptr += "            canvas.height = container.clientHeight - 10;\n";
  ptr += "        }\n";
  ptr += "        \n";
  ptr += "        // Resize canvas when window size changes\n";
  ptr += "        window.addEventListener('load', resizeCanvas);\n";
  ptr += "        window.addEventListener('resize', resizeCanvas);\n";
  ptr += "        // Crear gráfico con Canvas\n";
  ptr += "        window.onload = function() {\n";
  ptr += "            const canvas = document.getElementById('dataChart');\n";
  ptr += "            const ctx = canvas.getContext('2d');\n";
  ptr += "            \n";
  ptr += "            // Configuración del gráfico\n";
  ptr += "            const width = canvas.width;\n";
  ptr += "            const height = canvas.height;\n";
  ptr += "            const padding = 10;\n";
  ptr += "            const maxDataValue = 100;\n";
  ptr += "            \n";
  ptr += "            // Dibujar marco\n";
  ptr += "            ctx.strokeStyle = '#009688';\n";
  ptr += "            ctx.lineWidth = 1;\n";
  ptr += "            ctx.strokeRect(0, 0, width, height);\n";
  ptr += "            \n";
  ptr += "            // Dibujar líneas de temperatura (usando los 10 puntos)\n";
  ptr += "            ctx.beginPath();\n";
  ptr += "            ctx.strokeStyle = 'red';\n";
  ptr += "            ctx.lineWidth = 2;\n";
  ptr += "            \n";
  ptr += "            for (let i = 0; i < temperaturas.length; i++) {\n";
  ptr += "                const x = padding + i * ((width - padding * 2) / (temperaturas.length - 1));\n";
  ptr += "                // Escalar temperaturas al rango visible\n";
  ptr += "                const y = height - padding - ((temperaturas[i] - 20) / 10) * (height - padding * 2);\n";
  ptr += "                \n";
  ptr += "                if (i === 0) {\n";
  ptr += "                    ctx.moveTo(x, y);\n";
  ptr += "                } else {\n";
  ptr += "                    ctx.lineTo(x, y);\n";
  ptr += "                }\n";
  ptr += "            }\n";
  ptr += "            ctx.stroke();\n";
  ptr += "            \n";
  ptr += "            // Dibujar líneas de humedad (usando los 10 puntos)\n";
  ptr += "            ctx.beginPath();\n";
  ptr += "            ctx.strokeStyle = 'blue';\n";
  ptr += "            ctx.lineWidth = 2;\n";
  ptr += "            \n";
  ptr += "            for (let i = 0; i < humedades.length; i++) {\n";
  ptr += "                const x = padding + i * ((width - padding * 2) / (humedades.length - 1));\n";
  ptr += "                const y = height - padding - (humedades[i] / maxDataValue) * (height - padding * 2);\n";
  ptr += "                \n";
  ptr += "                if (i === 0) {\n";
  ptr += "                    ctx.moveTo(x, y);\n";
  ptr += "                } else {\n";
  ptr += "                    ctx.lineTo(x, y);\n";
  ptr += "                }\n";
  ptr += "            }\n";
  ptr += "            ctx.stroke();\n";
  ptr += "            \n";
  ptr += "            // Leyenda\n";
  ptr += "            ctx.font = '10px Arial';\n";
  ptr += "            ctx.fillStyle = 'red';\n";
  ptr += "            ctx.fillRect(padding, padding, 10, 5);\n";
  ptr += "            ctx.fillText('Temperatura', padding + 15, padding + 5);\n";
  ptr += "            \n";
  ptr += "            ctx.fillStyle = 'blue';\n";
  ptr += "            ctx.fillRect(padding + 80, padding, 10, 5);\n";
  ptr += "            ctx.fillText('Humedad', padding + 95, padding + 5);\n";
  ptr += "        };\n";
  ptr += "    </script>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

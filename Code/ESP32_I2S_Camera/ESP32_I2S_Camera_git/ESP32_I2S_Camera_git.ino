#include "OV7670.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include "BMP.h"

const int SIOD = 21; // SDA
const int SIOC = 22; // SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 17;
const int D2 = 16;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

#define ssid1     "Familia montañez"
#define password1 "Tomashilda243"

OV7670 *camera;
WiFiMulti wifiMulti;
WiFiServer server(80);
unsigned char bmpHeader[BMP::headerSize];

void serve()
{
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.read(); // consume '\n'

  if (req.indexOf("GET /camera") >= 0) {
    camera->oneFrame();
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: image/bmp");
    client.println("Connection: close");
    client.println();
    client.write(bmpHeader, BMP::headerSize);
    client.write(camera->frame, camera->xres * camera->yres * 2);
  } else {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(
      "<style>body{margin:0} img{height:100%;width:auto}</style>"
      "<img id='a' src='/camera' onload='this.style.display=\"initial\"; var b=document.getElementById(\"b\"); b.style.display=\"none\"; b.src=\"/camera?\"+Date.now();'>"
      "<img id='b' style='display:none' src='/camera' onload='this.style.display=\"initial\"; var a=document.getElementById(\"a\"); a.style.display=\"none\"; a.src=\"/camera?\"+Date.now();'>"
    );
  }

  delay(1);
  client.stop();
}

void setup() 
{
  Serial.begin(115200);

  wifiMulti.addAP(ssid1, password1);
  Serial.println("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);

  server.begin();
}

void loop()
{
  serve();
  camera->oneFrame();
}
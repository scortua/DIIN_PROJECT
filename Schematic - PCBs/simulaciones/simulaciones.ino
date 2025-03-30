#include <Arduino.h> //https://github.com/fellipecouto/PWMOutESP32 [ http://www.efeitonerd.com.br ]
#include <DHT.h>

#define hm1 0
#define hm2 1
#define am1 2
#define am2 3
#define X 4
#define Y 5
#define A 6
#define DHT11_PIN 9
#define electroiman 8
#define motobomba 7

DHT dht(DHT11_PIN,DHT11);

void setup() {
  pinMode(hm1, OUTPUT);
  pinMode(hm2, OUTPUT);
  pinMode(am1, OUTPUT);
  pinMode(am2, OUTPUT);
  pinMode(electroiman, OUTPUT);
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
  pinMode(A, INPUT);
  pinMode(DHT11_PIN, INPUT);
  pinMode(motobomba, OUTPUT);
  dht.begin();
}

void loop() {

  if(digitalRead(X) != 0 && digitalRead(Y) == 0 && digitalRead(A) == 0){

    digitalWrite(hm2, LOW);
    digitalWrite(am2, LOW);


    digitalWrite(hm1, HIGH);
    digitalWrite(am1, LOW);
    delay(1000);
    digitalWrite(hm1, LOW);
    digitalWrite(am1, HIGH);
    delay(1000);
  }

  if(digitalRead(X) == 0 && digitalRead(Y) != 0 && digitalRead(A) == 0){

    digitalWrite(hm1, LOW);
    digitalWrite(am1, LOW);


    digitalWrite(hm2, HIGH);
    digitalWrite(am2, LOW);
    delay(1000);
    digitalWrite(hm2, LOW);
    digitalWrite(am2, HIGH);
    delay(1000);
  }

  if(digitalRead(X) == 0 && digitalRead(Y) == 0 && digitalRead(A) != 0){

    digitalWrite(hm2, LOW);
    digitalWrite(am2, LOW);
    digitalWrite(hm1, LOW);
    digitalWrite(am1, LOW);    

    digitalWrite(electroiman,HIGH);
    delay(1000);
    digitalWrite(electroiman, LOW);
    delay(1000);

  }

  float temp=dht.readTemperature();
  float hum=dht.readHumidity();

  if( temp > 30 || hum < 50){
    digitalWrite(hm2, LOW);
    digitalWrite(am2, LOW);
    digitalWrite(hm1, LOW);
    digitalWrite(am1, LOW);    

    digitalWrite(motobomba,HIGH);
    delay(1000);
  }else{
    digitalWrite(motobomba, LOW);
  }

}

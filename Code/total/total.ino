#include "DHT.h"

#define DHTPIN 5     // Pin donde está conectado el sensor DHT
#define DHTTYPE DHT22   // Tipo de sensor DHT (DHT11, DHT22, etc.)
DHT dht(DHTPIN, DHTTYPE);

// Definición de pines para el L298N
const int motorPin1 = 6;  // Pin PWM para controlar velocidad en una dirección
const int motorPin2 = 7; // Pin PWM para controlar velocidad en dirección opuesta

// Definición de pines para el L298N
const int motorPin3 = 8;  // Pin PWM para controlar velocidad en una dirección
const int motorPin4 = 9; // Pin PWM para controlar velocidad en dirección opuesta

// Definición de pines para step motor
const int bob1 = 10;
const int bob2 = 11;
const int bob3 = 12;
const int bob4 = 13;
const int mis = 2.5; // 5 10 15

// Variables para el control del motor
int velocidad1 = 0, velocidad2 = 0;        // Valor PWM (0-255)
bool direccion1 = true, direccion2 = true;    // true = adelante, false = atrás
bool modoAutomatico = false;
char step = 'p';

// Variables para el modo automático
unsigned long tiempoAnterior = 0;
const int INTERVALO = 30;  // Intervalo en ms para el cambio de velocidad
int paso = 1;              // Incremento/decremento de velocidad

void setup() {
  // Configuración motor 1
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  // Configuración motor 2
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  // Configuración step motor
  pinMode(bob1, OUTPUT);
  pinMode(bob2, OUTPUT);
  pinMode(bob3, OUTPUT);
  pinMode(bob4, OUTPUT);

  // Configuracion dht22
  dht.begin();
  
  // Inicializar motor detenido
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);

  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  
  // Iniciar comunicación serial
  Serial.begin(115200);
  Serial.println("Control de Motor L298N");
  Serial.println("Comandos disponibles:");
  Serial.println("A+velocidad - Giro en sentido horario (0-255)");
  Serial.println("B+velocidad - Giro en sentido antihorario (0-255)");
  Serial.println("C+velocidad - Giro en sentido horario (0-255)");
  Serial.println("D+velocidad - Giro en sentido antihorario (0-255)");
  Serial.println("E - Giro en sentido horario (step motor)");
  Serial.println("F - Giro en sentido antihorario (step motor)");
  Serial.println("S - Detener motor");
  Serial.println("M - Activar/desactivar modo automático");
}

void loop() {
  // Leer comandos del serial
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n'); // Leer hasta nueva línea
    procesarComando(comando);
  }
  
  // Control automático si está activo
  if (modoAutomatico) {
    modoVariacionAutomatica();
  }

  if (step == 'E' || step == 'e') {
    stepdireccion(bob1, bob2, bob3, bob4, mis);
    Serial.println("Step motor girando en sentido horario");
  } else if (step == 'F' || step == 'f') {
    stepdireccion(bob4, bob3, bob2, bob1, mis);
    Serial.println("Step motor girando en sentido antihorario");
  } else if (step == 'P' || step == 'p'){
    digitalWrite(bob1, LOW);
    digitalWrite(bob2, LOW);
    digitalWrite(bob3, LOW);
    digitalWrite(bob4, LOW);
    Serial.println("step motor detenido");
  } else {
    Serial.println("Comando no reconocido");
  }
  // Leer temperatura y humedad
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print("%, Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
}

void procesarComando(String comando) {
  comando.trim();  // Eliminar espacios en blanco
  
  if (comando.length() > 0) {
    char primerCaracter = comando.charAt(0);
    step = primerCaracter;
    // Detener modo automático si se recibe cualquier comando
    if (primerCaracter != 'M' && modoAutomatico) { 
      modoAutomatico = false;
      Serial.println("Modo automático desactivado");
    }
    
    switch (primerCaracter) {
      case 'A':
      case 'a':
        // Giro en sentido horario
        if (comando.indexOf('+') != -1) { // devuelve la posición del primer '+' encontrado
          velocidad1 = comando.substring(comando.indexOf('+') + 1).toInt(); // extraer velocidad
          velocidad1 = constrain(velocidad1, 0, 255); // Limitar velocidad entre 0 y 255
          moverMotor1(true, velocidad1); // mover motor hacia adelante
          Serial.print("Motor 1 girando hacia adelante con velocidad: "); 
          Serial.println(velocidad1);
        }
        break;
        
      case 'B':
      case 'b':
        // Giro en sentido antihorario
        if (comando.indexOf('+') != -1) { 
          velocidad1 = comando.substring(comando.indexOf('+') + 1).toInt();
          velocidad1 = constrain(velocidad1, 0, 255);
          moverMotor1(false, velocidad1);
          Serial.print("Motor 1 girando hacia atrás con velocidad: ");
          Serial.println(velocidad1);
        }
        break;
        
      case 'S':
      case 's':
        // Detener motor
        detenerMotor();
        Serial.println("Motores detenidos");
        break;
        
      case 'M':
      case 'm':
        // Activar/desactivar modo automático
        modoAutomatico = !modoAutomatico;
        if (modoAutomatico) {
          Serial.println("Modo automático activado");
          velocidad1 = 0;  // Iniciar desde velocidad 0
          paso = 1;       // Iniciar incrementando
        } else {
          Serial.println("Modo automático desactivado");
          detenerMotor();
        }
        break;
      
      case 'C':
      case 'c':
        // Giro en sentido horario para motor 2
        if (comando.indexOf('+') != -1) { 
          velocidad2 = comando.substring(comando.indexOf('+') + 1).toInt();
          velocidad2 = constrain(velocidad2, 0, 255);
          moverMotor2(true, velocidad2); // mover motor hacia adelante
          Serial.print("Motor 2 girando hacia adelante con velocidad: "); 
          Serial.println(velocidad2);
        }
        break;

      case 'D':
      case 'd':
        // Giro en sentido antihorario para motor 2
        if (comando.indexOf('+') != -1) { 
          velocidad2 = comando.substring(comando.indexOf('+') + 1).toInt();
          velocidad2 = constrain(velocidad2, 0, 255);
          moverMotor2(false, velocidad2);
          Serial.print("Motor 2 girando hacia atrás con velocidad: ");
          Serial.println(velocidad2);
        }
        break;
        
      default:
        Serial.println("Comando no reconocido");
        break;
    }
  }
}

void moverMotor1(bool direc, int vel) {
  if (direc) {
    // Dirección 1 (adelante)
    analogWrite(motorPin1, vel);
    analogWrite(motorPin2, 0);
  } else {
    // Dirección 2 (atrás)
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, vel);
  }
  
  direccion1 = direc;
  velocidad1 = vel;
}

void moverMotor2(bool direc, int vel){
  if (direc) {
    // Dirección 1 (adelante)
    analogWrite(motorPin3, vel);
    analogWrite(motorPin4, 0);
  } else {
    // Dirección 2 (atrás)
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, vel);
  }

  direccion2 = direc;
  velocidad2 = vel;
}

void detenerMotor() {
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 0);
  // Reiniciar variables
  velocidad1 = 0;
  velocidad2 = 0;
}

void modoVariacionAutomatica() {
  unsigned long tiempoActual = millis(); // Obtener tiempo actual
  
  // Cambiar velocidad cada INTERVALO milisegundos
  if (tiempoActual - tiempoAnterior >= INTERVALO) { 
    tiempoAnterior = tiempoActual; // Actualizar tiempo anterior
    
    // Cambiar dirección de incremento/decremento en los límites
    if (velocidad1 >= 255) {
      paso = -1;  // Comenzar a decrementar
    } else if (velocidad1 <= 0) {
      paso = 1;   // Comenzar a incrementar
      // Cambiar dirección de giro cuando llega a 0
      direccion1 = !direccion1;
    }
    
    // Actualizar velocidad
    velocidad1 += paso;
    velocidad1 = constrain(velocidad1, 0, 255);
    
    // Aplicar al motor
    moverMotor1(direccion1, velocidad1);
    
    // Mostrar información cada 10 pasos para no saturar el puerto serial
    if (velocidad1 % 10 == 0) {
      Serial.print("Auto - Dirección: ");
      Serial.print(direccion1 ? "Adelante" : "Atrás");
      Serial.print(", Velocidad: ");
      Serial.println(velocidad1);
    }
  }
}

void stepdireccion(int pin1, int pin2, int pin3, int pin4, int ms){
  Serial.println("step motor");
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(ms);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH); 
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(ms);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(ms);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  delay(ms);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  delay(ms);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  delay(ms);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delay(ms);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delay(ms);
}
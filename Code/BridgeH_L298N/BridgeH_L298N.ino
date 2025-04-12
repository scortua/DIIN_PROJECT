// Definición de pines para el L298N
const int motorPin1 = 6;  // Pin PWM para controlar velocidad en una dirección
const int motorPin2 = 7; // Pin PWM para controlar velocidad en dirección opuesta

// Variables para el control del motor
int velocidad = 0;        // Valor PWM (0-255)
bool direccion = true;    // true = adelante, false = atrás
bool modoAutomatico = false;

// Variables para el modo automático
unsigned long tiempoAnterior = 0;
const int INTERVALO = 30;  // Intervalo en ms para el cambio de velocidad
int paso = 1;              // Incremento/decremento de velocidad

void setup() {
  // Configuración de pines como salida
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  
  // Inicializar motor detenido
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  
  // Iniciar comunicación serial
  Serial.begin(115200);
  Serial.println("Control de Motor L298N");
  Serial.println("Comandos disponibles:");
  Serial.println("A+velocidad - Giro en sentido horario (0-255)");
  Serial.println("B+velocidad - Giro en sentido antihorario (0-255)");
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
}

void procesarComando(String comando) {
  comando.trim();  // Eliminar espacios en blanco
  
  if (comando.length() > 0) {
    char primerCaracter = comando.charAt(0);
    
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
          velocidad = comando.substring(comando.indexOf('+') + 1).toInt(); // extraer velocidad
          velocidad = constrain(velocidad, 0, 255); // Limitar velocidad entre 0 y 255
          moverMotor(true, velocidad); // mover motor hacia adelante
          Serial.print("Motor girando hacia adelante con velocidad: "); 
          Serial.println(velocidad);
        }
        break;
        
      case 'B':
      case 'b':
        // Giro en sentido antihorario
        if (comando.indexOf('+') != -1) { 
          velocidad = comando.substring(comando.indexOf('+') + 1).toInt();
          velocidad = constrain(velocidad, 0, 255);
          moverMotor(false, velocidad);
          Serial.print("Motor girando hacia atrás con velocidad: ");
          Serial.println(velocidad);
        }
        break;
        
      case 'S':
      case 's':
        // Detener motor
        detenerMotor();
        Serial.println("Motor detenido");
        break;
        
      case 'M':
      case 'm':
        // Activar/desactivar modo automático
        modoAutomatico = !modoAutomatico;
        if (modoAutomatico) {
          Serial.println("Modo automático activado");
          velocidad = 0;  // Iniciar desde velocidad 0
          paso = 1;       // Iniciar incrementando
        } else {
          Serial.println("Modo automático desactivado");
          detenerMotor();
        }
        break;
        
      default:
        Serial.println("Comando no reconocido");
        break;
    }
  }
}

void moverMotor(bool direc, int vel) {
  if (direc) {
    // Dirección 1 (adelante)
    analogWrite(motorPin1, vel);
    analogWrite(motorPin2, 0);
  } else {
    // Dirección 2 (atrás)
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, vel);
  }
  
  direccion = direc;
  velocidad = vel;
}

void detenerMotor() {
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 0);
  velocidad = 0;
}

void modoVariacionAutomatica() {
  unsigned long tiempoActual = millis(); // Obtener tiempo actual
  
  // Cambiar velocidad cada INTERVALO milisegundos
  if (tiempoActual - tiempoAnterior >= INTERVALO) { 
    tiempoAnterior = tiempoActual; // Actualizar tiempo anterior
    
    // Cambiar dirección de incremento/decremento en los límites
    if (velocidad >= 255) {
      paso = -1;  // Comenzar a decrementar
    } else if (velocidad <= 0) {
      paso = 1;   // Comenzar a incrementar
      // Cambiar dirección de giro cuando llega a 0
      direccion = !direccion;
    }
    
    // Actualizar velocidad
    velocidad += paso;
    velocidad = constrain(velocidad, 0, 255);
    
    // Aplicar al motor
    moverMotor(direccion, velocidad);
    
    // Mostrar información cada 10 pasos para no saturar el puerto serial
    if (velocidad % 10 == 0) {
      Serial.print("Auto - Dirección: ");
      Serial.print(direccion ? "Adelante" : "Atrás");
      Serial.print(", Velocidad: ");
      Serial.println(velocidad);
    }
  }
}
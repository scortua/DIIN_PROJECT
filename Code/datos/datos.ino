#include <driver/i2s.h>

// Parámetros de I2S
#define I2S_WS  25
#define I2S_SD  32
#define I2S_SCK 33

#define SAMPLE_RATE 16000
#define SAMPLE_BITS I2S_BITS_PER_SAMPLE_16BIT
#define CHANNEL_FORMAT I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_PORT I2S_NUM_0

// Configuración del buffer de energía
#define WINDOW_MS 20                         // Cada cuánto calcular la energía (milisegundos)
#define VECTOR_DURATION_SEC 2                // Duración de la grabación (segundos)
#define VECTOR_SIZE (VECTOR_DURATION_SEC * 1000 / WINDOW_MS) // Tamaño del vector de energía

// Umbral para detectar voz
#define ENERGY_THRESHOLD 250

// Variables
float energyVector[VECTOR_SIZE];
bool recording = false;
unsigned long recordingStartTime = 0;
int vectorIndex = 0;

void setupI2S() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = SAMPLE_BITS,
    .channel_format = CHANNEL_FORMAT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

float calculateEnergy() {
  const int samplesToRead = SAMPLE_RATE * WINDOW_MS / 1000;
  int16_t buffer[samplesToRead];
  size_t bytes_read;

  i2s_read(I2S_PORT, (void*)buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

  int samples_read = bytes_read / sizeof(int16_t);
  uint32_t sum = 0;

  for (int i = 0; i < samples_read; i++) {
    sum += abs(buffer[i]);
  }

  return (float)sum / samples_read;
}

void printEnergyVector() {
  Serial.println("\nEnergía grabada durante 2 segundos:");
  for (int i = 0; i < VECTOR_SIZE; i++) {
    Serial.print(energyVector[i]);
    Serial.print(",");
  }
  Serial.println("\nGrabación completa.");
  Serial.println("---------------------------");
}

void setup() {
  Serial.begin(115200);
  setupI2S();
}

void loop() {
  float currentEnergy = calculateEnergy();
  
  // SIEMPRE imprimir la energía actual
  Serial.println(currentEnergy);

  if (!recording) {
    // No estamos grabando aún
    if (currentEnergy > ENERGY_THRESHOLD) {
      // Se detectó voz, iniciar grabación
      Serial.println("¡Voz detectada! Iniciando grabación de 3 segundos...");
      recording = true;
      recordingStartTime = millis();
      vectorIndex = 0;
    }
  } else {
    // Estamos grabando
    if (vectorIndex < VECTOR_SIZE) {
      energyVector[vectorIndex] = currentEnergy;
      vectorIndex++;
    }

    if (millis() - recordingStartTime >= VECTOR_DURATION_SEC * 1000) {
      // Terminar grabación
      recording = false;
      printEnergyVector();
    }
  }

  delay(WINDOW_MS);
}
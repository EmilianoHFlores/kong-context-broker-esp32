#include <Wire.h>
#include <Adafruit_PM25AQI.h>
#include <SensirionI2cScd4x.h>

SensirionI2cScd4x scd4x;
Adafruit_PM25AQI pm25 = Adafruit_PM25AQI();

const uint32_t TIEMPO_ESPERA_PM25 = 10000;  
const uint8_t MAX_REINTENTOS_PM25 = 3;

void setup() {
  Serial.begin(115200);

  Wire.begin();
  

  inicializarSCD41();
  
  Serial1.begin(9600);
  delay(TIEMPO_ESPERA_PM25); 
  
  if (!pm25.begin_UART(&Serial1)) {
    Serial.println("Error al inicializar sensor PM2.5!");
    while (1) delay(10);
  }
  
  Serial.println("Todos los sensores inicializados correctamente");
}

void loop() {

  leerSCD41();
  
  delay(1000);
  
  leerPM25();
  
  delay(4000);
}

void inicializarSCD41() {
  uint16_t error;
  char errorMessage[256];
  
  scd4x.begin(Wire, 0x62);

  error = scd4x.startPeriodicMeasurement();
  
  if (error) {
    mostrarErrorSCD("Iniciando mediciones", error);
    while(1);
  }
  
  Serial.println("SCD41 listo");
  delay(500);
}

void leerSCD41() {
  bool dataReady = false;
  uint16_t error = scd4x.getDataReadyStatus(dataReady);
  
  if (error) {
    mostrarErrorSCD("Verificando estado", error);
    return;
  }
  
  if (dataReady) {
    uint16_t co2;
    float temp, hum;
    error = scd4x.readMeasurement(co2, temp, hum);
    
    if (!error) {
      Serial.println("\n=== Datos SCD41 ===");
      Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
      Serial.print("Temp: "); Serial.print(temp); Serial.println(" °C");
      Serial.print("Humedad: "); Serial.print(hum); Serial.println(" %");
    } else {
      mostrarErrorSCD("Leyendo datos", error);
    }
  }
}

void leerPM25() {
  PM25_AQI_Data datos;
  uint8_t intentos = 0;
  bool exito = false;

  while (intentos < MAX_REINTENTOS_PM25 && !exito) {
    Serial1.flush();
    exito = pm25.read(&datos);
    
    if (!exito) {
      delay(500);
      intentos++;
    }
  }

  if (exito) {
    Serial.println("\n=== Datos PM2.5 ===");
    Serial.print("PM1.0: "); Serial.print(datos.pm10_standard); Serial.println(" µg/m³");
    Serial.print("PM2.5: "); Serial.print(datos.pm25_standard); Serial.println(" µg/m³");
    Serial.print("PM10: "); Serial.print(datos.pm100_standard); Serial.println(" µg/m³");
    Serial.print("Partículas >0.3µm: "); Serial.println(datos.particles_03um);
    Serial.print("Partículas >0.5µm: "); Serial.println(datos.particles_05um);
  } else {
    Serial.println("Error leyendo sensor PM2.5!");
  }
}

void mostrarErrorSCD(const char* contexto, uint16_t error) {
  char errorMessage[256];
  errorToString(error, errorMessage, 256);
  Serial.print("[SCD41] Error en '"); Serial.print(contexto); Serial.print("': ");
  Serial.println(errorMessage);
}
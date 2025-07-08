#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_PM25AQI.h"
#include <SensirionI2cScd4x.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

// ======= Configuración ========
#define UPLOAD_PERIOD 1  // Minutos entre lecturas

// ======= Estructura de datos ========
struct SensorData {
    float pm10;
    float pm25;
    float pm100;
    float co2;
    float temp;
    float hum;
};

// ======= Objetos globales ========
Adafruit_PM25AQI aqi;
SensirionI2cScd4x scd4x;

// Variables para sleep mode
volatile uint8_t sleepCounter = 0;
const uint8_t SLEEP_CYCLES = UPLOAD_PERIOD * 60 / 8;  // Ciclos de 8 segundos


// ======= Funciones ========
void initializeSensors() {
    // Configurar pull-ups para I2C
    pinMode(SDA, INPUT_PULLUP);
    pinMode(SCL, INPUT_PULLUP);
    
    Wire.begin();
    delay(1000);
    
    // Inicializar sensor SCD41 con reintentos
    Serial.println("Inicializando SCD41...");
    uint16_t error = 0;
    for(int i = 0; i < 5; i++) {
        // CORRECCIÓN: Añadir dirección I2C 0x62 como segundo parámetro
        scd4x.begin(Wire, 0x62);  // Configurar bus I2C y dirección
        
        error = scd4x.startPeriodicMeasurement();
        if (!error) {
            Serial.println("SCD41 inicializado correctamente");
            break;
        }
        Serial.print("Intento ");
        Serial.print(i+1);
        Serial.print(" fallido: 0x");
        Serial.println(error, HEX);
        delay(1000);
    }
    
    if (error) {
        Serial.print("Error crítico en SCD41: 0x");
        Serial.println(error, HEX);
    }
    
    // Inicializar sensor PM2.5
    Serial.println("Inicializando sensor PM2.5...");
    Serial1.begin(9600);
    delay(2000);
    
    if (!aqi.begin_UART(&Serial1)) {
        Serial.println("Error inicializando sensor PM2.5");
    }
    delay(1000);
}

bool readSensors(SensorData* data) {
    // Leer sensor de partículas con reintentos
    PM25_AQI_Data pmData;
    int intentos = 0;
    const int max_intentos = 5;
    bool pmSuccess = false;
    
    while(intentos < max_intentos) {
        if(aqi.read(&pmData)) {
            data->pm10 = pmData.pm10_env;
            data->pm25 = pmData.pm25_env;
            data->pm100 = pmData.pm100_env;
            pmSuccess = true;
            break;
        }
        delay(500);
        intentos++;
    }
    
    if(!pmSuccess) {
        Serial.println("Error lectura sensor PM2.5 después de reintentos");
        return false;
    }
    
    // Leer sensor SCD41 con reintentos
    uint16_t co2 = 0;
    float temp = 0, hum = 0;
    uint16_t error = 1;
    intentos = 0;
    
    while(intentos < max_intentos) {
        error = scd4x.readMeasurement(co2, temp, hum);
        if (!error) {
            data->co2 = co2;
            data->temp = temp;
            data->hum = hum;
            break;
        }
        delay(500);
        intentos++;
    }
    
    if(error) {
        Serial.print("Error lectura SCD41 después de reintentos: 0x");
        Serial.println(error, HEX);
        return false;
    }
    
    return true;
}

void simulateLoRaSend(const SensorData& data) {
    Serial.println("======== SIMULACIÓN DE ENVÍO LoRa ========");
    Serial.print("PM1.0: "); Serial.print(data.pm10); Serial.println(" μg/m³");
    Serial.print("PM2.5: "); Serial.print(data.pm25); Serial.println(" μg/m³");
    Serial.print("PM10: ");  Serial.print(data.pm100); Serial.println(" μg/m³");
    Serial.print("CO₂: ");   Serial.print(data.co2); Serial.println(" ppm");
    Serial.print("Temp: ");  Serial.print(data.temp); Serial.println(" °C");
    Serial.print("Humedad: "); Serial.print(data.hum); Serial.println(" %");
    Serial.println("=========================================");
}

// Configurar Watchdog para modo sleep
void configureWatchdog() {
    cli();                           // Deshabilitar interrupciones
    MCUSR &= ~(1<<WDRF);             // Limpiar flag de reset
    WDTCSR |= (1<<WDCE) | (1<<WDE);  // Habilitar cambios
    WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0);  // 8 segundos
    sei();                           // Habilitar interrupciones
}

// Entrar en modo sleep
void enterSleep() {
    // Serial.println("-> Durmiendo...");
    Serial.flush();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    
    // Apagar periféricos innecesarios
    power_adc_disable();
    power_timer1_disable();
    power_timer3_disable();
    // power_usart1_disable();
    power_spi_disable();
    power_twi_disable();
    delay(10);
    sleep_cpu();  // Entrar en sleep
    
    // Al despertar
    sleep_disable();
    power_all_enable();  // Reactivar periféricos
    // Serial.println("-> Despertado");
}

// ======= Setup principal ========
void setup() {
    // Retardo inicial para estabilización
    delay(4000);
    
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(1000);
    
    Serial.println("\n==================================");
    Serial.println("=== Iniciando dispositivo Feather 32u4 ===");
    Serial.println("Modo de simulación - No se enviarán datos reales por LoRa");
    Serial.println("==========================================");
    
    initializeSensors();
    configureWatchdog();
    
    Serial.print("\nIntervalo de lectura: ");
    Serial.print(UPLOAD_PERIOD);
    Serial.println(" minutos");
    Serial.print("Ciclos de sleep (8s): ");
    Serial.println(SLEEP_CYCLES);
    Serial.println("==============================+");
}

unsigned long lastWake = 0;
// ======= Loop principal ========
void loop() {
    // Leer sensores
    SensorData sensorData;
    if (readSensors(&sensorData)) {
        simulateLoRaSend(sensorData);
    } else {
        Serial.println("Error en lectura de sensores. Reintentando...");
    }
    
    Serial.print("\nEntrando en modo sleep por ");
    Serial.print(UPLOAD_PERIOD);
    Serial.println(" minutos...");
    Serial.flush();  // Asegurar que todos los datos se envíen antes de dormir
    
    // Reiniciar contador de sleep
    sleepCounter = 0;
    lastWake = millis();  // Guardar tiempo de último despertar
    
    // Ciclos de sleep de 8 segundos
    while(sleepCounter < SLEEP_CYCLES) {
        // wdt_reset();  // Resetear watchdog antes de dormir
        enterSleep();
    }
    
    Serial.println("\n¡Despertado! Preparando nueva lectura...");
    Serial.print((millis() - lastWake) / 1000.0);
    Serial.println(" segundos.");
    // delay(10000);
}

// Interrupción del Watchdog Timer
ISR(WDT_vect) {
    sleepCounter++;
}
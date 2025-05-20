#include <SPI.h>
#include <LoRa.h>

#define SS    8
#define RST   4
#define DIO0  7 

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver - Feather 32u4");

  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("¡Error al iniciar LoRa!");
    while (1);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Escuchando...\n");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    Serial.print("Paquete recibido (");
    Serial.print(packetSize);
    Serial.println(" bytes):");

    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    Serial.print("\nRSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.println("-------------------");
  }
}
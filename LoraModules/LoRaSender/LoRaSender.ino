#include <SPI.h>
#include <LoRa.h>

#define SS    8
#define RST   4
#define DIO0  7

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.println("Sending packet...");

  LoRa.beginPacket();
  LoRa.print("Hello from Feather 32u4");
  LoRa.endPacket();

  delay(2000);
}

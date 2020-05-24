#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

byte buff[2];

byte id = 0xFF;     // address of this node

#define DHTPIN 4
#define DHTTYPE DHT11

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender and DHT11 Sensor");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  dht.begin();

  if (!LoRa.begin(866E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  delay(3000);
  Serial.println("Sending packet from ");
  uint8_t hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  uint8_t temp = dht.readTemperature();

  buff[0] = temp;
  buff[1] = hum;
  buff[2] = id;

  Serial.println(id);
  Serial.print("Temperature in celcius: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(hum);

  // send packet
  LoRa.beginPacket();
  LoRa.write(buff, sizeof(buff));
  LoRa.endPacket();

  for (int i = 0; i > 4; i++) {
    Serial.println(buff[i]);
  }
}

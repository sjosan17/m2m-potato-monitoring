#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

const char* ssid = "................."; // Wifi SSID
const char* password = "............."; // Wifi Password
const char* username = "............."; // my AskSensors username
const char* pubTopic = "............."; // username/apiKeyIn
const unsigned int writeInterval = 25000;   // write interval (in ms)
//AskSensors MQTT config
const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;

WiFiClient askClient;
PubSubClient client(askClient);


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Gateway to AskSensor via MQTT");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(866E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa started!");
  // Start led on the esp32
  pinMode(LED_BUILTIN, OUTPUT);

  // WIFI and Asksensor section

  Serial.print("********** connecting to WIFI : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("->WiFi connected");
  Serial.println("->IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  onReceive(LoRa.parsePacket());
}

// LoRa function to read and print to serial.
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  int temp = LoRa.read();
  int hum = LoRa.read();
  byte id = LoRa.read();

  // Only take in data from node id, can add more with OR.
  if (id == 0xFF) {

    // if message is for this device, or broadcast, print details:
    Serial.print("From nodeID: ");
    Serial.println(id);
    Serial.print("Temperature in celcius: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(hum);
    Serial.println();
    // send to Asksensor via MQTT
    senddata(temp, hum);
  } else {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // To debug for gateway is working visually
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

void senddata(int temp, int hum) {
  if (!client.connected())
    reconnect();
  client.loop();
  Serial.println("********** Publish MQTT data to ASKSENSORS");
  char mqtt_payload[30] = "";
  snprintf (mqtt_payload, 30, "m1=%ld&m2=%ld", temp, hum);
  Serial.print("Publish message: ");
  Serial.println(mqtt_payload);
  client.publish(pubTopic, mqtt_payload);
  Serial.println("> MQTT data published");
  Serial.println("********** End ");
  Serial.println("*****************************************************");

  delay(writeInterval);// delay
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("********** Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", username, "")) {
      Serial.println("-> MQTT client connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("-> try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

# m2m-potato-monitoring
This is two TTGO Eps32 which are communicating with each other via LoRa. One is the node with DHT11 which is a temperature and humidity sensor. The other one is a gateway which is connected to WiFi to AskSensor web services. AskSensor will show graphs and will send an email warning when the temperature falls below a certain level. AskSensor is access through the browser and works on the smartphone and pc.

# Hardware:
5x cables\
1x 10 Ohm resistor\
1x DHT11 sensor\
2x TTGOESP32\
1x Breadboard\

For more details see the fritzing folder.

# Pins used in the code:
#define SCK     5    // GPIO5  -- SX1278's SCK\
#define MISO    19   // GPIO19 -- SX1278's MISO\
#define MOSI    27   // GPIO27 -- SX1278's MOSI\
#define SS      18   // GPIO18 -- SX1278's CS\
#define RST     14   // GPIO14 -- SX1278's RESET\
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)\

LoRa frequens used is the European 866 frequens, LoRa.begin(866E6)

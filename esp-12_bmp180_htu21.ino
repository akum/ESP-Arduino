// Import required libraries
#include "ESP8266WiFi.h"
#include "credentials.h"

// Setup i2c
#include "Wire.h"
#define I2C_SCL 12
#define I2C_SDA 13

// deepSleep time is defined in microseconds.
const int sleepTime = (5 * 60 * 1000000);

// HTU21
#include "SparkFunHTU21D.h"
HTU21D htu;
String hhumi, htemp;

// DHT11
#include "DHT.h"
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
String dhumi, dtemp;

// BMP180
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
String btemp, bpres;

void readSensor() {
  hhumi = String( htu.readHumidity() );
  htemp = String( htu.readTemperature() );

  dhumi = String( dht.readHumidity() );
  dtemp = String( dht.readTemperature() );

  btemp = String( bmp.readTemperature() );
  bpres = String( bmp.readPressure() / 3386.0 );

  // Serial.print("H_Humidity: ");
  // Serial.println(hhumi);
  // Serial.print("H_Temperature: ");
  // Serial.println(htemp);

  // Serial.print("D_Humidity: ");
  // Serial.println(dhumi);
  // Serial.print("D_Temperature: ");
  // Serial.println(dtemp);

  // Serial.print("B_Pressure: ");
  // Serial.println(bpres);
  // Serial.print("B_Temperature: ");
  // Serial.println(btemp);
}

void updateTS() {
  WiFiClient client;
  String postStr = "1=" + btemp + "&2=" + bpres + "&3=" + dhumi + "&4=" + dtemp + "&5=" + hhumi + "&6=" + htemp;

  if (client.connect(server, 80)) {
    client.println("POST /update HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + apiKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postStr.length());
    client.println("");
    client.print(postStr);
  }
  client.stop();
}

void setup(void) {
  // Serial.begin(74880);
  WiFi.begin(ssid, password);
  dht.begin();
  Wire.begin(I2C_SDA, I2C_SCL);
  bmp.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  readSensor();
  updateTS();
  ESP.deepSleep(sleepTime, WAKE_NO_RFCAL);
}

void loop() {
}

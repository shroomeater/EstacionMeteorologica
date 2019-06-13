/*
 Estación Meteorológica
 TIND-TIC
 2018/19

 GRUPO 4
 Nuria Galve Conteras 
 Manolo Garcia Martinez
 Daniel Alexis Jurado Ortiz
 */

//incluimos las librerías necesarias
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// asignar el bus del SPI a los pines
#define BME_SCK D1
#define BME_MISO D4
#define BME_MOSI D2
#define BME_CS D3

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long myChannelNumber = 764210; //número del canal del Thingspeak
const char * myWriteAPIKey = "CN82Z1DRLWJT83DU"; // APIKey del Thingspeak
const char* ssid = "Andared"; //nombre del WiFi
const char* password = "llevalatararaunvestidoblancollenodecascabeles"; //contraseña del WiFi

const char* server = "api.thingspeak.com"; //dirección del servidor al que se van a enviar los datos
WiFiClient client;

uint32_t TIEMPO_DeepSleep = 9e8;  // Tiempo en modo deep-sleep en microsegundos
uint8_t TIEMPO_Referencia;        // Tiempo de referencia para encender 1 segundo el LED

void setup() {
  Wire.begin(0, 2);
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (!bme.begin()) {
    Serial.println("No se encuentra un sensor BME280 válido, ¡comprobar el cableado!");
    while (1);
  }
  ThingSpeak.begin(client);

  TIEMPO_Referencia = millis();
}

void loop() {
  float t = bme.readTemperature(); //lectura de la temperatura
  float p = bme.readPressure(); //lectura de la presión atmosférica
  float h = bme.readHumidity(); //lectura de la humedad

  //imprimimos en el puerto serie los datos
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius Pressure: ");
  Serial.print(p);
  Serial.print(" Humidity: ");
  Serial.print(h);
  Serial.println("% send to Thingspeak");

  //Asignamos cada valor del sensor a su campo en el Thingspeak
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, p);
  ThingSpeak.setField(3, h);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  //seleccionar tiempo para el modo sueño
  if (millis() - TIEMPO_Referencia > 1000) {
    ESP.deepSleep(TIEMPO_DeepSleep, WAKE_RF_DEFAULT); // Calibración de señal de radio si es necesario
  }
}


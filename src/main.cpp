#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

const char* ssid = "Redmi";
const char* password = "12345678";


const int ledPin = 2;

#define LED D0

SoftwareSerial SpeeduinoSerial(D5, D6); //Rx. Tx
int secs = 0;
String chr;
char AcmdResp[78];


void setup() {

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(115200);
  SpeeduinoSerial.begin(115200);

  if (!SpeeduinoSerial) {
    Serial.println("Invalid");
  }

  Serial.println("------------setup end---------------");
}

int cnt = 0;
int nrOfBytes = 0;

byte response[100];

char caracter;

int availableBytes = 0;

void loop() {

  // Serial.println();
  // Serial.print("loop :");
  // Serial.print(cnt);

  // SpeeduinoSerial.flush();
  SpeeduinoSerial.write("A");
  Serial.println("->A");

  if (SpeeduinoSerial.available() > 0) {
    // Serial.println("SeeduinoSrerial is available");
    // caracter = SpeeduinoSerial.read();
    for (int i = 0;i < 64;i++) {
      caracter = SpeeduinoSerial.read();
      Serial.print(caracter);
      Serial.print("|");
    }
  }

  delay(1000);
  cnt++;
}
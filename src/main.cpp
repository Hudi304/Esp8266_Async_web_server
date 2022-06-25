#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

const char* ssid = "Redmi";
const char* password = "12345678";


const int ledPin = 2;

#define LED D0

SoftwareSerial SpeeduinoSerial(D5, D6, false); //Rx. Tx
int secs = 0;
String chr;
char AcmdResp[78];


byte response[100];

void printResponse() {
  for (int i = 0;i < 100;i++) {
    Serial.print("|");
    Serial.print(response[i], HEX);
  }
}

void setup() {


  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(115200);
  // SpeeduinoSerial.begin(115200);

  delay(500);
  Serial.swap();
  delay(500);


  Serial.print("A");

}



void loop() {

}
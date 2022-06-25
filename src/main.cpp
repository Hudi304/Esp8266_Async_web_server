#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

const char* ssid = "Redmi";
const char* password = "12345678";


#define LED D0

SoftwareSerial SpeeduinoSerial(D5, D6); //Rx. Tx
int secs = 0;
String chr;
char AcmdResp[78];


void setup() {

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
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

void loop() {

  Serial.println();
  Serial.print("loop :");
  Serial.print(cnt);



  SpeeduinoSerial.write("A");
  Serial.println("->A");

  if (SpeeduinoSerial.available() > 0) {
    Serial.println("");
    Serial.println("SeeduinoSrerial is available");
    Serial.println(SpeeduinoSerial.available());
    caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());

    caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());caracter = SpeeduinoSerial.read();
    Serial.println(SpeeduinoSerial.available());

    for (int i = 0;i < 75;i++) {
      caracter = SpeeduinoSerial.read();
      Serial.print(caracter);
      Serial.print("|");
    }
  }

  delay(5000);
  cnt++;
}
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Redmi";
const char* password = "12345678";
bool ledState = 0;
const int ledPin = 2;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* adminClient;
uint8_t clientIndex = 0;

long int loopIndex = 0;

void notifyClients() {
  ws.textAll(String(ledState));
}

void notifyClientsStr(String msg) {
  ws.textAll(msg);
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len, AsyncWebSocketClient* client) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String dataString = String((const char*)data);
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      String msg = "";
      msg.concat("Client[");
      msg.concat(String(client->id()));
      msg.concat("] turned the LED : ");
      if (ledState) {
        msg.concat("OFF");
      }
      else {
        msg.concat("ON");
      }
      notifyClientsStr(msg);
    }

    if (strcmp((char*)data, "A") == 0) {
      // Serial.print('A');
      notifyClientsStr("UART : A");
    }

    if (strcmp((char*)data, "R") == 0) {
      if (Serial.available() > 0) {
        char msg = Serial.read();
        notifyClientsStr("Serial Available");
        notifyClientsStr(String(msg));
        notifyClientsStr(String(Serial.available()));
      }
      else {
        notifyClientsStr("Serial NOT Available");
      }
    }
    else {
      String msg = "";
      msg.concat("Client[");
      msg.concat(String(client->id()));
      msg.concat("]: ");
      for (int i = 0; i < len;i++) {
        msg.concat((char)data[i]);
      }
      notifyClientsStr(msg);
    }
  }
}

int clientNo = 0;

void onEvent(AsyncWebSocket* socket, AsyncWebSocketClient* client, AwsEventType type,
  void* arg, uint8_t* data, size_t len) {
  // Serial.printf("Msg Client [%u]  \n", client->id());

  switch (type) {
  case WS_EVT_CONNECT:
    // Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    // if (adminClientId == 0) {
      // Serial.printf("in if");
      // clientIndex++;
      // adminClientId = client->id();
      // adminClient = client;
      // Serial.printf("%u", (char*)adminClient->id());
    // }

    clientNo++;

    notifyClientsStr("con");
    break;
  case WS_EVT_DISCONNECT:
    // Serial.printf("WebSocket client #%u disconnected\n", client->id());
    clientNo--;
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len, client);
    break;
  case WS_EVT_PONG:
    break;
  case WS_EVT_ERROR:
    // Serial.printf("error\n");
    // Serial.print((char*)data);

    break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var) {
  if (var == "STATE") {
    if (ledState) {
      return "ON";
    }
    else {
      return "OFF";
    }
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  initWebSocket();
  server.begin();
}

bool recvInProgress = false;
bool doneTransmitting = true;
bool doneReading = true;

bool readyToQuery = true;
bool readyToReadData = false;
bool readyToSendToSocket = false;
bool sendingToSocket = false;
bool bufferHasNewData = false;


char receivedBytes[20];
char charsForSocket[20];
u_int8_t recvIndex = 0;

void flushUARTbuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void doneTransmittingCleanUp() {
  readyToQuery = true;
  readyToReadData = false;
  readyToSendToSocket = true;
  strcpy(charsForSocket, receivedBytes);
  flushUARTbuffer();
  recvIndex = 0;
  // ws.textAll("done Transmitting");
}

void sendData() {
  if (readyToQuery == true && clientNo > 0) {
    // ws.textAll("sent Data");
    // notifyClientsStr("sent Data");
    flushUARTbuffer();
    Serial.print('A');
    readyToQuery = false;
    readyToReadData = true;
  }
}

void receiveByte() {
  if (Serial.available() > 0 && readyToReadData) {
    if (recvIndex < 13) {
      char received;
      received = Serial.read();
      receivedBytes[recvIndex] = received;
      strcpy(charsForSocket, receivedBytes);
      ws.textAll(receivedBytes);
      recvIndex++;
      bufferHasNewData = true;
    }
    else {
      flushUARTbuffer();
      readyToReadData = false;
      readyToSendToSocket = true;
      strcpy(charsForSocket, receivedBytes);
      char msg[30] = "finished reading";
      char msg2[30] = "flushed buffer";
      sprintf(msg2, "%d", Serial.available());
      recvIndex = 0;
    }
  }
  else {
    flushUARTbuffer();
    readyToQuery = false;
    readyToReadData = false;
    readyToSendToSocket = true;
    strcpy(charsForSocket, receivedBytes);
    recvIndex = 0;

  }
}

void sendDataToSocket() {
  if (readyToSendToSocket && bufferHasNewData) {
    char msg[40] = "finished reading";
    sprintf(msg, "%d : %s", loopIndex, charsForSocket);
    ws.textAll(msg);
    readyToSendToSocket = false;
    readyToQuery = true;
    bufferHasNewData = false;
  }
}

void loop() {
  loopIndex++;
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);

  sendData();
  receiveByte();
  sendDataToSocket();
}
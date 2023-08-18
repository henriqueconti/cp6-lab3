#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include "DHT.h"

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define DHTPIN  7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int TAMANHO = 200;

#define led 13

void setup() {
  dht.begin();
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  Serial.setTimeout(10);
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  StaticJsonDocument<TAMANHO> json;

  float temp = dht.readTemperature();
  float umi = dht.readHumidity();

  json["temperatura"] = temp;
  json["umidade"] = umi;

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    json["rfid"] = cardUID;
  }

  if (Serial.available() > 0) {
    StaticJsonDocument<TAMANHO> json;
    deserializeJson(json, Serial);
    if (json.containsKey("led")) {
      int valor = json["led"];
      analogWrite(led, valor);
    }
  }

  String jsonPayload;
  serializeJson(json, jsonPayload);
  Serial.println(jsonPayload);

  delay(1000);
}

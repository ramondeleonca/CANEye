#include <Arduino.h>
#include <Wire.h>
#include <CAN.h>
#include <lib/ColorSensorV3.h>

ColorSensorV3 colorSensor;

const int CAN_RATE = 500E3;
const int CAN_RX = 27;
const int CAN_TX = 26;
const int CAN_ID = 30;

const int R_PIN = 2;
const int G_PIN = 4;
const int B_PIN = 16;
const int PROX_PIN = 17;

// #define CAN_ENABLED

enum PacketType : int {
  kRed = 0x00,
  kGreen = 0x01,
  kBlue = 0x02,
  kProximity = 0x03
};

void setup() {
  Serial.begin(115200);
  Wire.begin();
  colorSensor.begin();
  #ifdef CAN_ENABLED
  CAN.setPins(CAN_RX, CAN_TX);
  CAN.begin(CAN_RATE);
  #endif
}

void loop() {
  int r = colorSensor.getRed();
  int g = colorSensor.getGreen();
  int b = colorSensor.getBlue();
  int prox = colorSensor.getProximity();

  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
  analogWrite(PROX_PIN, map(prox, 0, 2047, 0, 255));

  #ifdef CAN_ENABLED
  CAN.beginPacket(CAN_ID);
  CAN.write(PacketType::kRed);
  CAN.write(r);
  CAN.endPacket();

  CAN.beginPacket(CAN_ID);
  CAN.write(PacketType::kGreen);
  CAN.write(g);
  CAN.endPacket();

  CAN.beginPacket(CAN_ID);
  CAN.write(PacketType::kBlue);
  CAN.write(b);
  CAN.endPacket();

  CAN.beginPacket(CAN_ID);
  CAN.write(PacketType::kProximity);
  CAN.write(prox);
  CAN.endPacket();
  #endif

  Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.print(b);
  Serial.print(",");
  Serial.print(prox);
  Serial.println();

  delay(100);
}

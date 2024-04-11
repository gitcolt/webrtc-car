#include <Arduino.h>
#include <printf.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "/home/colt/projects/embedded/webrtccar/include/motor_state.hpp"

#define ROLE_TX

#define CE_PIN 9
#define CSN_PIN 8

#define FORWARD_PIN 3
#define BACKWARD_PIN 4

RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = { "1Node", "2Node" };
bool radio_num = 1;

uint8_t payload = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("Serial initialized");

  if (!radio.begin()) {
    if (!radio.isChipConnected()) {
      Serial.println(F("Chip is not connected"));
    } else
      Serial.println(F("Chip is connected"));

    Serial.println(F("radio hardware not responding!"));
    while (1) { }
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address[radio_num]); // uses pipe 0
  radio.openReadingPipe(1, address[!radio_num]); // uses pipe 1

  radio.stopListening();

  if (!radio.isChipConnected())
    Serial.println("Chip is not connected correctly");
  
  Serial.println("TX INITIALIZED");
}

bool process_serial_input() {
  uint8_t in = Serial.read();
  payload = in;
  return radio.write(&payload, sizeof(payload));
  // switch (in) {
  //   case FORWARD:
  //     payload = FORWARD;
  //     return radio.write(&payload, sizeof(payload));
  //   case STOP:
  //     payload = STOP;
  //     return radio.write(&payload, sizeof(payload));
  //   case REVERSE:
  //     payload = REVERSE;
  //     return radio.write(&payload, sizeof(payload));
  //   case LEFT:
  //     payload = LEFT;
  //     return radio.write(&payload, sizeof(payload));
  //   case RIGHT:
  //     payload = RIGHT;
  //     return radio.write(&payload, sizeof(payload));
  //   case STRAIGHT:
  //     payload = STRAIGHT;
  //     return radio.write(&payload, sizeof(payload));
  //   default:
  //     Serial.print("Unknown input byte '");
  //     Serial.print(in);
  //     Serial.println("'");
  // }
}

void loop() {
  if (!Serial.available())
    return;
  bool res = process_serial_input();

  if (res) {
    Serial.print(F("Sent byte: "));
    Serial.println(payload);
  } else
    Serial.println(F("Transmission failed or timed out"));
}

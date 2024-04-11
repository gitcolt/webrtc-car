#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RF24.h>

#include "motor_state.hpp"

#define CE_PIN 9
#define CSN_PIN 8

#define FORWARD_PIN 6
#define BACKWARD_PIN 7
#define LEFT_PIN 5
#define RIGHT_PIN 4

RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = { "1Node", "2Node" };
bool radio_num = 0;
static InputStat prev_input_stat = {0};

InputStat payload = {0};

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 disp(SCREEN_WIDTH, SCREEN_HEIGHT);

#define MOTOR_STATE_TEXT_X 0
#define MOTOR_STATE_TEXT_Y 0
void display_motor_state_text(MotorState motor_state) {
  disp.clearDisplay();
  disp.setTextColor(WHITE);
  disp.setTextSize(2);
  disp.setCursor(MOTOR_STATE_TEXT_X, MOTOR_STATE_TEXT_X);
  switch (motor_state) {
    case FORWARD:
      disp.print(F("FORWARD"));
      break;
    case REVERSE:
      disp.print(F("REVERSE"));
      break;
    case STOP:
      disp.print(F("STOP"));
      break;
  }
  disp.display();
}

void setup() {
  Serial.begin(9600);

  // OLED
  if (!disp.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    Serial.println(F("Failed to initialize display"));
  else
    Serial.println(F("Initialized display"));

  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  digitalWrite(FORWARD_PIN, LOW);
  digitalWrite(BACKWARD_PIN, LOW);
  digitalWrite(LEFT_PIN, LOW);
  digitalWrite(RIGHT_PIN, LOW);

  Serial.println("Setup complete. Starting ...");

  // display_motor_state_text(FORWARD);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address[radio_num]);
  radio.openReadingPipe(1, address[!radio_num]);
  radio.startListening();

  if (!radio.isChipConnected())
    Serial.println("chip is not connected correctly!");

  Serial.println("RX INITIALIZED");
}

void motor_forward() {
  Serial.println("FORWARD");
  analogWrite(FORWARD_PIN, 200);
  analogWrite(BACKWARD_PIN, 0);
}

void motor_reverse() {
  Serial.println("REVERSE");
  analogWrite(FORWARD_PIN, 0);
  analogWrite(BACKWARD_PIN, 200);
}

void motor_stop() {
  Serial.println("STOP");
  analogWrite(FORWARD_PIN, 0);
  analogWrite(BACKWARD_PIN, 0);
}

void motor_left() {
  Serial.println("LEFT");
  digitalWrite(LEFT_PIN, HIGH);
  digitalWrite(RIGHT_PIN, LOW);
}

void motor_right() {
  Serial.println("RIGHT");
  digitalWrite(LEFT_PIN, LOW);
  digitalWrite(RIGHT_PIN, HIGH);
}

void motor_straight() {
  Serial.println("STRAIGHT");
  digitalWrite(LEFT_PIN, LOW);
  digitalWrite(RIGHT_PIN, LOW);
}

void process_input(struct InputStat input) {
  uint8_t in = *(uint8_t *)&input;
  Serial.print("INPUT: ");
  Serial.print(in >> 3);
  Serial.print(" ");
  Serial.print((in >> 2) & 1);
  Serial.print(" ");
  Serial.print((in >> 1) & 1);
  Serial.print(" ");
  Serial.println((in     ) & 1);
  if (input.forward) {
    if (input.backward && !prev_input_stat.backward)
      motor_reverse();
    else
      motor_forward();
  } else if (input.backward)
    motor_reverse();
  else
    motor_stop();

  if (input.left) {
    if (input.right && !prev_input_stat.right)
      motor_right();
    else
      motor_left();
  } else if (input.right)
    motor_right();
  else
    motor_straight();

  prev_input_stat = input;

  // display_motor_state_text((MotorState)byte);
  // switch(byte) {
  //   case FORWARD:
  //     motor_forward();
  //     break;
  //   case REVERSE:
  //     motor_reverse();
  //     break;
  //   case STOP:
  //     motor_stop();
  //     break;
  //   case LEFT:
  //     motor_left();
  //     break;
  //   case RIGHT:
  //     motor_right();
  //     break;
  //   case STRAIGHT:
  //     motor_straight();
  //     break;
  //   default:
  //     Serial.print(F("Unknown input byte '"));
  //     Serial.print(byte);
  //     Serial.println(F("'"));
  // }
}

void loop() {
  uint8_t pipe;
  if (radio.available(&pipe)) {
    uint8_t bytes = radio.getPayloadSize();
    radio.read(&payload, bytes);

    Serial.print("received: ");
    Serial.println( (*(uint8_t *)&payload) );
    process_input(payload);
    // process_input(Serial.read());

    // while (Serial.available())
      // Serial.read();
  }
}
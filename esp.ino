#include <ESP32Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- SERVO ----
Servo s1, s2;
const int SERVO1 = 12;
const int SERVO2 = 19;

// ---- OLED DISPLAY (SPI) ----
const int OLED_CLK = 14;   // SCK
const int OLED_MOSI = 22;  // SDA (MOSI)
const int OLED_RES = 2;    // RES
const int OLED_DC = 4;     // DC
const int OLED_CS = 5;     // CS

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RES, OLED_CS);

// ---- POTENTIOMETER ----
const int POT_PIN = 34;    // ADC0

// ---- SERVO CONTROL ----
int target1 = 90, target2 = 90;  // Target angles
int current1 = 90, current2 = 90; // Current angles
int a1 = 90, a2 = 90;            // Servo positions
bool stopped = false;
unsigned long lastData = 0;
unsigned long lastUpdate = 0;
int speedDelay = 50;              // Milliseconds between servo steps

void setup() {
  Serial.begin(115200);

  pinMode(POT_PIN, INPUT);

  // ---- INITIALIZE OLED ----
  SPI.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Servo Control Ready");
  display.println("Waiting for commands...");
  display.display();
  delay(2000);

  // ---- INITIALIZE SERVOS ----
  s1.attach(SERVO1);
  s2.attach(SERVO2);

  s1.write(90);
  s2.write(90);

  // ---- INITIALIZE FAILSAFE TIMER ----
  lastData = millis();
}

void loop() {
  // ---- READ POTENTIOMETER (0-4095) and map to speed (500ms-1ms delay) ----
  int potValue = analogRead(POT_PIN);
  speedDelay = map(potValue, 0, 4095, 500, 1); // Higher pot = faster speed (1ms per degree)

  // ---- SERIAL CONTROL (always check, even if stopped) ----
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    int c = data.indexOf(',');

    if (c > 0) {
      target1 = constrain(data.substring(0, c).toInt(), 30, 150);
      target2 = constrain(data.substring(c + 1).toInt(), 30, 150);
      lastData = millis();
      stopped = false;  // Reset stopped flag when data arrives
    }
  }

  // ---- FAILSAFE (500ms timeout) ----
  if (millis() - lastData > 500) {
    stopped = true;
  }

  // ---- SMOOTH SERVO MOVEMENT ----
  if (!stopped && (millis() - lastUpdate > speedDelay)) {
    // Move servo 1 towards target
    if (current1 < target1) {
      current1++;
    } else if (current1 > target1) {
      current1--;
    }

    // Move servo 2 towards target
    if (current2 < target2) {
      current2++;
    } else if (current2 > target2) {
      current2--;
    }

    s1.write(current1);
    s2.write(current2);
    lastUpdate = millis();
  }

  // ---- FAILSAFE: Hold last position when stopped ----
  // Servos retain their last angle when stopped (no reset to 90)

  // ---- UPDATE DISPLAY ----
  updateDisplay(potValue);
}

void updateDisplay(int potValue) {
  display.clearDisplay();
  
  // Title
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("SERVO CTRL");
  
  display.setTextSize(1);
  display.setCursor(0, 18);
  
  // Servo 1 info
  display.print("S1: ");
  display.print(current1);
  display.print("  Tgt: ");
  display.println(target1);
  
  // Servo 2 info
  display.print("S2: ");
  display.print(current2);
  display.print("  Tgt: ");
  display.println(target2);
  
  // Speed and Potentiometer value
  display.print("Delay: ");
  display.print(speedDelay);
  display.print("ms  Pot: ");
  display.print(potValue);
  display.println("/4095");
  
  // Status
  display.setCursor(0, 56);
  display.print("Status: ");
  if (stopped) {
    display.println("STOPPED");
  } else {
    display.println("RUNNING");
  }
  
  display.display();
}

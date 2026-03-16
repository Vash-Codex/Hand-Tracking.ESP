# Hand-Tracking.ESP
Lightweight ESP32 project that maps real-time MediaPipe hand landmarks to two servos over serial, with smooth movement, OLED status, and manual keyboard controls.


This project reads hand landmarks from a webcam (MediaPipe) and sends mapped servo angles over serial to an ESP32. The ESP32 sketch moves two servos smoothly, shows status on an SPI SSD1306 OLED, and uses a potentiometer to adjust movement speed/failsafe.
Features

Hand-tracking control (MediaPipe) + keyboard fallback/manual controls
Smooth servo movement with configurable speed (potentiometer)
On-board OLED status display and failsafe when serial input stops
Hardware

ESP32 development board
2 x hobby servos (signal to GPIO 12 and 19)
SSD1306 OLED (SPI: CLK 14, MOSI 22, RES 2, DC 4, CS 5)
Potentiometer to ADC pin 34 (speed control)
External 5V (or appropriate) power for servos + common ground with ESP32
Software

Arduino sketch: esp.ino (runs on ESP32)
PC client: hand_tracking.py (Python + OpenCV + MediaPipe + pyserial)

Dependencies / Install

Arduino: ESP32 board support + libraries: ESP32Servo, Adafruit_SSD1306, Adafruit_GFX, SPI
Python (3.8+ recommended):
pip install opencv-python mediapipe pyserial
Wiring (summary)

Servo 1 signal → GPIO12
Servo 2 signal → GPIO19
Potentiometer middle → GPIO34, other ends to 3.3V and GND
OLED: CLK→14, MOSI→22, RES→2, DC→4, CS→5, plus VCC/GND
Ensure servo power supply can supply current and shares GND with ESP32
Usage

Upload esp.ino to the ESP32 (select correct board and port).
On the PC, edit hand_tracking.py to set the correct COM port (default shown in script is COM7).
Start the Python script:
python hand_tracking.py
Controls:
T — Toggle hand tracking / manual control
Q/W — Decrease/Increase Servo1
A/S — Decrease/Increase Servo2
R — Reset both servos to 90°
ESC — Exit
Potentiometer changes movement speed (delay per degree).

import cv2
import mediapipe as mp
import serial
import time

# ---------- SERIAL ----------
ser = serial.Serial("COM7", 115200, timeout=1)  # change COM port
time.sleep(2)

# ---------- MEDIAPIPE ----------
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1)
mp_draw = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)

# ---------- SERVO CONTROL ----------
servo1 = 90
servo2 = 90
use_hand_tracking = True  # Toggle between hand tracking and manual control

def map_range(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

def send_servo_command(s1, s2):
    """Send servo angles to ESP32"""
    try:
        ser.write(f"{s1},{s2}\n".encode())
    except:
        pass

while True:
    success, frame = cap.read()
    if not success:
        break

    frame = cv2.flip(frame, 1)
    h, w, _ = frame.shape
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb)

    # ---------- HAND TRACKING ----------
    if use_hand_tracking and result.multi_hand_landmarks:
        hand = result.multi_hand_landmarks[0]

        x = hand.landmark[9].x * w
        y = hand.landmark[9].y * h

        servo1 = map_range(x, 0, w, 30, 150)
        servo2 = map_range(y, h, 0, 30, 150)

        send_servo_command(servo1, servo2)
        mp_draw.draw_landmarks(frame, hand, mp_hands.HAND_CONNECTIONS)

    # ---------- KEYBOARD CONTROLS ----------
    key = cv2.waitKey(1) & 0xFF
    
    if key == 27:  # ESC to exit
        break
    elif key == ord('t'):  # T to toggle hand tracking
        use_hand_tracking = not use_hand_tracking
        mode = "Hand Tracking" if use_hand_tracking else "Manual Control"
        print(f"Mode: {mode}")
    
    # Servo 1 controls (Q/W or Arrow Up/Down)
    elif key in [ord('q'), ord('Q')]:
        servo1 = max(30, servo1 - 5)
        use_hand_tracking = False
        send_servo_command(servo1, servo2)
    elif key in [ord('w'), ord('W')]:
        servo1 = min(150, servo1 + 5)
        use_hand_tracking = False
        send_servo_command(servo1, servo2)
    
    # Servo 2 controls (A/S)
    elif key in [ord('a'), ord('A')]:
        servo2 = max(30, servo2 - 5)
        use_hand_tracking = False
        send_servo_command(servo1, servo2)
    elif key in [ord('s'), ord('S')]:
        servo2 = min(150, servo2 + 5)
        use_hand_tracking = False
        send_servo_command(servo1, servo2)
    
    # Reset to 90 degrees
    elif key in [ord('r'), ord('R')]:
        servo1 = 90
        servo2 = 90
        use_hand_tracking = False
        send_servo_command(servo1, servo2)

    # ---------- DISPLAY OVERLAY ----------
    mode_text = "Hand Tracking" if use_hand_tracking else "Manual Control"
    cv2.putText(frame, f"Mode: {mode_text}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    cv2.putText(frame, f"Servo1: {servo1}", (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)
    cv2.putText(frame, f"Servo2: {servo2}", (10, 110), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)
    
    cv2.putText(frame, "Controls:", (10, 150), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
    cv2.putText(frame, "Q/W:Servo1  A/S:Servo2  R:Reset  T:Toggle", (10, 170), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
    cv2.putText(frame, "ESC:Exit", (10, 190), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

    cv2.imshow("Hand Tracking Control", frame)

cap.release()
cv2.destroyAllWindows()
ser.close()
 
# Advanced-Home-Security-System---Arduino-

**Summary**

This home security system implements various Arduino functionalities and components, which include: a PIR motion sensor, a doorbell push button, a keypad for passcode entry, two indicator LEDs, and a servo motor mimicking a rotating security camera. This design is a modular approach where one component contributes to one of three security layers: access control, intrusion detection, and visual monitoring. 

**Instructions:**

**Hardware setup:**
-	Push button connected to D13
-	PIR sensor connected to D12
-	Keypad connected to D11 - D4
-	Green LED connected to D3
-	Red LED connected to D2
-	Servo motor connected to A5

**Software setup:**
- Use Tinkercad or Arduino IDE
- Then upload the provided cpp code to IDE

**How the system works:**
- PIR motion detector: Motion detected -> logs to serial monitor
- Push button (doorbell) -> logs to serrial monitor
- KeyPad: requires a 4 digit code, you have 3 tries. 
- Successfully authenticated -> Green LED lit up
- 3 Consecutive fails -> Red LED flash continuously
- Servo motor (camera): simulate camera movement every 5 seonds




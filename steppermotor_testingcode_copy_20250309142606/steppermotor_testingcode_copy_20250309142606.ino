// Define motor control pins
#define COIL_A1 D0
#define COIL_A2 D1
#define COIL_B1 D2
#define COIL_B2 D3
 
// Stepper motor step sequence (full step mode)
const int stepSequence[4][4] = {
    {1, 0, 1, 0}, // Step 1
    {0, 1, 1, 0}, // Step 2
    {0, 1, 0, 1}, // Step 3
    {1, 0, 0, 1}  // Step 4
};
 
void setup() {
    pinMode(COIL_A1, OUTPUT);
    pinMode(COIL_A2, OUTPUT);
    pinMode(COIL_B1, OUTPUT);
    pinMode(COIL_B2, OUTPUT);
}
 
void stepMotor(int stepDelay, bool reverse) {
    for (int i = 0; i < 200; i++) { // Adjust steps per revolution
        for (int step = 0; step < 4; step++) {
            int s = reverse ? (3 - step) : step; // Reverse direction if needed
            digitalWrite(COIL_A1, stepSequence[s][0]);
            digitalWrite(COIL_A2, stepSequence[s][1]);
            digitalWrite(COIL_B1, stepSequence[s][2]);42
            digitalWrite(COIL_B2, stepSequence[s][3]);
            delay(stepDelay); // Adjust speed (smaller delay = faster motor)
        }
    }
}
 
void loop() {
    stepMotor(10, false); // Move forward
    delay(1000);
    stepMotor(10, true); // Move backward
    delay(1000);
}
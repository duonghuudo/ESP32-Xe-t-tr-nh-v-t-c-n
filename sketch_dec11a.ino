// Pin definitions
#define IN1 12
#define IN2 13
#define IN3 14
#define IN4 27
#define NEAR_PIN 33

// Tần số và độ phân giải PWM
#define PWM_FREQ 5000
#define PWM_RES 8

volatile bool objectDetected = false;
volatile unsigned long lastInterruptTime = 0;

// Hàm điều chỉnh tốc độ (0-255)
void setMotorSpeed(int pin, int speed) {
  ledcWrite(pin, speed); // Thiết lập tốc độ trên chân PWM
}

// Hàm ngắt
void IRAM_ATTR detectObject() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > 200) { // Debounce 200ms
    objectDetected = digitalRead(NEAR_PIN) == 1;
    lastInterruptTime = currentTime;
  }
}

void setup() {
  // Cấu hình chân PWM với API mới
  ledcAttach(IN1, PWM_FREQ, PWM_RES);
  ledcAttach(IN2, PWM_FREQ, PWM_RES);
  ledcAttach(IN3, PWM_FREQ, PWM_RES);
  ledcAttach(IN4, PWM_FREQ, PWM_RES);

  // Cấu hình chân cảm biến
  pinMode(NEAR_PIN, INPUT_PULLUP);

  // Gắn ngắt
  attachInterrupt(digitalPinToInterrupt(NEAR_PIN), detectObject, CHANGE);

  // Bắt đầu giao tiếp Serial
  Serial.begin(115200);
}

void loop() {
  static bool obstacleHandled = false;

  if (objectDetected && !obstacleHandled) {
    Serial.println("Object detected! Handling obstacle...");
    handleObstacle();
    obstacleHandled = true;
    objectDetected = false;
  } else if (!objectDetected) {
    moveForward(128);
    obstacleHandled = false;
  }
}

// Các hàm điều khiển động cơ
void moveForward(int speed) {
  Serial.println("moveForward");
  setMotorSpeed(IN1, speed);
  setMotorSpeed(IN2, 0);
  setMotorSpeed(IN3, speed);
  setMotorSpeed(IN4, 0);
}

void turnLeft() {
  Serial.println("turnLeft");
  setMotorSpeed(IN1, 0);
  setMotorSpeed(IN2, 128);
  setMotorSpeed(IN3, 128);
  setMotorSpeed(IN4, 0);
  delay(500);
}

void turnRight() {
  Serial.println("turnRight");
  setMotorSpeed(IN1, 128);
  setMotorSpeed(IN2, 0);
  setMotorSpeed(IN3, 0);
  setMotorSpeed(IN4, 128);
  delay(500);
}

void turnAround() {
  Serial.println("turnAround");
  setMotorSpeed(IN1, 0);
  setMotorSpeed(IN2, 128);
  setMotorSpeed(IN3, 0);
  setMotorSpeed(IN4, 128);
  delay(700);
}

void handleObstacle() {
  int action = random(0, 2);
  switch (action) {
    case 0:
      turnAround();
      turnLeft();
      break;
    case 1:
      turnAround();
      turnRight();
      break;
  }
}

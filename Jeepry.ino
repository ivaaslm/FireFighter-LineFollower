#include <L298N.h>

// ================= MOTOR PIN =================
#define ENA 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10
#define ENB 5

L298N motorKiri(ENB, IN4, IN3);
L298N motorKanan(ENA, IN2, IN1);

// ================= SENSOR GARIS =================
#define S1 3
#define S2 12
#define S3 13
// S4 MATI
#define S5 4

// ================= SENSOR API =================
#define FLAME_PIN A2
#define FLAME_THRESHOLD 200

// ================= FAN =================
#define FAN_INA A0
#define FAN_INB A1

// ================= PID =================
float Kp = 4.5;
float Kd = 1.2;

int error, lastError = 0;
int baseSpeed = 110;
int maxSpeed  = 120;
int lastDirection = 0;

// ================= FLAG API =================
bool fireHandled = false;

// ================= SETUP =================
void setup() {
  Serial.begin(9600);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S5, INPUT);

  pinMode(FAN_INA, OUTPUT);
  pinMode(FAN_INB, OUTPUT);

  fanOff();
}

// ================= LOOP =================
void loop() {
  int flameValue = analogRead(FLAME_PIN);

  Serial.print("Flame: ");
  Serial.println(flameValue);

  // ===== API TERDETEKSI =====
  if (flameValue < FLAME_THRESHOLD && !fireHandled) {
    Serial.println("API TERDETEKSI");

    stopRobot();
    fanOn();
    delay(5000);
    fanOff();

    Serial.println("PUTAR BALIK");
    turnBack();

    fireHandled = true;
    return;
  }

  // ===== MODE NORMAL =====
  lineFollower();
}

// ================= LINE FOLLOWER =================
void lineFollower() {
  int position = readLine();
  error = 2000 - position;

  int P = error;
  int D = error - lastError;
  lastError = error;

  int pid = (Kp * P) + (Kd * D);

  int leftSpeed  = constrain(baseSpeed - pid, -maxSpeed, maxSpeed);
  int rightSpeed = constrain(baseSpeed + pid, -maxSpeed, maxSpeed);

  drive(leftSpeed, rightSpeed);
}

// ================= SENSOR GARIS + MEMORY =================
int readLine() {
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s5 = digitalRead(S5);

  // === KONDISI HOME / SEMUA SENSOR HITAM ===
  if (s1 == 0 && s2 == 0 && s3 == 0 && s5 == 0) {
    delay(500);
    turnBack();
    stopRobot();
    Serial.println("HOME / FINISH");
    while (1); // STOP SELAMANYA
  }

  if (s1 == 0) { lastDirection = -1; return 0; }
  if (s2 == 0) { lastDirection = -1; return 1000; }
  if (s3 == 0) { lastDirection = 0;  return 2000; }
  if (s5 == 0) { lastDirection = 1;  return 3000; }

  if (lastDirection == -1) return 0;
  if (lastDirection ==  1) return 4000;

  return 2000;
}

// ================= MOTOR =================
void drive(int kiri, int kanan) {
  if (kiri > 0) {
    motorKiri.setSpeed(kiri);
    motorKiri.backward();
  } else {
    motorKiri.setSpeed(-kiri);
    motorKiri.forward();
  }

  if (kanan > 0) {
    motorKanan.setSpeed(kanan);
    motorKanan.backward();
  } else {
    motorKanan.setSpeed(-kanan);
    motorKanan.forward();
  }
}

void stopRobot() {
  motorKiri.stop();
  motorKanan.stop();
}

// ================= PUTAR BALIK (PIVOT) =================
void turnBack() {
  int turnSpeed = 150;

  // kanan MAJU
  motorKanan.setSpeed(turnSpeed);
  motorKanan.backward();

  // kiri MUNDUR
  motorKiri.setSpeed(turnSpeed);
  motorKiri.forward();

  delay(1000);
  stopRobot();
}

// ================= FAN =================
void fanOn() {
  digitalWrite(FAN_INA, HIGH);
  digitalWrite(FAN_INB, LOW);
}

void fanOff() {
  digitalWrite(FAN_INA, LOW);
  digitalWrite(FAN_INB, LOW);
}

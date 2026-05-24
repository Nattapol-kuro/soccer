#include <POP32.h>
#include <POP32_Huskylens.h>
// #include <Wire.h>
// #include <MPU6050.h>
POP32_Huskylens huskylens;
// MPU6050 imu;
#define degToRad 0.0174f
#define sin30 sin(30.f * degToRad)
#define cos30 cos(30.f * degToRad)
// ค่าที่ใช้ปรับหุ่นให้เข้าด้านหน้าตรงลูกบอล
#define rot_Kp 1.5
#define rot_Ki 0.0
#define rot_Kd 0.15
#define sp_rot 160      // ค่า setpoint ที่ลูกบอลอยู่ตรงกลางกล้องแกน x  320/2 = 160
#define rotErrorGap 15  // ค่า Error ที่ยอมให้หุ่นหยุดทำงาน
#define idleSpd 30      // ค่าความเร็วการหมุนเมื่อไม่เจอลูกบอล
float rot_error, rot_pError, rot_i, rot_d, rot_w;
int ballPosX = 160;
// ค่าที่ใช้ปรับหุ่นให้เข้าใกล้ลูกบอล
#define fli_Kp 1.125
#define fli_Ki 0.0
#define fli_Kd 0.0
#define flingErrorGap 15  // ค่า Error ที่ยอมให้หุ่นหยุดทำงาน
float spFli = 170;        // ค่า setpoint ที่ยอมให้ลูกบอลอยู่ใกล้หุ่นมากที่สุด อาจเริ่มที่จุดกลางจอ แกน Y
float fli_error, fli_pError, fli_i, fli_d, fli_spd;
int ballPosY;
// ค่าที่ใช้ปรับหุ่นให้ตรงทิศอ้างอิง
#define alignErrorGap 4
float vecCurve, radCurve;
int discoveState = 1;
// ค่าที่ใช้รักษาทิศหุ่นยนต์
#define head_Kp 1.25f
#define head_Ki 0.0001f
#define head_Kd 0.125f
float head_error, head_pError, head_w, head_d, head_i;
/* >> ball shooting <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define limPin A0
#define reloadSpd 70
//////////////////////////////////
#define SenF A1
#define SenL A2
#define SenR A3
#define Sen_Front (3333 + 452) / 2
#define Sen_Left (3969 + 1015) / 2
#define Sen_Right (3956 + 500) / 2
/////////////////////////////////
// #define Xaxis_Kp 1.2
// #define Xaxis_Ki 0.005
// #define Xaxis_Kd 0.12
#define Xaxis_Kp 1.25
#define Xaxis_Ki 0.01
#define Xaxis_Kd 0.14

#define Yaxis_Kp 1.5
#define Yaxis_Kd 0
float Xaxis_Error, Xaxis_PvEror, Xaxis_I, Xaxis_D, Xaxis_spd;
float Yaxis_Error, Yaxis_PvEror, Yaxis_D, Yaxis_spd;
int lastgoalcoord;
int gaolX = 160;
int count, bypassLR, bypassC, countC;

struct MergedGoal {
  float x, y, w, h;
  bool found;
};

#define SPEED_SCALE 0.00000000625f
#define FIELD_Y_MAX 95.0f
#define FIELD_Y_MIN -95.0f
#define FIELD_X_MAX 50.0f
#define FIELD_X_MIN -50.0f

#define OUR_GOAL_Y -69.5f
#define CENTER_Y 0.0f
#define FORWARD_Y 30.0f

float estimatedX = 0.0f;
float estimatedY = 0.0f;

int menu() {
  while (1) {
    int NUMBER_KNOB = 4;
    int x = knob(1, NUMBER_KNOB);
    x = constrain(x, 1, NUMBER_KNOB);
    oled.clear();
    oled.text(0, NUMBER_KNOB, "======MEUN======", x);
    oled.text(2, 0, "CASE = %d", x);
    if (x == 1) oled.text(4, 0, ">chksens", x);
    if (x == 2) oled.text(4, 0, ">CoordsBall", x);
    if (x == 3) oled.text(4, 0, ">D_B", x);
    if (x == 4) oled.text(4, 0, ">S_B3", x);

    oled.text(6, 5, "PRESS_SW_OK", x);
    oled.show();
    if (SW_OK()) {
      oled.clear();
      return x;
    }
    if (SW_A()) {
      shoot();
      reload();
    }
    if (SW_B()) {
      Auto_zero();
    }
    delay(100);
  }
}
int x = 1;

void MENU() {
  int x = menu();
  if (x == 1) {
    while (1) {
      chksens();
    }
  } else if (x == 2) {
    while (1) {
      CoordsBall();
    }
  } else if (x == 3) {
    while (1) {
      D_B();
    }
  } else if (x == 4) {
    while (1) {
      S_B3();
    }
  }
}

float thetaRad, vx, vy, spd1, spd2, spd3;
void zeroYaw() {
  Serial1.begin(115200);
  delay(100);
  // Sets data rate to 115200 bps
  Serial1.write(0XA5);
  delay(100);
  Serial1.write(0X54);
  delay(100);
  // pitch correction roll angle
  Serial1.write(0XA5);
  delay(100);
  Serial1.write(0X55);
  delay(100);
  // zero degree heading
  Serial1.write(0XA5);
  delay(100);
  Serial1.write(0X52);
  delay(100);
  // automatic mode
}

float pvYaw, lastYaw;
uint8_t rxCnt = 0, rxBuf[8];
bool getIMU() {
  while (Serial1.available()) {
    rxBuf[rxCnt] = Serial1.read();
    if (rxCnt == 0 && rxBuf[0] != 0xAA) return;
    rxCnt++;
    if (rxCnt == 8) {  // package is complete
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) {  // data package is correct
        int16_t angle = (int16_t)(rxBuf[1] << 8 | rxBuf[2]);
        angle = angle % 36000;
        if (angle > 17900) angle -= 36000;
        if (angle < -17900) angle += 36000;
        pvYaw = angle / 100.f;
        return true;
      }
    }
  }
  // Serial.println(pvYaw);
  return false;
}

void Auto_zero() {
  zeroYaw();
  getIMU();
  int timer = millis();
  oled.clear();
  oled.text(1, 2, "Setting zero");
  pvYaw = 90.0f;
  while (abs(pvYaw) > 0.1) {  //วนทำซ้ำจนกว่าองศาจะน้อยกว่า 0.05
    if (getIMU()) {
      oled.text(3, 6, "Yaw: %f  ", pvYaw);
      oled.show();
      // beep();
      if (millis() - timer > 3000) {  //เวลาเกิน 5 วิให้ zeroYaw อีกครั้ง
        zeroYaw();
        timer = millis();
      }
    }
  }
  oled.clear();
  oled.show();
  sound(3000, 100);
}

void wheel(int s1, int s2, int s3) {
  motor(1, s1);
  motor(2, s2);
  motor(3, s3);
}

void holonomic(float spd, float theta, float omega) {
  thetaRad = theta * degToRad;
  vx = spd * cos(thetaRad);
  vy = spd * sin(thetaRad);
  spd1 = vy * cos30 - vx * sin30 + omega;
  spd2 = -vy * cos30 - vx * sin30 + omega;
  spd3 = vx + omega;
  wheel(spd1, spd2, spd3);
}

float angleDiff(float target, float current) {
  float diff = target - current;
  if (diff > 180) diff -= 360;
  else if (diff < -180) diff += 360;
  return diff;
}

void heading(float spd, float theta, float spYaw) {
  getIMU();
  head_error = angleDiff(spYaw, pvYaw);

  head_i = constrain(head_i + head_error, -50, 50);
  head_d = head_error - head_pError;
  head_w = (head_error * head_Kp) + (head_i * head_Ki) + (head_d * head_Kd);
  head_w = constrain(head_w, -100, 100);


  int Speed = 0;
  if (abs(head_error) > 40) {
    Speed = 0;
  } else if (abs(head_error) <= 40 && abs(head_error) > 15) {
    Speed = spd * 0.5;
  } else {
    Speed = spd;
  }
  holonomic(Speed, theta, head_w);

  head_pError = head_error;
}

void shoot() {
  // beep();
  motor(4, reloadSpd);
  delay(150);
  motor(4, 0);
  delay(50);
}

int timer = 0;
void reload() {
  motor(4, reloadSpd);
  timer = 0;
  for (int i = 0; i < 2500; i++) {
    timer++;
    if (analogRead(limPin) > 1000) break;
    delay(1);
  }
  if (timer == 2500) {     // ถ้าก้านยิงติด
    motor(4, -reloadSpd);  // เลื่อนก้านยิงไปข้างหน้า
    delay(500);            //ก่อน 0.5 วินาที
    motor(4, reloadSpd);
    timer = 0;
    for (int i = 0; i < 2500; i++) {
      timer++;
      if ((analogRead(limPin) > 1000)) break;
      delay(1);
    }
  }
  motor(4, 0);
}

long now, lastTime;

void setup() {
  Serial.begin(9600);
  while (!huskylens.begin(Wire)) {
    oled.text(1, 0, "Huskylens failed!");
    oled.show();
    delay(100);
  }
  delay(1000);
  Auto_zero();
  delay(500);
  // reload();
  // reload();
  waitSW_OK_bmp();
  // if (SW_A()) {
  //   shoot();
  //   delay(150);
  //   reload();
  // }
  // if (SW_B()) {
  //   Auto_zero();
  // }
  // int looptime = millis();
  // while (millis() - looptime <= 1000) {

  //   getIMU();
  //   heading(60, 90, 0);
  //   now = micros();
  //   updatePosition(60, 90, now - lastTime);


  //   Serial.print("X: ");
  //   Serial.print(estimatedX);
  //   Serial.print("  Y: ");
  //   Serial.print(estimatedY);
  //   Serial.print("  Yaw: ");
  //   Serial.println(pvYaw, 1);

  //   lastTime = now;
  // }
  // wheel(0, 0, 0);
  // Serial.begin(115200);
  // Wire.begin();
  // imu.initialize();
}

void loop() {
  gg();
  // oled.text(4, 0, "lim=%d     ", analog(0));
  // oled.show();
  // AtanTrack3();
  // waitSW_OK_bmp();
  // bump();
  // S_B();
  // heading(100, 45, 0);
  // long looptime = millis();
  // while (millis() - looptime <= 300) {
  //   heading(100, 90, 45);
  // }
  // wheel(0, 0, 0);
  // MENU();
  // Atantrak_dibbling();
  // AtanTrack3();
  // Bumping();
  // chksens();
  // heading(100,90,0);
  // bool seeBall = huskylens.updateBlocks() && huskylens.blockSize[1];
  // if (seeBall) {
  //   ballPosX = huskylens.blockInfo[1][0].x;
  //   ballPosY = huskylens.blockInfo[1][0].y;
  // }
  // if (seeBall) {
  //   atantrak();
  // } else {
  //   bg();
  // }
  // TrackXaxis();
  // if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
  //   ballPosX = huskylens.blockInfo[1][0].x;
  //   ballPosY = huskylens.blockInfo[1][0].y
  // MergedGoal goal = getMergedGoalAll();
  //     Serial.print("ballPosX: ");
  //     Serial.print(ballPosX);
  //     Serial.print(" gx: ");
  //     Serial.println(gx);
  //     Serial.print("ID2: ");
  //     Serial.print(huskylens.blockSize[2]);
  //     Serial.print(" ID3: ");
  //     Serial.println(huskylens.blockSize[3]);
  //     oled.clear();

  //     // ===== Goal =====
  // oled.text(1, 0, "G:%f, %f    ", goal.x, goal.y);
  // oled.text(2, 0, "W:%f H:%f    ", goal.w, goal.h);

  //     // ===== Align =====
  //     oled.text(4, 0, "AX:%f", ax);
  //     oled.text(5, 0, "AY:%f", ay);
  //     oled.text(6, 0, "AW:%f", aw);

  // oled.show();
  // }
  // }
  // dibbling_Search();
  //  backtogoal_Bumping();
  // atantrak();


  // int16_t ax, ay, az;

  // imu.getAcceleration(&ax, &ay, &az);

  // Serial.print("ax: ");
  // Serial.print(ax);
  // Serial.print(" ay: ");
  // Serial.print(ay);
  // Serial.print(" az: ");
  // Serial.println(az);

  // delay(50);
  // CoordsBall();
  // chksens();
}

void chksens() {
  while (1) {
    oled.text(2, 0, "C=%d     ", analog(1));
    oled.text(3, 0, "L=%d     ", analog(2));
    oled.text(4, 0, "R=%d     ", analog(3));
    oled.show();
  }
}

void Read() {
  Serial.print(analogRead(A1));
  Serial.print("           ");
  Serial.print(analogRead(A2));
  Serial.print("           ");
  Serial.println(analogRead(A3));
}

void CoordsBall() {
  oled.clear();
  while (1) {
    huskylens.updateBlocks();

    // Serial.print("ballX  ");
    // Serial.print(ballPosX);
    // Serial.print("\t\t");
    // Serial.print("ballY  ");
    // Serial.print(ballPosY);

    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;

    float dx = huskylens.blockInfo[1][0].x - 165;
    float dy = 160 - huskylens.blockInfo[1][0].y;

    float Setha = atan2(dy, dx) * (180 / PI);
    float SethaPos;

    if (Setha >= 0) {
      SethaPos = Setha;
    } else if (Setha < 0) {
      SethaPos = 180 + Setha;
    }

    Yaxis_Error = 190 - huskylens.blockInfo[1][0].y;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    Yaxis_PvEror = Yaxis_Error;

    // Serial.println("SethaPos = ");
    // Serial.print(SethaPos);
    // Serial.print("\t\t");
    // Serial.println("Yaxis_spd = ");
    // Serial.println(Yaxis_spd);
  }
}

MergedGoal getMergedGoalAll() {
  MergedGoal g = { 0, 0, 0, 0, false };
  float minX = 320, maxX = 0, minY = 240, maxY = 0;

  if (!huskylens.updateBlocks()) return;

  for (int id = 2; id <= 3; id++) {
    int count = huskylens.blockSize[id];
    for (int i = 0; i < count; i++) {
      float cx = huskylens.blockInfo[id][i].x;
      float cy = huskylens.blockInfo[id][i].y;
      float cw = huskylens.blockInfo[id][i].width;
      float ch = huskylens.blockInfo[id][i].height;

      float left = cx - cw / 2.0;
      float right = cx + cw / 2.0;
      float top = cy - ch / 2.0;
      float bottom = cy + ch / 2.0;

      if (left < minX) minX = left;
      if (right > maxX) maxX = right;
      if (top < minY) minY = top;
      if (bottom > maxY) maxY = bottom;
      g.found = true;
    }
  }

  if (!g.found) return g;

  g.w = maxX - minX;
  g.h = maxY - minY;
  g.x = minX + g.w / 2.0;
  g.y = minY + g.h / 2.0;

  return g;
}

void updatePosition(double spd, double theta, double deltaTime) {

  double worldAngle = (theta - pvYaw) * degToRad;
  double dx = SPEED_SCALE * spd * cos(worldAngle) * deltaTime;
  double dy = SPEED_SCALE * spd * sin(worldAngle) * deltaTime;
  estimatedX += dx;
  estimatedY += dy;

  // Serial.print("dx: ");
  // Serial.print(dx);
  // Serial.print("  dy: ");
  // Serial.print(dy);
  // Serial.print("  worldAngle: ");
  // Serial.print(worldAngle);
  // Serial.print("  cos(worldAngle): ");
  // Serial.print(cos(worldAngle));
  // Serial.print("  sin(worldAngle): ");
  // Serial.print(sin(worldAngle));
  // Serial.print("  deltaTime: ");
  // Serial.println(deltaTime);

  estimatedX = constrain(estimatedX, FIELD_X_MIN, FIELD_X_MAX);
  estimatedY = constrain(estimatedY, FIELD_Y_MIN, FIELD_Y_MAX);
}
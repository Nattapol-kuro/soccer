void D_B() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    dribblingtothegoal();
  } else {
    backtogoal();
  }
}

int bypass = 0;
void dribblingtothegoal() {
  count = 2;
  long looptime = millis();
  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    getIMU();

    float ballX = huskylens.blockInfo[1][0].x;
    float ballY = huskylens.blockInfo[1][0].y;
    float ballW = huskylens.blockInfo[1][0].width;

    MergedGoal goal = getMergedGoalAll();

    // ======= Phase 0 & 1: เห็นโกล =======
    if (goal.found) {
      float goalLeft = goal.x - (goal.w / 2.0);
      float goalRight = goal.x + (goal.w / 2.0);

      bool ballInGoalX = (ballX > goalLeft) && (ballX < goalRight);
      bool ballInGoalY = (ballY > goal.y) && (ballY > 220) && (goal.h > 60);

      // ======= Phase 1: ยิง =======
      if (ballInGoalX && ballInGoalY || analogRead(A1) > Sen_Front) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
          wheel(0, 0, 0);
          return;
        }

        shoot();
        wheel(0, 0, 0);
        unsigned long t = millis();
        while (millis() - t < 600) holonomic(100, 270, 0);
        wheel(0, 0, 0);
        reload();
        return;
      }

      // ======= Phase 0: จัดบอลเข้ากรอบโกล =======
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta, speed, rot;
      float goalYaw = alignErr * 0.2;  // แปลง goalError เป็นองศาที่ต้องหมุน

      if (!ballInGoalX) {
        // บอลซ้าย/ขวาโกล → เลื่อนหุ่นจัดบอลเข้ากรอบ X
        theta = constrain(90.0 + (alignErr * 0.4), 45, 135);
        rot_w = constrain(-alignErr * 0.2, -15, 15);
        speed = 90;
      } else {
        // บอลใน X แล้ว แต่ยังไม่ใกล้พอ → วิ่งตรงเข้า
        theta = constrain(90.0 - (ballError * 0.25), 60, 120);
        rot_w = 0;
        // goalYaw = 0;
        speed = 70;
      }
      if (bypass < count) {
        looptime = millis();
        if (analogRead(SenL) > Sen_Left) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            bypass++;
            holonomic(100, 35, 0);
          }
          wheel(0, 0, 0);
        } else if (analogRead(SenR) > Sen_Right) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            bypass++;
            holonomic(100, 145, 0);
          }
          wheel(0, 0, 0);
        } else {
          holonomic(speed, theta, rot_w);
        }
      }
      // return;
    }

    // // ======= Phase 1.5: เห็นโกลน้อยกว่า 50% → หันหน้าหาโกล =======
    if (goal.found && goal.w < 160) {
      float goalError = goal.x - 160.0;  // + = โกลอยู่ขวา, - = โกลอยู่ซ้าย
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta = constrain(90.0 - (ballError * 0.25), 45, 135);
      float rot_w = constrain(-alignErr * 0.1, -20, 20);
      float speed = 80;

      //   // Serial.print("goalError: ");
      //   // Serial.print(goalError);
      // Serial.print(" | rot: ");
      // Serial.println(rot_w);
      //   // Serial.print(" | goal.w: ");
      //   // Serial.println(goal.w);
      if (bypass < count) {
        looptime = millis();
        if (analogRead(SenL) > Sen_Left) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            bypass++;
            holonomic(100, 35, 0);
          }
          wheel(0, 0, 0);
        } else if (analogRead(SenR) > Sen_Right) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            bypass++;
            holonomic(100, 145, 0);
          }
          wheel(0, 0, 0);
        } else {
          holonomic(speed, theta, rot_w);
        }
      }
      continue;
    }

    // ======= Phase 2: ไม่เห็นโกลเลย → เลี้ยงบอลไปข้างหน้า =======
    else {
      float ballError = ballX - 160.0;
      float theta = constrain(90.0 - (ballError * 0.25), 50, 130);
      float rot = constrain(angleDiff(0, pvYaw) * 1.5, -20, 20);

      float speed;
      if (abs(ballError) < (ballW / 2.0)) {
        speed = 100;
      } else {
        speed = 80;
      }
      if (bypass < count) {
        looptime = millis();
        if (analogRead(SenL) > Sen_Left) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            bypass++;
            holonomic(100, 35, 0);
          }
          wheel(0, 0, 0);
        } else if (analogRead(SenR) > Sen_Right) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            bypass++;
            holonomic(100, 145, 0);
          }
          wheel(0, 0, 0);
        } else {
          heading(speed, theta, 0);
        }
      }
    }
  }
  wheel(0, 0, 0);
}
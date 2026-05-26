void S_B3() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    AtanTrack3();
  } else {
    backtogoal();
  }
}

// void TrackXaxis() {
//   if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//     Xaxis_Error = huskylens.blockInfo[1][0].x - 160;
//     Xaxis_D = Xaxis_Error - Xaxis_PvEror;
//     Xaxis_spd = (Xaxis_Error * Xaxis_Kp) + (Xaxis_D * Xaxis_Kd);
//     if (abs(Xaxis_spd) < 25 && abs(Xaxis_Error) > 6) {
//       Xaxis_spd = (Xaxis_spd > 0) ? 25 : -25;
//     } else Xaxis_spd = constrain(Xaxis_spd, -80, 80);
//     Xaxis_PvEror = Xaxis_Error;
//     heading(Xaxis_spd, 0, 0);
//   }
// }

void Atantrak_dibbling() {
  if ((huskylens.updateBlocks() && huskylens.blockSize[1])) {
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;

    float QuaDrantX = ballPosX - 160;
    float QuaDrantY = 200 - ballPosY;
    float Setha = atan2(QuaDrantY, QuaDrantX) * (180.0 / PI);
    float SethaPos;
    // if (Setha >= 0) SethaPos = Setha;
    // else SethaPos = 180 + Setha;

    Yaxis_Error = 210 - ballPosY;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -30, 100);
    Yaxis_PvEror = Yaxis_Error;
    getIMU();
    // Serial.print(" | X : ");
    // Serial.print(QuaDrantX);
    // Serial.print(" | Y : ");
    // Serial.print(QuaDrantY);
    // Serial.print(" | atan : ");
    // Serial.print(Setha);
    // Serial.print(" | Pos : ");
    // Serial.println(SethaPos);
    if (Yaxis_Error >= 40) {
      holonomic(Yaxis_spd, Setha, 0);
    } else {
      heading(Yaxis_spd, Setha, 0);
    }
    if (abs(Yaxis_Error) <= 10) {
      TrackXaxis();
      if (abs(Xaxis_Error) <= 25) {
        getIMU();
        dribbling();
      }
    }
  }
}

bool getGoalCenter(int &goalX) {
  float sumX = 0;
  float sumWeight = 0;

  // เช็คทั้ง ID 2 และ 3
  for (int id = 2; id <= 3; id++) {

    int count = huskylens.blockSize[id];

    for (int i = 0; i < count; i++) {

      int x = huskylens.blockInfo[id][i].x;
      int w = huskylens.blockInfo[id][i].width;
      int h = huskylens.blockInfo[id][i].height;

      float area = w * h;

      sumX += x * area;
      sumWeight += area;
    }
  }

  if (sumWeight > 0) {
    goalX = sumX / sumWeight;
    return true;
  }

  return false;
}

void dribbling() {
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
      bool ballInGoalY = (ballY > goal.y) && (ballY > 200) && (goal.h > 60);

      // ======= Phase 1: ยิง =======
      if (ballInGoalX && ballInGoalY || analogRead(A1) > Sen_Front) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
          wheel(0, 0, 0);
          return;
        }
        unsigned long t = millis();
        while (millis() - t < 600) holonomic(100, 270, 0);
        wheel(0, 0, 0);
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

      holonomic(speed, theta, rot_w);
      // return;
    }

    // // ======= Phase 1.5: เห็นโกลน้อยกว่า 50% → หันหน้าหาโกล =======
    if (goal.found && goal.w < 160) {
      float goalError = goal.x - 160.0;  // + = โกลอยู่ขวา, - = โกลอยู่ซ้าย
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta = constrain(90.0 - (ballError * 0.25), 45, 135);
      float rot_w = constrain(-alignErr * 0.1, -20, 20);
      float speed = 100;

      //   // Serial.print("goalError: ");
      //   // Serial.print(goalError);
      // Serial.print(" | rot: ");
      // Serial.println(rot_w);
      //   // Serial.print(" | goal.w: ");
      //   // Serial.println(goal.w);

      holonomic(speed, theta, rot_w);
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

      heading(speed, theta, 0);
    }
  }

  wheel(0, 0, 0);
}

int lastError = 0;
long errorStarttime = 0;

void TrackXaxis2() {
  if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) return;

  Xaxis_Error = huskylens.blockInfo[1][0].x - 160;
  Xaxis_D = Xaxis_Error - Xaxis_PvEror;

  // I term — reset ถ้า error ข้ามศูนย์ ป้องกัน windup
  if ((Xaxis_Error > 0) != (Xaxis_PvEror > 0)) Xaxis_I = 0;
  else Xaxis_I += Xaxis_Error;
  Xaxis_I = constrain(Xaxis_I, -200, 200);

  Xaxis_spd = (Xaxis_Error * 0.6)
              + (Xaxis_I * Xaxis_Ki)
              + (Xaxis_D * 0.8);

  if (abs(Xaxis_Error) < 12) {
    Xaxis_spd = 0;  // deadzone
  } else if (abs(Xaxis_spd) < 12 && abs(Xaxis_Error) > 12) {
    Xaxis_spd = (Xaxis_spd > 0) ? 15 : -15;  // min speed
  } else {
    Xaxis_spd = constrain(Xaxis_spd, -80, 80);
  }

  Xaxis_PvEror = Xaxis_Error;
  lastError = Xaxis_Error;
  heading(Xaxis_spd, 0, 0);
}

void AtanTrack3() {
  if ((huskylens.updateBlocks() && huskylens.blockSize[1])) {
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;
    float QuaDrantX = huskylens.blockInfo[1][0].x - 150;
    float QuaDrantY = 180 - huskylens.blockInfo[1][0].y;
    float TanTheta = QuaDrantY / QuaDrantX;
    float Setha = atan(TanTheta) * (180 / PI);
    float SethaPos;
    float DisTanT = sqrt(pow(abs(QuaDrantX), 2) + pow(abs(QuaDrantY), 2));
    if (Setha >= 0) {
      SethaPos = Setha;
    } else if (Setha < 0) {
      SethaPos = 180 + Setha;
    } else if (SethaPos <= 100 && SethaPos >= 80) {
      SethaPos = 90;
    }
    // SetYaw();
    Yaxis_Error = 160 - huskylens.blockInfo[1][0].y;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    Yaxis_PvEror = Yaxis_Error;

    getIMU();
    // long looptime = millis();
    // if (analogRead(SenL) > Sen_Left) {
    //   looptime = millis();
    //   while (millis() - looptime <= 150) {
    //     holonomic(100, 35, 0);
    //   }
    //   wheel(0, 0, 0);
    // } else if (analogRead(SenR) > Sen_Right) {
    //   looptime = millis();
    //   while (millis() - looptime <= 150) {
    //     holonomic(100, 145, 0);
    //   }
    //   wheel(0, 0, 0);
    // } else {
      heading(Yaxis_spd, SethaPos, 0);
    // }

    float goalEstX, goalEstY, goalEstWidth;
    if (huskylens.blockSize[2]) {
      goalEstX = huskylens.blockInfo[2][0].x;
      goalEstY = huskylens.blockInfo[2][0].y;
      goalEstWidth = huskylens.blockInfo[2][0].width;
    } else if (huskylens.blockSize[3]) {
      goalEstX = huskylens.blockInfo[3][0].x;
      goalEstY = huskylens.blockInfo[3][0].y;
      goalEstWidth = huskylens.blockInfo[3][0].width;
    }
    float goalLeft = goalEstX - goalEstWidth / 2.0;
    float goalRight = goalEstX + goalEstWidth / 2.0;
    bool ballInGoalArea = false;
    if (goalEstWidth > 0) {
      ballInGoalArea = (ballPosX >= goalLeft && ballPosX <= goalRight);
    }
    if (Yaxis_Error <= 25) {
      getIMU();
      lastYaw = pvYaw;
      if (abs(pvYaw) <= 5 || ballInGoalArea) {  //(huskylens.updateBlocks() && huskylens.blockSize[2] && huskylens.blockSize[3])) {
        if (abs(Xaxis_Error) <= 15) {
          dribblingtothegoal();
          // wheel(0, 0, 0);
          // SetYaw();
          // getIMU();
          // beep();
          // lastYaw = pvYaw;
          // bypassYaw = 0;
          // beep();
          // Bumping();
        } else {
          TrackXaxis2();
        }
      } else if ((abs(pvYaw) > 10) && (huskylens.updateBlocks() && huskylens.blockSize[1])) {
        // if (huskylens.updateBlocks() && huskylens.blockSize[2] && huskylens.blockSize[3]) bypassYaw = 1;
        while ((huskylens.updateBlocks() && huskylens.blockSize[1]) && Yaxis_Error <= 20) {
          getIMU();
          rot_error = 155 - huskylens.blockInfo[1][0].x;
          rot_d = rot_error - rot_pError;
          rot_pError = rot_error;
          rot_w = (rot_error * 0.45) + (rot_i * 0.05) + (rot_d * 0.8);
          rot_w = constrain(rot_w, -100, 100);
          Xaxis_Error = huskylens.blockInfo[1][0].x - 160;
          Xaxis_D = Xaxis_Error - Xaxis_PvEror;
          Xaxis_PvEror = Xaxis_Error;

          int rot = (Xaxis_Error * Xaxis_Kp) + (Xaxis_D * Xaxis_Kd);
          rot = constrain(rot, -100, 100);

          Yaxis_Error = 180 - huskylens.blockInfo[1][0].y;
          Yaxis_D = Yaxis_Error - Yaxis_PvEror;
          Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
          Yaxis_spd = constrain(Yaxis_spd, -100, 100);
          Yaxis_PvEror = Yaxis_Error;

          if (Yaxis_Error > 5) {
            // if (analogRead(SenL) > Sen_Left) {
            //   looptime = millis();
            //   while (millis() - looptime <= 150) {
            //     heading(100, 35, 0);
            //   }
            //   wheel(0, 0, 0);
            // } else if (analogRead(SenR) > Sen_Right) {
            //   looptime = millis();
            //   while (millis() - looptime <= 150) {
            //     heading(100, 145, 0);
            //   }
            //   wheel(0, 0, 0);
            // } else {
              holonomic(Yaxis_spd, 90, rot_w);
            // }
          }
          if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }

          // int targetSpeed = (abs(pvYaw) > 50) ? 90 : 70;
          // int currentSpeed = 0.8 * currentSpeed + 0.2 * targetSpeed;  // smoothing speed
          // if (pvYaw < 0) {
          //   vecCurve = 0;
          // } else {
          //   vecCurve = 180;
          // }
          // holonomic(currentSpeed, vecCurve, rot_w);
          // if (Yaxis_Error > 5) heading(Yaxis_spd, 90, rot);
          // if (abs(pvYaw) <= 10) {
          //   // bypassYaw = 1;
          //   break;
          // }
        }
      }
    }
  }
}

int lastgoalpos() {
  if (huskylens.blockInfo[2][0].x < 160 || huskylens.blockInfo[3][0].x < 160) return 1;
  else if (huskylens.blockInfo[2][0].x > 160 || huskylens.blockInfo[3][0].x < 160) return 2;
  else return 0;
}

void AtanTrack4() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    lastgoalcoord = lastgoalpos();
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;
    float QuaDrantX = huskylens.blockInfo[1][0].x - 160;
    float QuaDrantY = 210 - huskylens.blockInfo[1][0].y;
    float TanTheta = QuaDrantY / QuaDrantX;
    float Setha = atan(TanTheta) * (180 / PI);
    float SethaPos;
    float DisTanT = sqrt(pow(abs(QuaDrantX), 2) + pow(abs(QuaDrantY), 2));
    if (Setha >= 0) {
      SethaPos = Setha;
    } else if (Setha < 0) {
      SethaPos = 180 + Setha;
    }
    float rot_w = 0;
    int goalID = (huskylens.blockSize[2]) ? 2 : (huskylens.blockSize[3] ? 3 : -1);

    bool ballInDangerZone = false;
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
      float ballX = huskylens.blockInfo[1][0].x;
      if (ballX < 50 || ballX > 170) {
        ballInDangerZone = true;
      }
    }

    if (goalID != -1 && !ballInDangerZone) {
      // ทำงานเฉพาะตอนเห็นโกล และ บอลไม่อยู่ในเขตอันตราย
      float goalX = huskylens.blockInfo[goalID][0].x;
      float goalW = huskylens.blockInfo[goalID][0].width;

      float goalLeft = goalX - (goalW / 2.0);
      float goalRight = goalX + (goalW / 2.0);
      float centerCamera = 160.0;
      float margin = goalW * 0.1;

      if (centerCamera < (goalLeft + margin)) {
        rot_w = -(abs(centerCamera - goalLeft) * 1.15) * 2.5;
      } else if (centerCamera > (goalRight - margin)) {
        rot_w = (abs(centerCamera - goalRight) * 1.15) * 2.5;
      } else {
        float errorRot = centerCamera - goalX;
        rot_w = errorRot * 0.5;
      }
    } else {
      // ถ้าบอลอยู่ในเขตอันตราย หรือไม่เห็นโกล ให้หยุดหมุนทันที
      rot_w = 0;
    }

    rot_w = constrain(rot_w, -40, 40);
    getIMU();
    Yaxis_Error = 180 - huskylens.blockInfo[1][0].y;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    if (abs(ballPosX - 160) > 20 && Yaxis_Error < 5) {
      Yaxis_spd = 50;
    }
    Yaxis_PvEror = Yaxis_Error;
    if (ballPosY > 150) {
      if (SethaPos > 100) {
        SethaPos = 180;
      } else if (SethaPos < 80) {
        SethaPos = 0;
      }
    }
    if (abs(ballPosX - 160) < 20 && Yaxis_Error < 5) {
      // beep();
      dribblingtothegoal();
      // bump();
    }
    // if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
    holonomic(Yaxis_spd, SethaPos, rot_w);
    // Serial.println(SethaPos);
  }
}

void dip() {
  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    if (analogRead(A1) < Sen_Front) {
      if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
      holonomic(100, 90, 0);
    } else {
      shoot();
      reload();
      long looptime = millis();
      while (millis() - looptime <= 500) {
        if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
        heading(100, 270, 0);
      }
    }
  }
}

void bump() {
  int state = 1;
  long looptime = millis();

  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    if (state == 1) {
      looptime = millis();
      while (millis() - looptime <= 1500) {
        if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);
      state = 2;
    } else if (state == 2) {
      looptime = millis();
      while (millis() - looptime <= 500) {
        if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
        heading(100, 315, 0);
      }
      wheel(0, 0, 0);
      state = 3;
    } else if (state == 3) {
      AtanTrack4();
    }
  }
}


// void Bumping() {
//   float angleToGoal = 90;
//   int previousAngle = 90;
//   bypassLR = 0;
//   count = 0;
//   bypassC = 0;
//   countC = 0;
//   float goalEstX = 160, goalEstY = 120, goalEstWidth = 50;

//   while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//     if (analogRead(A1) > Sen_Front && (huskylens.updateBlocks() && !(huskylens.blockSize[2] || huskylens.blockSize[3]))) {
//       wheel(0, 0, 0);
//       delay(100);
//       holonomic(50, 270, 0);
//       delay(100);
//       holonomic(80, 270, 0);
//       delay(200);
//       break;
//     }
//     getIMU();
//     ballPosX = huskylens.blockInfo[1][0].x;
//     ballPosY = huskylens.blockInfo[1][0].y;
//     float QuaDrantX = huskylens.blockInfo[1][0].x - 150;
//     float QuaDrantY = 180 - huskylens.blockInfo[1][0].y;
//     float TanTheta = QuaDrantY / QuaDrantX;
//     float Setha = atan(TanTheta) * (180 / PI);
//     float SethaPos;
//     if (Setha >= 0) {
//       SethaPos = Setha;
//     } else if (Setha < 0) {
//       SethaPos = 180 + Setha;
//     }

//     // ตรวจสอบโกลซ้ายหรือขวา
//     if (huskylens.blockSize[2]) {
//       goalEstX = huskylens.blockInfo[2][0].x;
//       goalEstY = huskylens.blockInfo[2][0].y;
//       goalEstWidth = huskylens.blockInfo[2][0].width;
//     } else if (huskylens.blockSize[3]) {
//       goalEstX = huskylens.blockInfo[3][0].x;
//       goalEstY = huskylens.blockInfo[3][0].y;
//       goalEstWidth = huskylens.blockInfo[3][0].width;
//     }

//     float goalLeft = goalEstX - goalEstWidth / 2.0;
//     float goalRight = goalEstX + goalEstWidth / 2.0;
//     float centerGoalX = goalEstX;
//     float diffFromCenter = ballPosX - centerGoalX;

//     float targetAngle = 90;
//     if (abs(diffFromCenter) >= 10) {
//       float maxAngleOffset = 25;
//       float maxOffset = goalEstWidth / 2.0;
//       float offsetRatio = constrain(diffFromCenter / maxOffset, -1.0, 1.0);
//       targetAngle = 90 + offsetRatio * maxAngleOffset;
//     }

//     if (ballPosY < 200) {
//       targetAngle = 90;
//     }

//     angleToGoal = previousAngle * 0.7 + targetAngle * 0.3;
//     previousAngle = angleToGoal;
//     int lastangleToGoal = angleToGoal;

//     if (ballPosY < 205) {
//       angleToGoal = 90;
//     }
//     if (analogRead(A2) > Sen_Left) angleToGoal = 70;
//     else if (analogRead(A3) > Sen_Right) angleToGoal = 120;

//     bool ballInGoalArea = false;
//     if (goalEstWidth > 0) {
//       ballInGoalArea = (ballPosX >= goalLeft && ballPosX <= goalRight);
//     }

//     if ((ballPosX > 120 && ballPosX < 180) && ballPosY > 200) heading(100, angleToGoal, 0);
//     /*else if ((ballPosX > 150 && ballPosX < 200) && !ballInGoalArea) holonomic(100, angleToGoal, 0);*/
//     else heading(100, 90, 0);

//     // else holonomic(100, angleToGoal, 0);
//     // if(ballPosY < 210) heading(100, angleToGoal, 0);

//     // if (/*goalEstY > 30 && */ballPosY > 230 && ((analogRead(A1) > Sen_Front && ballInGoalArea) /*|| (ballPosX >= goalLeft + 10 && ballPosX <= goalRight - 10)*/)) {
//     if (ballPosY > 180 && (analogRead(A1) > Sen_Front /*&& (ballPosX >= goalLeft + 10 && ballPosX <= goalRight - 10)*/) /*&& ballInGoalArea*/) {
//       shoot();
//       holonomic(0, 0, 0);
//       delay(50);
//       holonomic(100, 270, 0);
//       delay(800);
//       holonomic(0, 0, 0);
//       reload();
//       delay(50);
//       holonomic(0, 0, 0);
//       break;
//       // delay(1000);
//     }
//   }
// }

void Bumping() {

  float goalEstX = 160;
  float goalEstWidth = 50;

  int slideDir = 0;
  unsigned long pushStart = millis();

  static unsigned long looptime = 0;
  static int slidePhase = 0;

  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {

    getIMU();

    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;

    bool seeGoal = false;

    // อ่านโกล
    if (huskylens.blockSize[2]) {
      goalEstX = huskylens.blockInfo[2][0].x;
      goalEstWidth = huskylens.blockInfo[2][0].width;
      seeGoal = true;
    } else if (huskylens.blockSize[3]) {
      goalEstX = huskylens.blockInfo[3][0].x;
      goalEstWidth = huskylens.blockInfo[3][0].width;
      seeGoal = true;
    }

    // ยิง
    // if (ballPosY > 180 && analogRead(A1) > Sen_Front) {
    //   shoot();
    //   wheel(0, 0, 0);
    //   delay(50);
    //   heading(100, 270, 0);
    //   delay(500);
    //   wheel(0, 0, 0);
    // break;
    // }

    // ===== ดัน =====
    if (millis() - pushStart < 3000) {

      float diff = ballPosX - goalEstX;

      float angle = 90;
      if (abs(diff) > 10) {
        float ratio = constrain(diff / (goalEstWidth / 2.0), -1.0, 1.0);
        angle = 90 + ratio * 25;
      }
      if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
        break;
      }
      heading(100, angle, 0);
    }

    // ===== slide + rotate =====
    else {

      // เลือกทิศ
      if (slideDir == 0) {
        if (goalEstX < 160) slideDir = -1;
        else if (goalEstX > 160) slideDir = 1;
        else slideDir = (random(0, 2) == 0) ? -1 : 1;
      }

      // ---------- PHASE 0: slide ----------
      if (slidePhase == 0) {

        if (looptime == 0) looptime = millis();

        if (millis() - looptime <= 700) {
          if (slideDir == -1) {
            looptime = millis();
            while (millis() - looptime <= 700) {
              if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
                break;
              }
              heading(100, -160, 0);
            }
            wheel(0, 0, 0);
            // while (analogRead(SenF) > Sen_Front) { heading(100, 90, 45); }
            // wheel(0, 0, 0);
          } else {
            looptime = millis();
            while (millis() - looptime <= 700) {
              if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
                break;
              }
              heading(100, -20, 0);
            }
            wheel(0, 0, 0);
            // while (analogRead(SenF) > Sen_Front) { heading(100, 90, 135); }
            // wheel(0, 0, 0);
          }
        } else {
          slidePhase = 1;
          looptime = 0;
        }
      }

      // ---------- PHASE 1: หาโกล ----------
      else {

        if (!seeGoal) {
          if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
            break;
          }
          heading(100, 90, 0);
          continue;
        }

        float rotError = 160 - goalEstX;
        float rot = rotError * 0.25;

        if (abs(rot) < 12 && abs(rotError) > 15) {
          rot = (rot > 0) ? 25 : -25;
        }

        rot = constrain(rot, -50, 50);

        // 🔥 สำคัญ: วิ่ง + หมุน
        if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
          break;
        }
        // holonomic(80, 90, rot);

        if (abs(rotError) < 10) {
          slidePhase = 0;
          slideDir = 0;
          pushStart = millis();
        }
      }
    }
  }
}

void Bigbump() {
  while (1) {
    if (analogRead(limPin) < 700) reload();
    int FoundLeft = 0, FoundRight = 0, FoundCent = 0;
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {  // ball found
      // lastYaw = pvYaw;


      if (count >= 2) bypassLR = 1;
      if (countC >= 1) bypassC = 1;

      // if(count == 2) AtanTrack3();

      if (analogRead(A1) > Sen_Front && (huskylens.updateBlocks() && !(huskylens.blockSize[2] || huskylens.blockSize[3])) && bypassC == 0) {
        holonomic(0, 0, 0);
        delay(100);
        holonomic(50, 270, 0);
        delay(100);
        holonomic(80, 270, 0);
        delay(100);
        countC++;

      } else if (analogRead(A2) > Sen_Left && bypassLR == 0) {
        count++;
        holonomic(0, 20, 0);
        delay(100);
        holonomic(50, 20, 0);
        delay(100);
        holonomic(80, 20, 0);
        delay(100);
      } else if (analogRead(A3) > Sen_Right && bypassLR == 0) {
        count++;
        holonomic(0, 160, 0);
        delay(100);
        holonomic(50, 160, 0);
        delay(100);
        holonomic(80, 160, 0);
        delay(100);
      } else {
        AtanTrack3();
      }
    } else {
      bypassLR = 0;
      count = 0;
      bypassC = 0;
      countC = 0;

      // state 0 main   state 1
      int goalEstX, goalEstY, goalEstWidth;
      int nubL, nubR, vecCurveV;
      int FoundLeft = 0, FoundRight = 0, FoundCent = 0;
      int state = 0;
      int state3Count = 0;
      int lastVecCurveV = 5;
      long looptime = millis();
      while (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
        reload();

        if (analogRead(A3) > Sen_Right) {
          FoundRight = 1;
        }
        if (analogRead(A2) > Sen_Left) {
          FoundLeft = 1;
        }
        if (analogRead(A1) > Sen_Front) {
          FoundCent = 1;
        }
        if (FoundRight == 1 && FoundLeft == 1) {

          FoundLeft = 0;
          FoundRight = 0;
          // count++;
          looptime = millis();
          while (millis() - looptime <= 550) {
            getIMU();
            // if (analogRead(A2) > Sen_Left) FoundCent == 1;
            heading(80, 90, 0);
            if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
              break;
            }
          }
          // if (count == 2) {
          //   count = 0;
          //   state = 6;
          // }
          // if (count > 2) {
          //   looptime = millis();
          //   while (millis() - looptime <= 550) {
          //     getIMU();
          //     // if (analogRead(A2) > Sen_Left) FoundCent == 1;
          //     heading(100, 90, 0);
          //     if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
          //       break;
          //     }
          //   }
          // }
          // else state = 1;
        } else if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right && FoundRight == 0) {
          FoundLeft = 1;
          // getIMU();
          holonomic(100, 315, 0);
          nubL = millis();
          while (millis() - nubL <= 120) {
            if (analogRead(A3) > Sen_Right) {
              holonomic(0, 0, 0);
              FoundRight = 1;
              break;
            }
          }
          // state = 1;
          //break;
        } else if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right && FoundLeft == 0) {
          FoundRight = 1;
          // getIMU();
          holonomic(100, 225, 0);
          nubR = millis();
          while (millis() - nubR <= 150) {
            if (analogRead(A2) > Sen_Left) {
              // getIMU();
              holonomic(0, 0, 0);
              FoundLeft = 1;
              break;
            }
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              break;
            }
          }
          // state = 1;
          //break;
        } else if (analogRead(A3) > Sen_Right && analogRead(A2) > Sen_Left && analogRead(A1) > Sen_Front) {
          FoundLeft = 0;
          FoundRight = 0;
          FoundCent = 0;
          if ((huskylens.blockSize[2] && huskylens.blockSize[3]) || !(huskylens.blockSize[2] || huskylens.blockSize[3])) {
            goalEstX = 150;
          } else if (huskylens.blockSize[2]) {
            goalEstX = huskylens.blockInfo[2][0].x;
            goalEstY = huskylens.blockInfo[2][0].y;
            goalEstWidth = huskylens.blockInfo[2][0].width;
          } else if (huskylens.blockSize[3]) {
            goalEstX = huskylens.blockInfo[3][0].x;
            goalEstY = huskylens.blockInfo[3][0].y;
            goalEstWidth = huskylens.blockInfo[3][0].width;
          }
          if (!(huskylens.blockSize[2] || huskylens.blockSize[3])) {
            vecCurveV = 270;
          } else if (goalEstX >= 150 + 60) {
            vecCurveV = 315;
          } else if (goalEstX <= 150 - 60) {
            vecCurveV = 225;
          } else {
            vecCurveV = 270;
          }
          getIMU();
          heading(100, vecCurveV, 0);
        }
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
          break;
        }

        // if (abs(pvYaw) > 15) {
        //   SetYaw();
        // }


        if (state == 1) {
          vecCurveV = 90;
          getIMU();
          heading(100, 240, 0);

          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          looptime = millis();
          while (millis() - looptime <= 1100) {
            if (analogRead(A2) > Sen_Left && (millis() - looptime >= 350)) vecCurveV = 25;
            else if (analogRead(A3) > Sen_Right && (millis() - looptime >= 350)) vecCurveV = 155;
            // if (analogRead(A2) > Sen_Left) FoundCent == 1;
            getIMU();
            heading(100, vecCurveV, 0);
            if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
              break;
            }
          }
          if (vecCurveV == 90) count++;
          else count = 0;
          state = 0;
        } else if (state == 2) {  // ส่ายหน้า
          int Yor = 0;
          int targetyor = 60;
          count = 0;
          // beep();
          holonomic(0, 0, 0);
          // if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front) {
          //   holonomic(0, 0, 0);
          //   state = 0;
          //   break;
          // }

          looptime = millis();
          while (millis() - looptime <= 2000) {
            getIMU();
            if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
              for (Yor = pvYaw; pvYaw <= targetyor; Yor++) {
                if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
                  holonomic(0, 0, 0);
                  break;
                }
                getIMU();
                heading(0, 0, Yor);
              }
            }

            // holonomic(0, 0, 0);
            // while (abs(pvYaw) > 11 && huskylens.updateBlocks() && !(huskylens.blockSize[1])) heading(0, 0, 0);

            if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
              for (Yor = pvYaw; pvYaw >= -targetyor; Yor--) {
                if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
                  holonomic(0, 0, 0);
                  break;
                }
                getIMU();
                heading(0, 0, Yor);
              }
            }

            holonomic(0, 0, 0);
            // while (abs(pvYaw) > 11 && huskylens.updateBlocks() && !(huskylens.blockSize[1])) heading(0, 0, 0);
            // if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
            //   for (Yor = pvYaw; pvYaw >= 0; Yor--) {
            //     if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            //       holonomic(0, 0, 0);
            //       break;
            //     }
            //     getIMU();
            //     heading(0, 0, Yor);
            //   }
            // }

            if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front || (huskylens.updateBlocks() && huskylens.blockSize[1])) {
              holonomic(0, 0, 0);
              state = 0;
              break;
            }
          }
          while (abs(pvYaw) > 11 && huskylens.updateBlocks() && !(huskylens.blockSize[1])) heading(0, 0, 0);
          state = 3;
        } else if (state == 4) {
          count = 0;
          // beep();
          // holonomic(0, 0, 0);
          // if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front) {
          //   holonomic(0, 0, 0);
          //   state = 0;
          //   break;
          // }
          // looptime = millis();
          // while (millis() - looptime <= 2000) {
          looptime = millis();
          while (millis() - looptime <= 150) {
            getIMU();
            heading(80, 180, 0);

            if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front || (huskylens.updateBlocks() && huskylens.blockSize[1])) {
              if (analogRead(A2) > Sen_Left) holonomic(80, 0, 0), delay(200);
              else if (analogRead(A3) > Sen_Right) holonomic(80, 180, 0), delay(200);
              else if (analogRead(A1) > Sen_Front) holonomic(80, 270, 0), delay(200);
              holonomic(0, 0, 0);
              state = 0;
              break;
            }
          }

          looptime = millis();
          while (state == 3 && millis() - looptime <= 900) {
            getIMU();
            heading(80, 0, 0);

            if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front) {
              if (analogRead(A2) > Sen_Left) holonomic(80, 0, 0), delay(200);
              else if (analogRead(A3) > Sen_Right) holonomic(80, 180, 0), delay(200);
              else if (analogRead(A1) > Sen_Front) holonomic(80, 270, 0), delay(200);
              holonomic(0, 0, 0);
              state = 0;
              break;
            }
          }
          looptime = millis();
          while (state == 3 && millis() - looptime <= 100) {
            getIMU();
            heading(80, 180, 0);

            if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front) {
              if (analogRead(A2) > Sen_Left) holonomic(80, 0, 0), delay(200);
              else if (analogRead(A3) > Sen_Right) holonomic(80, 180, 0), delay(200);
              else if (analogRead(A1) > Sen_Front) holonomic(80, 270, 0), delay(200);
              holonomic(0, 0, 0);
              state = 0;
              break;
            }
          }
        } else if (state == 3) {
          count = 0;
          long startTime = millis();
          looptime = millis();

          // เลือกทิศทาง vecCurveV ตามเงื่อนไขที่ผู้ใช้ต้องการ
          if (state3Count == 0) {
            vecCurveV = (random(0, 1000) < 500) ? 5 : 175;  // สุ่มครั้งแรก
          } else if (state3Count == 1) {
            vecCurveV = (lastVecCurveV == 5) ? 175 : 5;  // กลับทิศจากครั้งที่แล้ว
            state3Count = 0;
          } else {
            vecCurveV = (random(0, 1000) < 500) ? 5 : 175;
          }

          lastVecCurveV = vecCurveV;  // บันทึกค่าล่าสุด
          state3Count++;              // เพิ่มจำนวนครั้งที่เข้า state 3
          unsigned long changeDirDelay = random(800, 1000);
          unsigned long lastSwitchTime = millis();
          bool wentLeft = (vecCurveV == 5);
          int switchCount = 0;
          // เพิ่มตัวแปรเพื่อติดตามเวลาเจอเส้นล่าสุด
          unsigned long lastLeftDetected = 0;
          unsigned long lastRightDetected = 0;


          while (millis() - looptime <= 3000 && !(huskylens.updateBlocks() && huskylens.blockSize[1])) {
            getIMU();
            heading(100, vecCurveV, 0);

            int sensorL = analogRead(A2);
            int sensorR = analogRead(A3);
            unsigned long currentTime = millis();

            bool switched = false;

            if (sensorL > Sen_Left && sensorR > Sen_Right) {  // เจอเส้นซ้าย
              if (!wentLeft && millis() - lastRightDetected <= 400) {
                changeDirDelay += 250;  // เพิ่ม delay หากเพิ่งเจอเส้นขวาใน 100 ms ที่แล้ว
              }
              lastLeftDetected = millis();

              if (!wentLeft && currentTime - lastSwitchTime <= 500) {
                switchCount++;
              }
              vecCurveV = 5;
              wentLeft = true;
              lastSwitchTime = currentTime;
            }

            else if (sensorL > Sen_Left && sensorR > Sen_Right) {  // เจอเส้นขวา
              if (wentLeft && millis() - lastLeftDetected <= 400) {
                changeDirDelay += 250;  // เพิ่ม delay หากเพิ่งเจอเส้นซ้ายใน 100 ms ที่แล้ว
              }
              lastRightDetected = millis();

              if (wentLeft && currentTime - lastSwitchTime <= 500) {
                switchCount++;
              }
              vecCurveV = 175;
              wentLeft = false;
              lastSwitchTime = currentTime;
            }


            // หากสลับฝั่งเร็วไปมาหลายครั้ง → เด้งมุมสนาม → เดินหน้าหลบ
            if (switchCount >= 2) {
              unsigned long forwardStart = millis();
              while (millis() - forwardStart <= 500) {
                getIMU();
                heading(100, 90, 0);
              }

              // reset ทุกอย่างหลังหลบมุม
              switchCount = 0;
              vecCurveV = (random(0, 1000) <= 500) ? 5 : 175;
              wentLeft = (vecCurveV == 5);
              lastSwitchTime = millis();
              changeDirDelay = random(800, 1000);
            }


            if (currentTime - lastSwitchTime >= changeDirDelay) {
              vecCurveV = (vecCurveV == 5) ? 175 : 5;
              wentLeft = !wentLeft;
              lastSwitchTime = currentTime;
              changeDirDelay = random(800, 1000);
            }
          }

          state = 0;
        } else if (state == 5) {
          looptime = millis();
          while (!(huskylens.updateBlocks() && huskylens.blockSize[1]) && millis() - looptime <= 400) {
            getIMU();
            heading(80, 270, 0);
          }
        } /*else if (state == 6) {  // ส่ายหน้าใช้ความเร็ว
          int ballFounds = false;
          looptime = millis();
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) ballFounds = true;
          while (millis() - looptime < 900) {
            holonomic(0, 90, 20);
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              ballFounds = true;
              break;
            }
          }
          getIMU();
          looptime = millis();
          while (ballFounds == false && abs(pvYaw) > 5 && millis() - looptime < 600) {
            getIMU();
            heading(0, 0, 0);
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              ballFounds = true;
              break;
            }
          }
          looptime = millis();
          while (ballFounds == false && millis() - looptime < 1700) {
            holonomic(0, 90, -20);
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              ballFounds = true;
              break;
            }
          }
          looptime = millis();
          while (ballFounds == false && millis() - looptime < 400) {
            holonomic(0, 90, -20);
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              ballFounds = true;
              break;
            }
          }
          looptime = millis();
          getIMU();
          while (ballFounds == false && abs(pvYaw) > 5 && millis() - looptime < 1000) {
            getIMU();
            heading(0, 0, 0);
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              ballFounds = true;
              break;
            }
          }
          // looptime = millis();
          // while (ballFounds == false && looptime - millis() <= 600) {
          //   getIMU();
          //   heading(100, 90, 0);
          //   if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
          //     ballFounds = true;
          //     break;
          //   }
          // }
          state = 0;
        }*/
      }
    }
  }
}
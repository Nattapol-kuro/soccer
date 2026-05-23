bool firstbump = 1;

void S_B1() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    AtanTrack1();
  } else {
    backtogoal();
  }
}

void AtanTrack1() {
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
    }
    // SetYaw();
    Yaxis_Error = 160 - huskylens.blockInfo[1][0].y;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    Yaxis_PvEror = Yaxis_Error;

    getIMU();
    heading(Yaxis_spd, SethaPos, 0);

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
        TrackXaxis2();
        if (abs(Xaxis_Error) <= 15 && firstbump == 1) {
          bump_oblique_test();
        } else{
          Dribbling();
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

          if (Yaxis_Error > 5) holonomic(Yaxis_spd, 90, rot_w);
          if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
        }
      }
    }
  }
}

void AtanTrack2() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    lastgoalcoord = lastgoalpos();
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;

    float QuaDrantX = ballPosX - 160;
    float QuaDrantY = 210 - ballPosY;
    float Setha = atan(QuaDrantY / QuaDrantX) * (180.0 / PI);
    float SethaPos = (Setha >= 0) ? Setha : 180 + Setha;

    // หาโกล + คำนวณ rot_w
    int goalID = huskylens.blockSize[2] ? 2 : (huskylens.blockSize[3] ? 3 : -1);
    float rot_w = 0;

    if (goalID != -1 && !(ballPosX < 50 || ballPosX > 170)) {
      float goalX = huskylens.blockInfo[goalID][0].x;
      float goalW = huskylens.blockInfo[goalID][0].width;
      float goalLeft = goalX - (goalW / 2.0);
      float goalRight = goalX + (goalW / 2.0);
      float center = 160.0;
      float margin = goalW * 0.1;

      if (center < goalLeft + margin) rot_w = -(abs(center - goalLeft) * 1.15) * 2.5;
      else if (center > goalRight - margin) rot_w = (abs(center - goalRight) * 1.15) * 2.5;
      else rot_w = (center - goalX) * 0.5;
    }
    rot_w = constrain(rot_w, -40, 40);

    // PD แกน Y
    getIMU();
    Yaxis_Error = 180 - ballPosY;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    Yaxis_PvEror = Yaxis_Error;

    if (abs(ballPosX - 160) > 20 && Yaxis_Error < 5) Yaxis_spd = 50;

    // แก้ SethaPos เมื่อบอลต่ำมาก
    if (ballPosY > 150) {
      if (SethaPos > 100) SethaPos = 180;
      else if (SethaPos < 80) SethaPos = 0;
    }

    // ถ้าบอลอยู่ตรงหน้าพอ → dip()
    if (abs(ballPosX - 160) < 20 && Yaxis_Error < 5) dip();

    holonomic(Yaxis_spd, SethaPos, rot_w);
  }
}

void bump_oblique() {
  int state = 1;
  long looptime = millis();

  // จำตำแหน่งโกลก่อนเริ่ม
  int goalID = huskylens.blockSize[2] ? 2 : (huskylens.blockSize[3] ? 3 : -1);
  float goalX = (goalID != -1) ? huskylens.blockInfo[goalID][0].x : 160.0;
  bool goalOnLeft = (goalX < 160);  // true = โกลอยู่ซ้าย, false = โกลอยู่ขวา

  while (1) {

    if (state == 1) {
      looptime = millis();
      while (millis() - looptime <= 1500) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);
      state = 2;

    } else if (state == 2) {
      // looptime = millis();
      // while (millis() - looptime <= 800) {
        // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
        if (goalOnLeft) {
          looptime = millis();
          while (millis() - looptime <= 300) {
            // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
            heading(100, 90, 60);  // เฉียงซ้าย
          }
          wheel(0, 0, 0);
        } else {
          looptime = millis();
          while (millis() - looptime <= 300) {
            // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
            heading(100, 90, -60);  // เฉียงขวา
          }
          wheel(0, 0, 0);
        }
      // }
      state = 3;
    } else if (state == 3) {
      // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
      // if (goalOnLeft) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
        heading(0, 0, 0);
      }
      wheel(0, 0, 0);
      // state = 4;
      // }
    } else if (state == 4) {
      if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
      AtanTrack4();
    }
    wheel(0, 0, 0);
  }
}

void bump_oblique_test() {
  int state = 1;
  long looptime = millis();

  while (firstbump) {

    int goalID = huskylens.blockSize[2] ? 2 : (huskylens.blockSize[3] ? 3 : -1);
    float goalX = (goalID != -1) ? huskylens.blockInfo[goalID][0].x : 160.0;
    bool goalOnLeft = (goalX < 160);  // true = โกลอยู่ซ้าย, false = โกลอยู่ขวา

    if (state == 1) {
      looptime = millis();
      while (millis() - looptime <= 1500) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);
      state = 2;

    } else if (state == 2) {
      // looptime = millis();
      // while (millis() - looptime <= 800) {
        // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
        if (goalOnLeft) {
          looptime = millis();
          while (millis() - looptime <= 500) {
            // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
            if(goalID == -1) break;
            huskylens.updateBlocks();
            goalID = huskylens.blockSize[2] ? 2 : (huskylens.blockSize[3] ? 3 : -1);
            heading(100, 90+20, 60);  // เฉียงซ้าย
          }
          wheel(0, 0, 0);
        } else {
          looptime = millis();
          while (millis() - looptime <= 500) {
            // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
            if(goalID == -1) break;
            huskylens.updateBlocks();
            goalID = huskylens.blockSize[2] ? 2 : (huskylens.blockSize[3] ? 3 : -1);
            heading(100, 90-20, -60);  // เฉียงขวา
          }
          wheel(0, 0, 0);
        }
      // }
      state = 3;
    } else if (state == 3) {
      // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
      // if (goalOnLeft) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        if (huskylens.updateBlocks() || huskylens.blockSize[1]) break;
        heading(60, 270, 0);
      }
      wheel(0, 0, 0);
      // break;
      state = 4;
      // }
    } else if (state == 4) {
      // if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) break;
      // AtanTrack4();
      firstbump = 0;
      backtogoal();
    }
    wheel(0, 0, 0);
  }
}

float angleToGoal = 90;
float previousAngle = 90;
int lastSeenGoal = 1;  // front

void Dribbling() {
  bypassLR = 0;
  count = 0;
  bypassC = 0;
  countC = 0;
  float goalEstX = 160, goalEstY = 120, goalEstWidth = 50;

  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    if (analogRead(SenF) > Sen_Front && (huskylens.updateBlocks() && !(huskylens.blockSize[2] || huskylens.blockSize[3]))) {
      holonomic(0, 0, 0);
      delay(100);
      holonomic(50, 270, 0);
      delay(100);
      holonomic(80, 270, 0);
      delay(200);
      break;
    }
    getIMU();
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;
    float QuaDrantX = huskylens.blockInfo[1][0].x - 150;
    float QuaDrantY = 180 - huskylens.blockInfo[1][0].y;
    float TanTheta = QuaDrantY / QuaDrantX;
    float Setha = atan(TanTheta) * (180 / PI);
    float SethaPos;
    if (Setha >= 0) {
      SethaPos = Setha;
    } else if (Setha < 0) {
      SethaPos = 180 + Setha;
    }

    // ตรวจสอบโกลซ้ายหรือขวา
    if (huskylens.blockSize[2]) {
      goalEstX = huskylens.blockInfo[2][0].x;
      goalEstY = huskylens.blockInfo[2][0].y;
      goalEstWidth = huskylens.blockInfo[2][0].width;
    } else if (huskylens.blockSize[3]) {
      goalEstX = huskylens.blockInfo[3][0].x;
      goalEstY = huskylens.blockInfo[3][0].y;
      goalEstWidth = huskylens.blockInfo[3][0].width;
    } else {
      heading(100, angleToGoal, 0);  // ใช้มุมล่าสุดไปก่อน
      continue;
    }

    float goalLeft = goalEstX - goalEstWidth / 2.0;
    float goalRight = goalEstX + goalEstWidth / 2.0;
    float centerGoalX = goalEstX;
    float diffFromCenter = ballPosX - centerGoalX;

    float targetAngle = 90;
    if (abs(diffFromCenter) >= 10) {
      float maxAngleOffset = 25;
      float maxOffset = goalEstWidth / 2.0;
      float offsetRatio = constrain(diffFromCenter / maxOffset, -1.0, 1.0);
      targetAngle = 90 + offsetRatio * maxAngleOffset;
    }

    if (ballPosY < 200) {
      targetAngle = 90;
    }

    angleToGoal = previousAngle * 0.7 + targetAngle * 0.3;
    previousAngle = angleToGoal;
    int lastangleToGoal = angleToGoal;

    if (ballPosY < 205) {
      angleToGoal = 90;
    }
    if (analogRead(SenL) > Sen_Left) angleToGoal = 70;
    else if (analogRead(SenR) > Sen_Right) angleToGoal = 120;

    bool ballInGoalArea = false;
    if (goalEstWidth > 0) {
      ballInGoalArea = (ballPosX >= goalLeft && ballPosX <= goalRight);
    }

    float error = lastangleToGoal - 90;
    // if(huskylens.updateBlocks() && !(huskylens.blockSize[2] && huskylens.blockSize[3])) {
    //   if(){
    //     error = angleToGoal - 90;
    //   }
    // }
    if (error > 180) error -= 360;
    if (error < -180) error += 360;

    int rott = 0;
    int angleWhenRott = angleToGoal;

    // หากโกลอยู่ที่ขอบซ้ายหรือขวา หนุนให้หมุนแรงขึ้น

    if (ballInGoalArea || abs(155 - ballPosX) > 15) {
      rott = 0;
      angleWhenRott = angleToGoal;
    } /* else if (goalEstX < 135 || goalEstX > 185 || !ballInGoalArea) {
      rott = constrain(error, -35, 35);  // หมุนเร็วขึ้นเมื่อโกลอยู่ขอบ
      angleWhenRott = 180 - angleToGoal;
    }*/
    else if (goalEstX >= 140 && goalEstX <= 160) {
      rott = constrain(error, -8, 8);
      angleWhenRott = 180 - angleToGoal;
    } else {
      rott = constrain(error * 2, -20, 20);
      angleWhenRott = 180 - angleToGoal;
    }

    if (abs(155 - ballPosX) < 8 && ballPosY > 205 /*&& abs(goalEstX - 161) > 10 */ && !ballInGoalArea) holonomic(100, angleWhenRott, rott);
    else if ((ballPosX > 140 && ballPosX < 160) && ballPosY > 205) holonomic(100, angleToGoal, 0);
    /*else if ((ballPosX > 150 && ballPosX < 200) && !ballInGoalArea) holonomic(100, angleToGoal, 0);*/
    else holonomic(100, 90, 0);

    // else holonomic(100, angleToGoal, 0);
    // if(ballPosY < 210) heading(100, angleToGoal, 0);

    // if (/*goalEstY > 30 && */ballPosY > 230 && ((analogRead(SensC) > SenCRef && ballInGoalArea) /*|| (ballPosX >= goalLeft + 10 && ballPosX <= goalRight - 10)*/)) {
    if ((analogRead(SenF) > Sen_Front /*&& (ballPosX >= goalLeft + 10 && ballPosX <= goalRight - 10)*/) /*&& ballInGoalArea*/) {
      shoot();
      holonomic(0, 0, 0);
      delay(50);
      holonomic(100, 270, 0);
      delay(800);
      holonomic(0, 0, 0);
      reload();
      delay(50);
      holonomic(0, 0, 0);
      break;
      // delay(1000);
    }
  }
}
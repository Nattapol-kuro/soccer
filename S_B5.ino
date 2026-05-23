void AtanTrack5() {
  if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) return;

  ballPosX = huskylens.blockInfo[1][0].x;
  ballPosY = huskylens.blockInfo[1][0].y;

  float QuaDrantX = ballPosX - 150;
  float QuaDrantY = 180 - ballPosY;
  float Setha = atan(QuaDrantY / QuaDrantX) * (180.0 / PI);
  float SethaPos = (Setha >= 0) ? Setha : 180 + Setha;

  Yaxis_Error = 180 - ballPosY;
  Yaxis_D = Yaxis_Error - Yaxis_PvEror;
  Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
  Yaxis_spd = constrain(Yaxis_spd, -100, 100);
  Yaxis_PvEror = Yaxis_Error;

  getIMU();

  if (Yaxis_Error > 25) {
    heading(Yaxis_spd, SethaPos, 0);
    return;
  }
  lastYaw = pvYaw;

  float goalEstX = 0, goalEstWidth = 0;
  if (huskylens.blockSize[2]) {
    goalEstX = huskylens.blockInfo[2][0].x;
    goalEstWidth = huskylens.blockInfo[2][0].width;
  } else if (huskylens.blockSize[3]) {
    goalEstX = huskylens.blockInfo[3][0].x;
    goalEstWidth = huskylens.blockInfo[3][0].width;
  }

  bool ballInGoalArea = (goalEstWidth > 0)
                        && (ballPosX >= goalEstX - goalEstWidth / 2.0)
                        && (ballPosX <= goalEstX + goalEstWidth / 2.0);

  if (abs(pvYaw) <= 5 || ballInGoalArea) {
    // จัดแกน X แบบ 1 tick ไม่มี loop
    Xaxis_Error = ballPosX - 160;
    Xaxis_I += Xaxis_Error;
    Xaxis_D = Xaxis_Error - Xaxis_PvEror;
    Xaxis_spd = (Xaxis_Error * Xaxis_Kp)
                + (Xaxis_I * Xaxis_Ki)
                + (Xaxis_D * Xaxis_Kd);

    if (abs(Xaxis_spd) < 10 && abs(Xaxis_Error) >= 5) {
      Xaxis_spd = (Xaxis_spd > 0) ? 15 : -15;
    } else {
      Xaxis_spd = constrain(Xaxis_spd, -80, 80);
    }
    Xaxis_PvEror = Xaxis_Error;
    heading(Xaxis_spd, 0, 0);

    if (abs(Xaxis_Error) <= 25 && abs(Yaxis_Error) < 10) {
      // beep();
      Dribble_past();
    }
  } else if (abs(pvYaw) > 10) {
    rot_error = 155 - ballPosX;
    rot_d = rot_error - rot_pError;
    rot_pError = rot_error;
    rot_w = (rot_error * 0.45) + (rot_i * 0.05) + (rot_d * 0.8);
    rot_w = constrain(rot_w, -100, 100);

    Yaxis_Error = 180 - ballPosY;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    Yaxis_PvEror = Yaxis_Error;

    if (Yaxis_Error > 5) holonomic(Yaxis_spd, 90, rot_w);
  }
}

void AtanTrack6() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    lastgoalcoord = lastgoalpos();
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;

    float QuaDrantX = ballPosX - 160;
    float QuaDrantY = 210 - ballPosY;
    float Setha = atan(QuaDrantY / QuaDrantX) * (180.0 / PI);
    float SethaPos = (Setha >= 0) ? Setha : 180 + Setha;

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

    getIMU();
    Yaxis_Error = 180 - ballPosY;
    Yaxis_D = Yaxis_Error - Yaxis_PvEror;
    Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
    Yaxis_spd = constrain(Yaxis_spd, -100, 100);
    Yaxis_PvEror = Yaxis_Error;

    if (abs(ballPosX - 160) > 20 && Yaxis_Error < 5) Yaxis_spd = 50;

    if (ballPosY > 150) {
      if (SethaPos > 100) SethaPos = 180;
      else if (SethaPos < 80) SethaPos = 0;
    }

    if (abs(ballPosX - 160) < 20 && Yaxis_Error < 5) dip();

    holonomic(Yaxis_spd, SethaPos, rot_w);
  }
}

void Dribble_past() {
  int state = 1;
  long looptime = millis();

  // จำตำแหน่งโกลก่อนเริ่ม
  int goalID = huskylens.blockSize[2] ? 2 : (huskylens.blockSize[3] ? 3 : -1);
  float goalX = (goalID != -1) ? huskylens.blockInfo[goalID][0].x : 160.0;
  bool goalOnLeft = (goalX < 160);  // true = โกลอยู่ซ้าย, false = โกลอยู่ขวา
  int move = 0;

  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    if (state == 1) {
      if (goalOnLeft) {
        looptime = millis();
        while (millis() - looptime <= 600) {
          if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
          heading(100, 135, 0);
        }
        wheel(0, 0, 0);
        state = 2;
      } else {
        looptime = millis();
        while (millis() - looptime <= 600) {
          if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
          heading(100, 45, 0);
        }
        wheel(0, 0, 0);
        state = 2;
      }
    } else if (state == 2) {
      if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) { break; }
      AtanTrack6();
    }
  }
}
void S_B5() {  //full state but not bump
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    long looptime;
    if (analogRead(A2) > Sen_Left && (millis() - startDeft > 1000)) {
      looptime = millis();
      while (millis() - looptime <= 200) {
        holonomic(100, 0, 0);
      }
      wheel(0, 0, 0);
    } else if (analogRead(A3) > Sen_Right && (millis() - startDeft > 1000)) {
      looptime = millis();
      while (millis() - looptime <= 200) {
        holonomic(100, 180, 0);
      }
      wheel(0, 0, 0);
    } else {
      AtanTrack5();
    }
  } else {
    BackTouchLine();
  }
}

void AtanTrack5() {
  if ((huskylens.updateBlocks() && huskylens.blockSize[1])) {
    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;
    float QuaDrantX = huskylens.blockInfo[1][0].x - 160;
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
          // dribblingtothegoal5();
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

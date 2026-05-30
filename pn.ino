void penaltyshoot() {
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {

    ballPosX = huskylens.blockInfo[1][0].x;
    ballPosY = huskylens.blockInfo[1][0].y;

    if (discoveState == 1) {

      float QuaDrantX = ballPosX - 165;
      float QuaDrantY = 220 - ballPosY;

      float Setha = atan2(QuaDrantY, QuaDrantX) * (180.0 / PI);

      float SethaPos;
      if (Setha >= 0) SethaPos = Setha;
      else SethaPos = 180 + Setha;

      Yaxis_Error = 230 - ballPosY;
      Yaxis_D = Yaxis_Error - Yaxis_PvEror;
      Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
      Yaxis_spd = constrain(Yaxis_spd, 0, 100);
      Yaxis_PvEror = Yaxis_Error;

      int spdLimit = map(ballPosY, 80, 230, 80, 25);
      spdLimit = constrain(spdLimit, 25, 80);
      Yaxis_spd = constrain(Yaxis_spd, -spdLimit, spdLimit);

      if (analogRead(A1) > 1200) {
        holonomic(50, 270, 0);
        delay(500);
        holonomic(0, 0, 0);
        return;
      }
      if (analogRead(A2) > 1800) {
        holonomic(50, 0, 0);
        delay(500);
        holonomic(0, 0, 0);
        return;
      }
      if (analogRead(A3) > 1800) {
        holonomic(50, 180, 0);
        delay(500);
        holonomic(0, 0, 0);
        return;
      }

      heading(Yaxis_spd, SethaPos, 0);

      if (Yaxis_Error <= 20) {
        wheel(0, 0, 0);
        getIMU();
        lastYaw = pvYaw;
        discoveState = 0;
      }
    }

    else {

      getIMU();

      if (lastYaw < 0) {
        vecCurve = 0;
        radCurve = 15;
      } else {
        vecCurve = 180;
        radCurve = -15;
      }

      holonomic(40, vecCurve, radCurve);

      if (abs(pvYaw) < alignErrorGap) {

        rot_error = sp_rot - ballPosX;

        if (abs(rot_error) < rotErrorGap) {

          wheel(0, 0, 0);
          beep();

          shoot();   // ยิง
          reload();  // เก็บก้านยิง
          wheel(0, 0, 0);

          long looptime = millis();
          while (millis() - looptime <= 150) {
            heading(50, 270, 0);
          }

          wheel(0, 0, 0);
        }
      }

      discoveState = 1;
    }
  }

  else {
    int sideRot = sp_rot - ballPosX;
    if (sideRot == 0) sideRot = 1;

    holonomic(0, 0, (sideRot / abs(sideRot)) * idleSpd);

    discoveState = 1;
  }
}

void penaltysave() {
  int FoundRight, FoundCent, FoundLeft, vecCurveV = 180;
  long nubL, nubR;
  while (1) {
    if (analogRead(A1) > Sen_Front) {
      FoundCent = 1;
    }
    if (analogRead(A2) > Sen_Left && analogRead(A3) < Sen_Right) {
      FoundLeft = 1;
      nubL = millis();
      while (millis() - nubL <= 80) {
        if (analogRead(A3) > Sen_Right) {
          heading(0, 0, 0);
          FoundRight = 1;
          break;
        } else if (analogRead(A1) > Sen_Front) {
          heading(0, 0, 0);
          FoundCent = 1;
          break;
        }
      }
    } else if (analogRead(A2) < Sen_Left && analogRead(A3) > Sen_Right) {
      FoundRight = 1;
      nubR = millis();
      while (millis() - nubR <= 80) {
        if (analogRead(A2) > Sen_Left) {
          heading(0, 0, 0);
          FoundLeft = 1;
          break;
        } else if (analogRead(A1) > Sen_Front) {
          heading(0, 0, 0);
          FoundCent = 1;
          break;
        }
      }
    }
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
    Xaxis_Error = huskylens.blockInfo[1][0].x - 150;
    Xaxis_D = Xaxis_Error - Xaxis_PvEror;
    Xaxis_spd = (Xaxis_Error * Xaxis_Kp) + (Xaxis_D * Xaxis_Kd);
    Xaxis_spd = constrain(Xaxis_spd, -100, 100);
    if (abs(Xaxis_spd) < 15 && abs(Xaxis_Error) >= 2) {
      Xaxis_spd = (Xaxis_spd > 0) ? 40 : -40;
    }
    Xaxis_PvEror = Xaxis_Error;

    rot_error = 150 - huskylens.blockInfo[1][0].x;
    rot_d = rot_error - rot_pError;
    rot_pError = rot_error;
    rot_w = (rot_error * 0.3) + (rot_i * rot_Ki) + (rot_d * rot_Kd);
    rot_w = constrain(rot_w, -80, 80);

    getIMU();
    if ((huskylens.updateBlocks() && huskylens.blockSize[1])) {
      if (FoundCent == 0 && FoundLeft == 0 && FoundRight == 0) {
        FoundCent = 0;
        FoundLeft = 0;
        FoundRight = 0;
        vecCurveV = 0;
      }
      if (FoundRight == 1 && FoundLeft == 1) {
        FoundRight = 0;
        FoundLeft = 0;
        vecCurveV = 90;
        Xaxis_spd = 60;
      } else if (FoundRight == 1 && FoundCent == 1) {
        FoundRight = 0;
        FoundLeft = 0;
        vecCurveV = 240;
      } else if (FoundCent == 1 && FoundLeft == 1) {
        FoundRight = 0;
        FoundLeft = 0;
        vecCurveV = 300;
      } else if (FoundLeft == 1) {
        FoundLeft = 0;
        vecCurveV = 0;
        Xaxis_spd = 60;
      } else if (FoundRight == 1) {
        FoundRight = 0;
        vecCurveV = 180;
        Xaxis_spd = 60;
      } else if (FoundCent == 1) {
        FoundCent = 0;
        vecCurveV = 270;
        Xaxis_spd = 60;
      }
      heading(Xaxis_spd, vecCurveV, 0);
    } else {
      /*if (FoundCent == 1 && FoundLeft == 1 && FoundRight == 1) {
        FoundCent = 0; FoundLeft = 0; FoundRight = 0;
        vecCurveV = SethaPos;
      }
      else */
      if (FoundRight == 1 && FoundLeft == 1) {
        FoundRight = 0;
        FoundLeft = 0;
        vecCurveV = 90;
      } else if (FoundRight == 1 && FoundCent == 1) {
        FoundRight = 0;
        FoundLeft = 0;
        vecCurveV = 240;
      } else if (FoundCent == 1 && FoundLeft == 1) {
        FoundRight = 0;
        FoundLeft = 0;
        vecCurveV = 300;
      } else if (FoundLeft == 1) {
        FoundLeft = 0;
        vecCurveV = 0;
      } else if (FoundRight == 1) {
        FoundRight = 0;
        vecCurveV = 180;
      } else if (FoundCent == 1) {
        FoundCent = 0;
        vecCurveV = 270;
      }
      heading(80, vecCurveV, 0);
    }
  }
}
void backtogoal() {
  int state = 0;
  bool front = 0;
  bool right = 0;
  bool left = 0;
  int vecCurve = 0;
  int count = 0;
  long looptimer;
  float targetYaw = 0;
  int stateTurn = 0;
  oled.clear();
  while (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
    if (state == 0) {
      looptimer = millis();
      while (millis() - looptimer <= 500) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
          break;
        }
        heading(100, 270, 0);
      }
      wheel(0, 0, 0);
      state = 1;
    } else if (state == 1) {
      if (ballPosX < 160) {
        vecCurve = -135;
      } else {
        vecCurve = -45;
      }
      heading(100, vecCurve, 0);
      if (analogRead(A2) > Sen_Left) {
        looptimer = millis();
        while (millis() - looptimer <= 60) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          if (analogRead(A3) > Sen_Right) {
            wheel(0, 0, 0);
            right = 1;
            break;
          } else heading(30, vecCurve, 0);
        }
        wheel(0, 0, 0);
        left = 1;
      } else if (analogRead(A3) > Sen_Right) {
        looptimer = millis();
        while (millis() - looptimer <= 60) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          if (analogRead(A2) > Sen_Left) {
            wheel(0, 0, 0);
            left = 1;
            break;
          } else heading(30, vecCurve, 0);
        }
        wheel(0, 0, 0);
        right = 1;
      }
      if (left && right) state = 5;
      else if (left || right) state = 2;

    } else if (state == 2) {
      looptimer = millis();
      while (millis() - looptimer <= 500) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
        if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
          left = 1;
          right = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (right == 1 && analogRead(A2) > Sen_Left) {
          left = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (left == 1 && analogRead(A3) > Sen_Right) {
          right = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (right == 1 && left == 0) heading(100, -135, 0);
        else if (left == 1 && right == 0) heading(100, -45, 0);
      }
      wheel(0, 0, 0);
      if (millis() - looptimer > 500) {
        // fallback ของ state 2
        if (ballPosX < 160) {
          looptimer = millis();
          while (millis() - looptimer <= 600) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
            heading(100, 0, 0);
          }
          wheel(0, 0, 0);
        } else {
          looptimer = millis();
          while (millis() - looptimer <= 600) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
            heading(100, 180, 0);
          }
          wheel(0, 0, 0);
        }
        while (1) {
          if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
            break;
          } else heading(100, 270, 0);
        }
        looptimer = millis();
        while (millis() - looptimer <= 300) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
          heading(100, 90, 0);
        }
        wheel(0, 0, 0);
        state = 5;
      }
    } else if (state == 3) {
      looptimer = millis();
      while (millis() - looptimer <= 500) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
        if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
          left = 1;
          right = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (right == 1 && analogRead(A2) > Sen_Left) {
          left = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (left == 1 && analogRead(A3) > Sen_Right) {
          right = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (right == 1 && left == 0) heading(100, -135, 0);
        else if (left == 1 && right == 0) heading(100, -45, 0);
      }
      wheel(0, 0, 0);
    } else if (state == 4) {
      if (ballPosX < 160) {
        looptimer = millis();
        while (millis() - looptimer <= 500) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
          heading(100, 90, 0);
        }
        wheel(0, 0, 0);
        looptimer = millis();
        while (millis() - looptimer <= 650) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 0, 0);
        }
        wheel(0, 0, 0);
<<<<<<< Updated upstream
        while (analogRead(A2) < Sen_Left && analogRead(A3)) {
=======
        while (analogRead(A2) < Sen_Left && analogRead(A3) < Sen_Right  ) {
>>>>>>> Stashed changes
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 270, 0);
        }
        wheel(0, 0, 0);
        looptimer = millis();
        while (millis() - looptimer <= 200) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 90, 0);
        }
        wheel(0, 0, 0);
        state = 5;
      } else {
        looptimer = millis();
        while (millis() - looptimer <= 500) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 90, 0);
        }
        wheel(0, 0, 0);
        looptimer = millis();
        while (millis() - looptimer <= 650) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 180, 0);
        }
        wheel(0, 0, 0);
        while (analogRead(A2) < Sen_Left && analogRead(A3)) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 270, 0);
        }
        wheel(0, 0, 0);
        looptimer = millis();
        while (millis() - looptimer <= 200) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
          heading(100, 90, 0);
        }
        wheel(0, 0, 0);
      }
      state = 5;
    } else if (state == 5) {
      int totalRounds = 4;
      bool seeBall = false;

      for (int round = 0; round < totalRounds && !seeBall; round++) {

        int dirs[4] = { 30, -30, -30, 30 };

        for (int i = 0; i < 4 && !seeBall; i++) {
          looptimer = millis();

          while (millis() - looptimer <= 300) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              seeBall = true;
              break;
            }
            holonomic(0, 0, dirs[i]);
          }

          wheel(0, 0, 0);
        }
      }

      if (seeBall) {
        state = 0;  // หรือ state ไล่บอล
      } else {
        state = 6;
      }
    } else if (state == 6) {
      int totalRounds = 2;  // ← กำหนดรอบ
      bool seeBall = false;

      for (int round = 0; round < totalRounds && !seeBall; round++) {

        int dirs[4] = { 180, 0, 0, 180 };

        for (int i = 0; i < 4 && !seeBall; i++) {
          looptimer = millis();

          while (millis() - looptimer <= 700) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              seeBall = true;
              break;
            }
            heading(70, dirs[i], 0);
          }

          wheel(0, 0, 0);
        }
      }
    }
  }
  if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    AtanTrack4();
  } else {
    state = 1;
  }
}
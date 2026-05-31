void dribblingtothegoal() {  //normal drib follow goal
  // sound(2654,100);
  bypassC = 0;
  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    getIMU();

    float ballX = huskylens.blockInfo[1][0].x;
    float ballY = huskylens.blockInfo[1][0].y;
    float ballW = huskylens.blockInfo[1][0].width;

    MergedGoal goal = getMergedGoalAll();

    // ======= Phase 0 & 1: เห็นโกล =======
    long looptime;
    if (analogRead(A1) > Sen_Front && (!((huskylens.updateBlocks() && huskylens.blockSize[2]) || (huskylens.updateBlocks() && huskylens.blockSize[3])))) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        heading(100, 270, 0);
      }
    } else if (analogRead(A2) > Sen_Left) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        if (analogRead(A3) > Sen_Right) {
          holonomic(100, 180, 0);
          delay((millis() - looptime) / 2);
          break;
        }
        holonomic(100, 0, 0);
      }
      wheel(0, 0, 0);
    } else if (analogRead(A3) > Sen_Right) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        if (analogRead(A2) > Sen_Left) {
          holonomic(100, 0, 0);
          delay((millis() - looptime) / 2);
          break;
        }
        holonomic(100, 180, 0);
      }
      wheel(0, 0, 0);
    }
    if (goal.found) {
      float goalLeft = goal.x - (goal.w / 2.0);
      float goalRight = goal.x + (goal.w / 2.0);

      bool ballInGoalX = (ballX > goalLeft) && (ballX < goalRight);
      bool ballInGoalY = (ballY - goal.y < 20) && (ballY > 200) && (goal.h > 60);

      // if (analogRead(A1) > Sen_Front) {
      //   bypassC++;
      // }
      // ======= Phase 1: ยิง =======
      if ((ballInGoalX && ballInGoalY) || analogRead(A1) > Sen_Front) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
          wheel(0, 0, 0);
          return;
        }

        // ===== หันหาโกล 1 tick =====
        // MergedGoal g = getMergedGoalAll();
        if (goal.found) {
          float rot = constrain(-(goal.x - 160.0) * 0.4, -40, 40);
          holonomic(0, 0, rot);
          // delay(100);
          // wheel(0, 0, 0);
        }

        // ===== ยิง =====
        // shoot();
        motor(4, 65);
        wheel(0, 0, 0);
        delay(150);
        motor(4, 0);
        delay(50);

        // wheel(0, 0, 0);
        unsigned long t = millis();
        while (millis() - t < 600) {

          if (analogRead(A2) > Sen_Left) {
            wheel(0, 0, 0);
            break;
          } else if (analogRead(A3) > Sen_Right) {
            wheel(0, 0, 0);
            break;
          }
          heading(100, 270, 0);
          // holonomic(100, 270, 0);
        }
        reload();
        // if (bypassC == 2 && (huskylens.updateBlocks() && huskylens.blockSize[1]) && ((huskylens.updateBlocks() && huskylens.blockSize[2]) || (huskylens.updateBlocks() && huskylens.blockSize[3]))) {
        //   bypassC = 0;
        //   AtanTrack_find_bypass();
        // }
        wheel(0, 0, 0);
        return;
      }

      // ======= Phase 0: จัดบอลเข้ากรอบโกล =======
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta, speed, rot;
      float goalYaw = alignErr * 0.2;  // แปลง goalError เป็นองศาที่ต้องหมุน

      if (!ballInGoalX && abs(ballError) < 80) {
        // บอลซ้าย/ขวาโกล → เลื่อนหุ่นจัดบอลเข้ากรอบ X
        theta = constrain(90.0 + (alignErr * 0.4), 45, 135);
        rot_w = constrain(-alignErr * 0.45, -70, 70);
        speed = 100;
      } else {
        // บอลใน X แล้ว แต่ยังไม่ใกล้พอ → วิ่งตรงเข้า
        theta = constrain(90.0 - (ballError * 0.25), 60, 120);
        rot_w = 0;
        // goalYaw = 0;
        speed = 100;
      }
      holonomic(speed, theta, rot_w);
      // return;
    }

    // // ======= Phase 1.5: เห็นโกลน้อยกว่า 50% → หันหน้าหาโกล =======
    else if (goal.found && goal.w < 160) {
      float goalError = goal.x - 160.0;  // + = โกลอยู่ขวา, - = โกลอยู่ซ้าย
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta = constrain(90.0 - (ballError * 0.35), 45, 135);
      float rot_w = constrain(-alignErr * 0.1, -40, 40);
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
    //  else if ((!(huskylens.updateBlocks() && huskylens.blockSize[1])) && (huskylens.blockSize[2] || huskylens.blockSize[3])) {
    //   lastYawseegoal = 0;
    //   heading(0, 0, lastYawseegoal);
    //   BackTouchLine();
    // }


    // ======= Phase 2: ไม่เห็นโกลเลย → เลี้ยงบอลไปข้างหน้า =======
    else {
      // sound(1111,100);
      float ballError = ballX - 160.0;
      float theta = constrain(90.0 - (ballError * 0.25), 50, 130);
      // float rot = constrain(angleDiff(0, pvYaw) * 1.5, -20, 20);

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






void dribblingtothegoal1() {  //normal drib follow goal
  // sound(2654,100);
  bypassC = 0;
  while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
    getIMU();

    float ballX = huskylens.blockInfo[1][0].x;
    float ballY = huskylens.blockInfo[1][0].y;
    float ballW = huskylens.blockInfo[1][0].width;

    MergedGoal goal = getMergedGoalAll();

    // ======= Phase 0 & 1: เห็นโกล =======
    long looptime;
    if (analogRead(A1) > Sen_Front && (!((huskylens.updateBlocks() && huskylens.blockSize[2]) || (huskylens.updateBlocks() && huskylens.blockSize[3])))) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        heading(100, 270, 0);
      }
    } else if (analogRead(A2) > Sen_Left) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        if (analogRead(A3) > Sen_Right) {
          holonomic(100, 180, 0);
          delay((millis() - looptime) / 2);
          break;
        }
        holonomic(100, 0, 0);
      }
      wheel(0, 0, 0);
    } else if (analogRead(A3) > Sen_Right) {
      looptime = millis();
      while (millis() - looptime <= 300) {
        if (analogRead(A2) > Sen_Left) {
          holonomic(100, 0, 0);
          delay((millis() - looptime) / 2);
          break;
        }
        holonomic(100, 180, 0);
      }
      wheel(0, 0, 0);
    }
    if (goal.found) {
      float goalLeft = goal.x - (goal.w / 2.0);
      float goalRight = goal.x + (goal.w / 2.0);

      bool ballInGoalX = (ballX > goalLeft) && (ballX < goalRight);
      bool ballInGoalY = /*(ballY - goal.y < 100) &&*/ (ballY > 215) /*&& (goal.h > 60)*/;

      // if (analogRead(A1) > Sen_Front) {
      //   bypassC++;
      // }
      // ======= Phase 1: ยิง =======
      if ((ballInGoalX && ballInGoalY) || analogRead(A1) > Sen_Front) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
          wheel(0, 0, 0);
          return;
        }

        // ===== หันหาโกล 1 tick =====
        // MergedGoal g = getMergedGoalAll();
        if (goal.found) {
          float rot = constrain(-(goal.x - 160.0) * 0.4, -40, 40);
          holonomic(0, 0, rot);
          // delay(100);
          // wheel(0, 0, 0);
        }

        // ===== ยิง =====
        // shoot();
        motor(4, 65);
        wheel(0, 0, 0);
        delay(150);
        motor(4, 0);
        delay(50);
        reload();
        // wheel(0, 0, 0);
        unsigned long t = millis();
        while (millis() - t < 600) {

          if (analogRead(A1) > Sen_Front) {
            wheel(0, 0, 0);
            break;
          } else if (analogRead(A2) > Sen_Left) {
            wheel(0, 0, 0);
            break;
          } else if (analogRead(A3) > Sen_Right) {
            wheel(0, 0, 0);
            break;
          } else {
            AtanTrack1();
          }
          // heading(100, 270, 0);
          // holonomic(100, 270, 0);
        }
        // if (bypassC == 2 && (huskylens.updateBlocks() && huskylens.blockSize[1]) && ((huskylens.updateBlocks() && huskylens.blockSize[2]) || (huskylens.updateBlocks() && huskylens.blockSize[3]))) {
        //   bypassC = 0;
        //   AtanTrack_find_bypass();
        // }
        wheel(0, 0, 0);
        return;
      }

      // ======= Phase 0: จัดบอลเข้ากรอบโกล =======
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta, speed, rot;
      float goalYaw = alignErr * 0.2;  // แปลง goalError เป็นองศาที่ต้องหมุน

      if (!ballInGoalX && abs(ballError) < 80) {
        // บอลซ้าย/ขวาโกล → เลื่อนหุ่นจัดบอลเข้ากรอบ X
        theta = constrain(90.0 + (alignErr * 0.4), 45, 135);
        rot_w = constrain(-alignErr * 0.45, -70, 70);
        speed = 100;
      } else {
        // บอลใน X แล้ว แต่ยังไม่ใกล้พอ → วิ่งตรงเข้า
        theta = constrain(90.0 - (ballError * 0.25), 60, 120);
        rot_w = 0;
        // goalYaw = 0;
        speed = 100;
      }
      holonomic(speed, theta, rot_w);
      // return;
    }

    // // ======= Phase 1.5: เห็นโกลน้อยกว่า 50% → หันหน้าหาโกล =======
    else if (goal.found && goal.w < 160) {
      float goalError = goal.x - 160.0;  // + = โกลอยู่ขวา, - = โกลอยู่ซ้าย
      float ballError = ballX - 160.0;
      float alignErr = goal.x - ballX;
      float theta = constrain(90.0 - (ballError * 0.35), 45, 135);
      float rot_w = constrain(-alignErr * 0.1, -40, 40);
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
    //  else if ((!(huskylens.updateBlocks() && huskylens.blockSize[1])) && (huskylens.blockSize[2] || huskylens.blockSize[3])) {
    //   lastYawseegoal = 0;
    //   heading(0, 0, lastYawseegoal);
    //   BackTouchLine();
    // }


    // ======= Phase 2: ไม่เห็นโกลเลย → เลี้ยงบอลไปข้างหน้า =======
    else {
      // sound(1111,100);
      float ballError = ballX - 160.0;
      float theta = constrain(90.0 - (ballError * 0.25), 50, 130);
      // float rot = constrain(angleDiff(0, pvYaw) * 1.5, -20, 20);

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
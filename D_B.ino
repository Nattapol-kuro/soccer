
void D_B() {
  int FoundLeft = 0, FoundRight = 0, FoundCent = 0;
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {  // ball found
      if (count >= 2) bypassLR = 1;
      if (countC >= 2) bypassC = 1;

      if (analogRead(SenF) > Sen_Front && (huskylens.updateBlocks() && !(huskylens.blockSize[2] || huskylens.blockSize[3])) && bypassC == 0) {
        holonomic(0, 0, 0);
        delay(100);
        holonomic(50, 270, 0);
        delay(100);
        holonomic(80, 270, 0);
        delay(100);
        countC++;

      } else if (analogRead(SenL) > Sen_Left && bypassLR == 0) {
        count++;
        holonomic(0, 20, 0);
        delay(100);
        holonomic(50, 20, 0);
        delay(100);
        holonomic(80, 20, 0);
        delay(100);
      } else if (analogRead(SenR) > Sen_Right && bypassLR == 0) {
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
    }else {
      bypassLR = 0;
      count = 0;
      bypassC = 0;
      countC = 0;
    backtogoal();
  }
}

// int bypass = 0;
// void dribblingtothegoal() {
//   count = 2;
//   long looptime = millis();
//   while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//     getIMU();

//     float ballX = huskylens.blockInfo[1][0].x;
//     float ballY = huskylens.blockInfo[1][0].y;
//     float ballW = huskylens.blockInfo[1][0].width;

//     MergedGoal goal = getMergedGoalAll();

//     // ======= Phase 0 & 1: เห็นโกล =======
//     if (goal.found) {
//       float goalLeft = goal.x - (goal.w / 2.0);
//       float goalRight = goal.x + (goal.w / 2.0);

//       bool ballInGoalX = (ballX > goalLeft) && (ballX < goalRight);
//       bool ballInGoalY = (ballY > goal.y) && (ballY > 220) && (goal.h > 60);

//       // ======= Phase 1: ยิง =======
//       if (ballInGoalX && ballInGoalY || analogRead(A1) > Sen_Front) {
//         if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
//           wheel(0, 0, 0);
//           return;
//         }

//         shoot();
//         wheel(0, 0, 0);
//         unsigned long t = millis();
//         while (millis() - t < 600) holonomic(100, 270, 0);
//         wheel(0, 0, 0);
//         reload();
//         return;
//       }

//       // ======= Phase 0: จัดบอลเข้ากรอบโกล =======
//       float ballError = ballX - 160.0;
//       float alignErr = goal.x - ballX;
//       float theta, speed, rot;
//       float goalYaw = alignErr * 0.2;  // แปลง goalError เป็นองศาที่ต้องหมุน

//       if (!ballInGoalX) {
//         // บอลซ้าย/ขวาโกล → เลื่อนหุ่นจัดบอลเข้ากรอบ X
//         theta = constrain(90.0 + (alignErr * 0.4), 45, 135);
//         rot_w = constrain(-alignErr * 0.2, -15, 15);
//         speed = 90;
//       } else {
//         // บอลใน X แล้ว แต่ยังไม่ใกล้พอ → วิ่งตรงเข้า
//         theta = constrain(90.0 - (ballError * 0.25), 60, 120);
//         rot_w = 0;
//         // goalYaw = 0;
//         speed = 70;
//       }
//       if (bypass < count) {
//         looptime = millis();
//         if (analogRead(SenL) > Sen_Left) {
//           looptime = millis();
//           while (millis() - looptime <= 150) {
//             bypass++;
//             holonomic(100, 35, 0);
//           }
//           wheel(0, 0, 0);
//         } else if (analogRead(SenR) > Sen_Right) {
//           looptime = millis();
//           while (millis() - looptime <= 150) {
//             bypass++;
//             holonomic(100, 145, 0);
//           }
//           wheel(0, 0, 0);
//         } else {
//           holonomic(speed, theta, rot_w);
//         }
//       }
//       // return;
//     }

//     // // ======= Phase 1.5: เห็นโกลน้อยกว่า 50% → หันหน้าหาโกล =======
//     if (goal.found && goal.w < 160) {
//       float goalError = goal.x - 160.0;  // + = โกลอยู่ขวา, - = โกลอยู่ซ้าย
//       float ballError = ballX - 160.0;
//       float alignErr = goal.x - ballX;
//       float theta = constrain(90.0 - (ballError * 0.25), 45, 135);
//       float rot_w = constrain(-alignErr * 0.1, -20, 20);
//       float speed = 80;

//       //   // Serial.print("goalError: ");
//       //   // Serial.print(goalError);
//       // Serial.print(" | rot: ");
//       // Serial.println(rot_w);
//       //   // Serial.print(" | goal.w: ");
//       //   // Serial.println(goal.w);
//       if (bypass < count) {
//         looptime = millis();
//         if (analogRead(SenL) > Sen_Left) {
//           looptime = millis();
//           while (millis() - looptime <= 150) {
//             bypass++;
//             holonomic(100, 35, 0);
//           }
//           wheel(0, 0, 0);
//         } else if (analogRead(SenR) > Sen_Right) {
//           looptime = millis();
//           while (millis() - looptime <= 150) {
//             bypass++;
//             holonomic(100, 145, 0);
//           }
//           wheel(0, 0, 0);
//         } else {
//           holonomic(speed, theta, rot_w);
//         }
//       }
//       continue;
//     }

//     // ======= Phase 2: ไม่เห็นโกลเลย → เลี้ยงบอลไปข้างหน้า =======
//     else {
//       float ballError = ballX - 160.0;
//       float theta = constrain(90.0 - (ballError * 0.25), 50, 130);
//       float rot = constrain(angleDiff(0, pvYaw) * 1.5, -20, 20);

//       float speed;
//       if (abs(ballError) < (ballW / 2.0)) {
//         speed = 100;
//       } else {
//         speed = 80;
//       }
//       if (bypass < count) {
//         looptime = millis();
//         if (analogRead(SenL) > Sen_Left) {
//           looptime = millis();
//           while (millis() - looptime <= 150) {
//             bypass++;
//             holonomic(100, 35, 0);
//           }
//           wheel(0, 0, 0);
//         } else if (analogRead(SenR) > Sen_Right) {
//           looptime = millis();
//           while (millis() - looptime <= 150) {
//             bypass++;
//             holonomic(100, 145, 0);
//           }
//           wheel(0, 0, 0);
//         } else {
//           heading(speed, theta, 0);
//         }
//       }
//     }
//   }
//   wheel(0, 0, 0);
// }

// int bypass = 0;

// void dribblingtothegoal() {
//   count = 2;
//   long looptime = millis();

//   while (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//     getIMU();

//     float ballX = huskylens.blockInfo[1][0].x;
//     float ballY = huskylens.blockInfo[1][0].y;
//     float ballW = huskylens.blockInfo[1][0].width;

//     MergedGoal goal = getMergedGoalAll();
//     bool nearGoal = goal.found && goal.w > 80;  // ✅ อยู่ใกล้โกลแล้ว

//     // ======= เห็นโกล =======
//     if (goal.found) {
//       bypass = 0;  // ✅ เห็นโกล → reset bypass ทันที

//       float goalLeft = goal.x - (goal.w / 2.0);
//       float goalRight = goal.x + (goal.w / 2.0);
//       bool ballInGoalX = (ballX > goalLeft) && (ballX < goalRight);
//       bool ballInGoalY = (ballY > goal.y) && (ballY > 220) && (goal.h > 60);
//       Yaxis_Error = 160 - huskylens.blockInfo[1][0].y;

//       // Phase 1: ยิง
//       if (((ballInGoalX && ballInGoalY) || analogRead(A1) > Sen_Front) && (Yaxis_Error <= 15)) {
//         if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
//           wheel(0, 0, 0);
//           return;
//         }
//         shoot();
//         wheel(0, 0, 0);
//         unsigned long t = millis();
//         while (millis() - t < 600) holonomic(100, 270, 0);
//         wheel(0, 0, 0);
//         reload();
//         return;
//       }

//       // Phase 0: จัดบอล / Phase 1.5: โกลแคบ
//       float ballError = ballX - 160.0;
//       float alignErr = goal.x - ballX;
//       float theta, speed;

//       if (goal.w < 160) {
//         // โกลแคบ
//         theta = constrain(90.0 - (ballError * 0.25), 45, 135);
//         rot_w = constrain(-alignErr * 0.1, -20, 20);
//         speed = 80;
//       } else if (!ballInGoalX) {
//         // โกลกว้าง บอลยังไม่อยู่ในกรอบ
//         theta = constrain(90.0 + (alignErr * 0.4), 45, 135);
//         rot_w = constrain(-alignErr * 0.2, -15, 15);
//         speed = 90;
//       } else {
//         // โกลกว้าง บอลอยู่ในกรอบแล้ว
//         theta = constrain(90.0 - (ballError * 0.25), 60, 120);
//         rot_w = 0;
//         speed = 70;
//       }

//       // ✅ เห็นโกล → เช็กเส้นทุกรอบ ไม่มี bypass
//       if (analogRead(SenL) > Sen_Left) {
//         looptime = millis();
//         while (millis() - looptime <= 150) holonomic(100, 35, 0);
//         wheel(0, 0, 0);
//       } else if (analogRead(SenR) > Sen_Right) {
//         looptime = millis();
//         while (millis() - looptime <= 150) holonomic(100, 145, 0);
//         wheel(0, 0, 0);
//       } else {
//         holonomic(speed, theta, rot_w);
//       }
//     }

//     // ======= ไม่เห็นโกล → วิ่งหาลูก =======
//     else {
//       float ballError = ballX - 160.0;
//       float theta = constrain(90.0 - (ballError * 0.25), 50, 130);
//       float speed = (abs(ballError) < (ballW / 2.0)) ? 100 : 80;

//       // ✅ nearGoal → sensor หลังเจอเส้นกรอบโกล → ไม่ต้องหนี
//       if (!nearGoal && analogRead(SenL) > Sen_Left) {
//         if (bypass < count) {
//           bypass++;
//           looptime = millis();
//           while (millis() - looptime <= 150) holonomic(100, 35, 0);
//           wheel(0, 0, 0);
//         }
//       } else if (!nearGoal && analogRead(SenR) > Sen_Right) {
//         if (bypass < count) {
//           bypass++;
//           looptime = millis();
//           while (millis() - looptime <= 150) holonomic(100, 145, 0);
//           wheel(0, 0, 0);
//         }
//       } else {
//         bypass = 0;  // ✅ ไม่เจอเส้น → reset
//         heading(speed, theta, 0);
//       }
//     }
//   }

//   bypass = 0;
//   wheel(0, 0, 0);
// }

void dribblingtothegoal() {
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
      float speed = 80;

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

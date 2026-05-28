void dribblingtothegoal() {  //normal drib follow goal
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
      bool ballInGoalY = (ballY > goal.y) && (ballY > 180) && (goal.h > 60);

      // ======= Phase 1: ยิง =======
      if (ballInGoalX && ballInGoalY || analogRead(A1) > Sen_Front) {
        if (!huskylens.updateBlocks() || !huskylens.blockSize[1]) {
          wheel(0, 0, 0);
          return;
        }

        // ===== หันหาโกล 1 tick =====
        // MergedGoal g = getMergedGoalAll();
        // if (g.found) {
        //   float rot = constrain(-(g.x - 160.0) * 0.4, -40, 40);
        //   holonomic(0, 0, rot);
        //   // delay(100);
        //   // wheel(0, 0, 0);
        // }

        // ===== ยิง =====
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

    // ======= Phase 2: ไม่เห็นโกลเลย → เลี้ยงบอลไปข้างหน้า =======
    else {
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

// void TouchLine() {
//   while (1) {
//     if (analogRead(limPin) < 700) reload();
//     int FoundLeft = 0, FoundRight = 0, FoundCent = 0;
//     if (huskylens.updateBlocks() && huskylens.blockSize[1]) {  // ball found
//       // lastYaw = pvYaw;


//       if (count >= 2) bypassLR = 1;
//       if (countC >= 2) bypassC = 1;

//       if (analogRead(SensC) > Sen_Front && (huskylens.updateBlocks() && !(huskylens.blockSize[2] || huskylens.blockSize[3])) && bypassC == 0) {
//         holonomic(0, 0, 0);
//         delay(100);
//         holonomic(50, 270, 0);
//         delay(100);
//         holonomic(80, 270, 0);
//         delay(100);
//         countC++;

//       } else if (analogRead(SensL) > SenLRef && bypassLR == 0) {
//         count++;
//         holonomic(0, 20, 0);
//         delay(100);
//         holonomic(50, 20, 0);
//         delay(100);
//         holonomic(80, 20, 0);
//         delay(100);
//       } else if (analogRead(SensR) > SenRRef && bypassLR == 0) {
//         count++;
//         holonomic(0, 160, 0);
//         delay(100);
//         holonomic(50, 160, 0);
//         delay(100);
//         holonomic(80, 160, 0);
//         delay(100);
//       } else {
//         AtanTrack2();
//       }
//     } else {
//       bypassLR = 0;
//       count = 0;
//       bypassC = 0;
//       countC = 0;

//       // state 0 main   state 1
//       int goalEstX, goalEstY, goalEstWidth;
//       int nubL, nubR, vecCurveV;
//       int FoundLeft = 0, FoundRight = 0, FoundCent = 0;
//       int state = 0;
//       int state3Count = 0;
//       int lastVecCurveV = 5;
//       while (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//         reload();

//         if (analogRead(SensR) > SenRRef) {
//           FoundRight = 1;
//         }
//         if (analogRead(SensL) > SenLRef) {
//           FoundLeft = 1;
//         }
//         if (analogRead(SensC) > Sen_Front) {
//           FoundCent = 1;
//         }
//         if (FoundRight == 1 && FoundLeft == 1) {

//           FoundLeft = 0;
//           FoundRight = 0;
//           count++;
//           loopTimer = millis();
//           while (millis() - loopTimer <= 550) {
//             getIMU();
//             // if (analogRead(SensL) > SenLRef) FoundCent == 1;
//             heading(100, 90, 0);
//             if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
//               break;
//             }
//           }
//           if (count == 1) {
//             count = 0;
//             state = 6;
//           }
//           // else state = 1;
//         } else if (analogRead(SensL) > SenLRef && analogRead(SensR) < SenRRef && FoundRight == 0) {
//           FoundLeft = 1;
//           // getIMU();
//           holonomic(100, 315, 0);
//           nubL = millis();
//           while (millis() - nubL <= 120) {
//             if (analogRead(SensR) > SenRRef) {
//               holonomic(0, 0, 0);
//               FoundRight = 1;
//               break;
//             }
//           }
//           // state = 1;
//           //break;
//         } else if (analogRead(SensL) < SenLRef && analogRead(SensR) > SenRRef && FoundLeft == 0) {
//           FoundRight = 1;
//           // getIMU();
//           holonomic(100, 225, 0);
//           nubR = millis();
//           while (millis() - nubR <= 150) {
//             if (analogRead(SensL) > SenLRef) {
//               // getIMU();
//               holonomic(0, 0, 0);
//               FoundLeft = 1;
//               break;
//             }
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//           }
//           // state = 1;
//           //break;
//         } else if (analogRead(SensR) < SenRRef && analogRead(SensL) < SenLRef && analogRead(SensC) < Sen_Front) {
//           FoundLeft = 0;
//           FoundRight = 0;
//           FoundCent = 0;
//           if ((huskylens.blockSize[2] && huskylens.blockSize[3]) || !(huskylens.blockSize[2] || huskylens.blockSize[3])) {
//             goalEstX = 150;
//           } else if (huskylens.blockSize[2]) {
//             goalEstX = huskylens.blockInfo[2][0].x;
//             goalEstY = huskylens.blockInfo[2][0].y;
//             goalEstWidth = huskylens.blockInfo[2][0].width;
//           } else if (huskylens.blockSize[3]) {
//             goalEstX = huskylens.blockInfo[3][0].x;
//             goalEstY = huskylens.blockInfo[3][0].y;
//             goalEstWidth = huskylens.blockInfo[3][0].width;
//           }
//           if (!(huskylens.blockSize[2] || huskylens.blockSize[3])) {
//             vecCurveV = 270;
//           } else if (goalEstX >= 150 + 60) {
//             vecCurveV = 315;
//           } else if (goalEstX <= 150 - 60) {
//             vecCurveV = 225;
//           } else {
//             vecCurveV = 270;
//           }
//           getIMU();
//           heading(100, vecCurveV, 0);
//         }
//         if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//           break;
//         }

//         if (abs(pvYaw) > 15) {
//           SetYaw();
//         }


//         if (state == 1) {
//           vecCurveV = 90;
//           getIMU();
//           heading(100, 240, 0);

//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           loopTimer = millis();
//           while (millis() - loopTimer <= 1100) {
//             if (analogRead(SensL) > SenLRef && (millis() - loopTimer >= 350)) vecCurveV = 25;
//             else if (analogRead(SensR) > SenRRef && (millis() - loopTimer >= 350)) vecCurveV = 155;
//             // if (analogRead(SensL) > SenLRef) FoundCent == 1;
//             getIMU();
//             heading(100, vecCurveV, 0);
//             if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
//               break;
//             }
//           }
//           if (vecCurveV == 90) count++;
//           else count = 0;
//           state = 0;
//         } else if (state == 2) {  // ส่ายหน้า
//           int Yor = 0;
//           int targetyor = 60;
//           count = 0;
//           // beep();
//           holonomic(0, 0, 0);
//           // if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front) {
//           //   holonomic(0, 0, 0);
//           //   state = 0;
//           //   break;
//           // }

//           loopTimer = millis();
//           while (millis() - loopTimer <= 2000) {
//             getIMU();
//             if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//               for (Yor = pvYaw; pvYaw <= targetyor; Yor++) {
//                 if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                   holonomic(0, 0, 0);
//                   break;
//                 }
//                 getIMU();
//                 heading(0, 0, Yor);
//               }
//             }

//             // holonomic(0, 0, 0);
//             // while (abs(pvYaw) > 11 && huskylens.updateBlocks() && !(huskylens.blockSize[1])) heading(0, 0, 0);

//             if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//               for (Yor = pvYaw; pvYaw >= -targetyor; Yor--) {
//                 if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                   holonomic(0, 0, 0);
//                   break;
//                 }
//                 getIMU();
//                 heading(0, 0, Yor);
//               }
//             }

//             holonomic(0, 0, 0);
//             // while (abs(pvYaw) > 11 && huskylens.updateBlocks() && !(huskylens.blockSize[1])) heading(0, 0, 0);
//             // if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//             //   for (Yor = pvYaw; pvYaw >= 0; Yor--) {
//             //     if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             //       holonomic(0, 0, 0);
//             //       break;
//             //     }
//             //     getIMU();
//             //     heading(0, 0, Yor);
//             //   }
//             // }

//             if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front || (huskylens.updateBlocks() && huskylens.blockSize[1])) {
//               holonomic(0, 0, 0);
//               state = 0;
//               break;
//             }
//           }
//           while (abs(pvYaw) > 11 && huskylens.updateBlocks() && !(huskylens.blockSize[1])) heading(0, 0, 0);
//           state = 3;
//         } else if (state == 4) {
//           count = 0;
//           // beep();
//           // holonomic(0, 0, 0);
//           // if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front) {
//           //   holonomic(0, 0, 0);
//           //   state = 0;
//           //   break;
//           // }
//           // loopTimer = millis();
//           // while (millis() - loopTimer <= 2000) {
//           loopTimer = millis();
//           while (millis() - loopTimer <= 150) {
//             getIMU();
//             heading(80, 180, 0);

//             if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front || (huskylens.updateBlocks() && huskylens.blockSize[1])) {
//               if (analogRead(SensL) > SenLRef) holonomic(80, 0, 0), delay(200);
//               else if (analogRead(SensR) > SenRRef) holonomic(80, 180, 0), delay(200);
//               else if (analogRead(SensC) > Sen_Front) holonomic(80, 270, 0), delay(200);
//               holonomic(0, 0, 0);
//               state = 0;
//               break;
//             }
//           }

//           loopTimer = millis();
//           while (state == 3 && millis() - loopTimer <= 900) {
//             getIMU();
//             heading(80, 0, 0);

//             if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front) {
//               if (analogRead(SensL) > SenLRef) holonomic(80, 0, 0), delay(200);
//               else if (analogRead(SensR) > SenRRef) holonomic(80, 180, 0), delay(200);
//               else if (analogRead(SensC) > Sen_Front) holonomic(80, 270, 0), delay(200);
//               holonomic(0, 0, 0);
//               state = 0;
//               break;
//             }
//           }
//           loopTimer = millis();
//           while (state == 3 && millis() - loopTimer <= 100) {
//             getIMU();
//             heading(80, 180, 0);

//             if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front) {
//               if (analogRead(SensL) > SenLRef) holonomic(80, 0, 0), delay(200);
//               else if (analogRead(SensR) > SenRRef) holonomic(80, 180, 0), delay(200);
//               else if (analogRead(SensC) > Sen_Front) holonomic(80, 270, 0), delay(200);
//               holonomic(0, 0, 0);
//               state = 0;
//               break;
//             }
//           }
//           // loopTimer = millis();
//           // while (state == 2 && millis() - loopTimer <= 450) {
//           //   getIMU();
//           //   heading(80, 245, 0);

//           //   if (analogRead(SensL) > SenLRef || analogRead(SensR) > SenRRef || analogRead(SensC) > Sen_Front) {
//           //     holonomic(0, 0, 0);
//           //     state = 0;
//           //     break;
//           //   }
//           // }

//         } else if (state == 3) {
//           count = 0;
//           long startTime = millis();
//           loopTimer = millis();

//           // เลือกทิศทาง vecCurveV ตามเงื่อนไขที่ผู้ใช้ต้องการ
//           if (state3Count == 0) {
//             vecCurveV = (random(0, 1000) < 500) ? 5 : 175;  // สุ่มครั้งแรก
//           } else if (state3Count == 1) {
//             vecCurveV = (lastVecCurveV == 5) ? 175 : 5;  // กลับทิศจากครั้งที่แล้ว
//             state3Count = 0;
//           } else {
//             vecCurveV = (random(0, 1000) < 500) ? 5 : 175;
//           }

//           lastVecCurveV = vecCurveV;  // บันทึกค่าล่าสุด
//           state3Count++;              // เพิ่มจำนวนครั้งที่เข้า state 3
//           unsigned long changeDirDelay = random(800, 1000);
//           unsigned long lastSwitchTime = millis();
//           bool wentLeft = (vecCurveV == 5);
//           int switchCount = 0;
//           // เพิ่มตัวแปรเพื่อติดตามเวลาเจอเส้นล่าสุด
//           unsigned long lastLeftDetected = 0;
//           unsigned long lastRightDetected = 0;


//           while (millis() - loopTimer <= 3000 && !(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//             getIMU();
//             heading(100, vecCurveV, 0);

//             int sensorL = analogRead(SensL);
//             int sensorR = analogRead(SensR);
//             unsigned long currentTime = millis();

//             bool switched = false;

//             if (sensorL > SenLRef && sensorR < SenRRef) {  // เจอเส้นซ้าย
//               if (!wentLeft && millis() - lastRightDetected <= 400) {
//                 changeDirDelay += 250;  // เพิ่ม delay หากเพิ่งเจอเส้นขวาใน 100 ms ที่แล้ว
//               }
//               lastLeftDetected = millis();

//               if (!wentLeft && currentTime - lastSwitchTime <= 500) {
//                 switchCount++;
//               }
//               vecCurveV = 5;
//               wentLeft = true;
//               lastSwitchTime = currentTime;
//             }

//             else if (sensorL < SenLRef && sensorR > SenRRef) {  // เจอเส้นขวา
//               if (wentLeft && millis() - lastLeftDetected <= 400) {
//                 changeDirDelay += 250;  // เพิ่ม delay หากเพิ่งเจอเส้นซ้ายใน 100 ms ที่แล้ว
//               }
//               lastRightDetected = millis();

//               if (wentLeft && currentTime - lastSwitchTime <= 500) {
//                 switchCount++;
//               }
//               vecCurveV = 175;
//               wentLeft = false;
//               lastSwitchTime = currentTime;
//             }


//             // หากสลับฝั่งเร็วไปมาหลายครั้ง → เด้งมุมสนาม → เดินหน้าหลบ
//             if (switchCount >= 2) {
//               unsigned long forwardStart = millis();
//               while (millis() - forwardStart <= 500) {
//                 getIMU();
//                 heading(100, 90, 0);
//               }

//               // reset ทุกอย่างหลังหลบมุม
//               switchCount = 0;
//               vecCurveV = (random(0, 1000) <= 500) ? 5 : 175;
//               wentLeft = (vecCurveV == 5);
//               lastSwitchTime = millis();
//               changeDirDelay = random(800, 1000);
//             }


//             if (currentTime - lastSwitchTime >= changeDirDelay) {
//               vecCurveV = (vecCurveV == 5) ? 175 : 5;
//               wentLeft = !wentLeft;
//               lastSwitchTime = currentTime;
//               changeDirDelay = random(800, 1000);
//             }
//           }

//           state = 0;
//         } else if (state == 5) {
//           if (huskylens.updateBlocks() && (huskylens.blockSize[2] && huskylens.blockSize[3]) && !(huskylens.blockSize[2] || huskylens.blockSize[3])) {
//             goalEstX = 160;
//           } else if (huskylens.blockSize[2] && huskylens.blockInfo[2][0].width >= 10 && huskylens.blockInfo[2][0].height >= 2) {
//             goalEstX = huskylens.blockInfo[2][0].x;
//             goalEstY = huskylens.blockInfo[2][0].y;
//             goalEstWidth = huskylens.blockInfo[2][0].width;
//           } else if (huskylens.blockSize[3] && huskylens.blockInfo[3][0].width >= 10 && huskylens.blockInfo[3][0].height >= 2) {
//             goalEstX = huskylens.blockInfo[3][0].x;
//             goalEstY = huskylens.blockInfo[3][0].y;
//             goalEstWidth = huskylens.blockInfo[3][0].width;
//           }
//           if (goalEstX > 160 + 40) {
//             vecCurveV = 300;
//           } else if (goalEstX < 160 - 40) {
//             vecCurveV = 240;
//           } else {
//             vecCurveV = 270;
//           }
//           if (FoundRight == 1 && FoundLeft == 1) {
//             FoundLeft = 0;
//             FoundRight = 0;
//             if (count == 0) {
//               state = 1;
//               loopTimer = millis();
//               while (millis() - loopTimer <= 450) {
//                 getIMU();
//                 // if (analogRead(SensL) > SenLRef) FoundCent == 1;
//                 heading(100, 90, 0);
//                 if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
//                   break;
//                 }
//               }
//             } else state = 1;
//           } else if (analogRead(SensL) > SenLRef && analogRead(SensR) < SenRRef && FoundRight == 0) {
//             FoundLeft = 1;
//             vecCurveV = 300;
//             nubL = millis();
//             while (millis() - nubL <= 100) {
//               if (analogRead(SensR) > SenRRef) {
//                 heading(0, 0, 0);
//                 FoundRight = 1;
//               }
//             }
//             // state = 1;
//             //break;
//           } else if (analogRead(SensL) < SenLRef && analogRead(SensR) > SenRRef && FoundLeft == 0) {
//             FoundRight = 1;
//             vecCurveV = 240;
//             nubR = millis();
//             while (millis() - nubR <= 150) {
//               if (analogRead(SensL) > SenLRef) {
//                 heading(0, 0, 0);
//                 FoundLeft = 1;
//               }
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                 break;
//               }
//             }
//             // state = 1;
//             //break;
//           }

//           getIMU();
//           heading(100, vecCurveV, 0);
//         } else if (state == 6) {  // ส่ายหน้าใช้ความเร็ว
//           int ballFounds = false;
//           loopTimer = millis();
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) ballFounds = true;
//           while (millis() - loopTimer < 800) {
//             holonomic(0, 90, 23);
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               ballFounds = true;
//               break;
//             }
//           }
//           // getIMU();
//           // loopTimer = millis();
//           // while (ballFounds == false && abs(pvYaw) > 5 && millis() - loopTimer < 600) {
//           //   getIMU();
//           //   heading(0, 0, 0);
//           //   if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//           //     ballFounds = true;
//           //     break;
//           //   }
//           // }
//           loopTimer = millis();
//           while (ballFounds == false && millis() - loopTimer < 1700) {
//             holonomic(0, 90, -23);
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               ballFounds = true;
//               break;
//             }
//           }
//           loopTimer = millis();
//           while (ballFounds == false && millis() - loopTimer < 750) {
//             holonomic(0, 90, 23);
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               ballFounds = true;
//               break;
//             }
//           }
//           // loopTimer = millis();
//           // getIMU();
//           // while (ballFounds == false && abs(pvYaw) > 5 && millis() - loopTimer < 1000) {
//           //   getIMU();
//           //   heading(0, 0, 0);
//           //   if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//           //     ballFounds = true;
//           //     break;
//           //   }
//           // }
//           // loopTimer = millis();
//           // while (ballFounds == false && loopTimer - millis() <= 600) {
//           //   getIMU();
//           //   heading(100, 90, 0);
//           //   if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//           //     ballFounds = true;
//           //     break;
//           //   }
//           // }
//           state = 0;
//         }
//       }
//     }
//   }
// }
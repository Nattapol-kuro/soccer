int getGoalY() {
  if (!huskylens.updateBlocks()) return 0;
  if (huskylens.blockSize[2] > 0) return huskylens.blockInfo[2][0].y;
  if (huskylens.blockSize[3] > 0) return huskylens.blockInfo[3][0].y;
  return 0;
}


void bbgg() {
  int state = 0;
  long looptime;
  float Goal_Error = 0;
  bool left = 0;
  bool right = 0;
  int vecC = 0;
  while (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
    // if (analogRead(SenL) > Sen_Left && analogRead(SenR) > Sen_Right) {
    //   state = 0;
    // } else if (analogRead(SenL) > Sen_Left) {
    //   vecC = 0;
    // } else if (analogRead(SenR) > Sen_Right) {
    //   vecC = 180;
    // }
    // looptime = millis();
    // while (millis() - looptime <= 150) {
    //   if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
    //   heading(100, vecC, 0);
    // }
    wheel(0, 0, 0);
    if (state == 0) {
      huskylens.updateBlocks();
      int goalY = getGoalY();
      if (goalY < 35 && (!(huskylens.updateBlocks() && huskylens.blockSize[1]))) {
        if (huskylens.updateBlocks()) {
          int goalX = 0;
          if (getGoalCenter(goalX)) {
            Goal_Error = goalX - 160;
            state = (Goal_Error > 0) ? 5 : 6;
          }
        }
      } else {
        looptime = millis();
        while (millis() - looptime <= 800) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
          heading(100, 270, 0);
        }
        wheel(0, 0, 0);
        if (huskylens.updateBlocks()) {
          int goalX = 0;
          if (getGoalCenter(goalX)) {
            Goal_Error = goalX - 160;
            state = (Goal_Error > 0) ? 1 : 2;
          }
        }
      }
    } else if (state == 1) {
      // ซ้าย: โกลอยู่ขวากล้อง (Error+) → slide ซ้าย จนกลาง
      if (!huskylens.updateBlocks()) continue;

      float sumX = 0;
      int cnt = 0;
      if (huskylens.blockSize[2] > 0) {
        sumX += huskylens.blockInfo[2][0].x;
        cnt++;
      }
      if (huskylens.blockSize[3] > 0) {
        sumX += huskylens.blockInfo[3][0].x;
        cnt++;
      }
      if (cnt == 0) continue;

      Goal_Error = (sumX / cnt) - 185;

      if (Goal_Error > 2) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, -20, 0);  // slide ซ้าย
      } else {
        wheel(0, 0, 0);
        state = 3;  // กลางแล้ว
      }

    } else if (state == 2) {
      // ขวา: โกลอยู่ซ้ายกล้อง (Error-) → slide ขวา จนกลาง
      if (!huskylens.updateBlocks()) continue;

      float sumX = 0;
      int cnt = 0;
      if (huskylens.blockSize[2] > 0) {
        sumX += huskylens.blockInfo[2][0].x;
        cnt++;
      }
      if (huskylens.blockSize[3] > 0) {
        sumX += huskylens.blockInfo[3][0].x;
        cnt++;
      }
      if (cnt == 0) continue;

      Goal_Error = (sumX / cnt) - 150;

      if (Goal_Error < 2) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 200, 0);  // slide ขวา
      } else {
        wheel(0, 0, 0);
        state = 3;  // กลางแล้ว
      }
    } else if (state == 3) {
      bool hitLine = false;
      int vecCurve = 270;

      while (!hitLine) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 270, 0);  // ถอยตรง

        if (analogRead(SenL) > Sen_Left) {
          looptime = millis();
          while (millis() - looptime <= 100) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
            if (analogRead(A3) > Sen_Right) {
              wheel(0, 0, 0);
              right = 1;
              break;
            } else heading(70, vecCurve, 0);
          }
          wheel(0, 0, 0);
          left = 1;
          if (left && right) hitLine = true;

        } else if (analogRead(SenR) > Sen_Right) {
          looptime = millis();
          while (millis() - looptime <= 100) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
            if (analogRead(A2) > Sen_Left) {
              wheel(0, 0, 0);
              left = 1;
              break;
            } else heading(70, vecCurve, 0);
          }
          wheel(0, 0, 0);
          right = 1;
          if (left && right) hitLine = true;
        }
      }
      wheel(0, 0, 0);
      if (left == 1 || right == 1) {
        state = 4;
      }
    } else if (state == 4) {
      looptime = millis();
      while (millis() - looptime <= 900) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);
      state = 5;
    } else if (state == 5) {
      bool seeBall = false;

      for (int round = 0; !seeBall; round++) {

        int dirs[4] = { 180, 0, 0, 180 };

        for (int i = 0; i < 4 && !seeBall; i++) {
          looptime = millis();

          while (millis() - looptime <= 400) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              seeBall = true;
              break;
            }
            heading(70, dirs[i], 0);
          }
          wheel(0, 0, 0);
        }
      }
    } else if (state == 6) {
      // ซ้าย: โกลอยู่ขวากล้อง (Error+) → slide ซ้าย จนกลาง
      if (!huskylens.updateBlocks()) continue;

      float sumX = 0;
      int cnt = 0;
      if (huskylens.blockSize[2] > 0) {
        sumX += huskylens.blockInfo[2][0].x;
        cnt++;
      }
      if (huskylens.blockSize[3] > 0) {
        sumX += huskylens.blockInfo[3][0].x;
        cnt++;
      }
      if (cnt == 0) continue;

      Goal_Error = (sumX / cnt) - 185;

      if (Goal_Error > 2) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 0, 0);  // slide ซ้าย
      } else {
        wheel(0, 0, 0);
        state = 5;  // กลางแล้ว
      }
    } else if (state == 7) {
      // ขวา: โกลอยู่ซ้ายกล้อง (Error-) → slide ขวา จนกลาง
      if (!huskylens.updateBlocks()) continue;

      float sumX = 0;
      int cnt = 0;
      if (huskylens.blockSize[2] > 0) {
        sumX += huskylens.blockInfo[2][0].x;
        cnt++;
      }
      if (huskylens.blockSize[3] > 0) {
        sumX += huskylens.blockInfo[3][0].x;
        cnt++;
      }
      if (cnt == 0) continue;

      Goal_Error = (sumX / cnt) - 150;

      if (Goal_Error < 2) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 180, 5);  // slide ขวา
      } else {
        wheel(0, 0, 0);
        state = 5;
      }
    }
  }
}

// void backtogoal() {
//   int state = 0;
//   bool front = 0;
//   bool right = 0;
//   bool left = 0;
//   int vecCurve = 0;
//   int count = 0;
//   long looptimer;
//   float targetYaw = 0;
//   int stateTurn = 0;
//   oled.clear();
//   while (1) {
//     if (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//       if (state == 0) {
//         looptimer = millis();
//         while (millis() - looptimer <= 500) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 270, 0);
//         }
//         wheel(0, 0, 0);
//         state = 1;
//       } else if (state == 1) {
//         if (ballPosX < 160) {
//           vecCurve = -135;
//         } else {
//           vecCurve = -45;
//         }
//         heading(100, vecCurve, 0);
//         if (analogRead(A2) > Sen_Left) {
//           looptimer = millis();
//           while (millis() - looptimer <= 60) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             if (analogRead(A3) > Sen_Right) {
//               wheel(0, 0, 0);
//               right = 1;
//               break;
//             } else heading(30, vecCurve, 0);
//           }
//           wheel(0, 0, 0);
//           left = 1;
//         } else if (analogRead(A3) > Sen_Right) {
//           looptimer = millis();
//           while (millis() - looptimer <= 60) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             if (analogRead(A2) > Sen_Left) {
//               wheel(0, 0, 0);
//               left = 1;
//               break;
//             } else heading(30, vecCurve, 0);
//           }
//           wheel(0, 0, 0);
//           right = 1;
//         }
//         if (left && right) state = 5;
//         else if (left || right) state = 2;

//       } else if (state == 2) {
//         looptimer = millis();
//         while (millis() - looptimer <= 500) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//           if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
//             left = 1;
//             right = 1;
//             wheel(0, 0, 0);
//             state = 4;
//             break;
//           }
//           if (right == 1 && analogRead(A2) > Sen_Left) {
//             left = 1;
//             wheel(0, 0, 0);
//             state = 4;
//             break;
//           }
//           if (left == 1 && analogRead(A3) > Sen_Right) {
//             right = 1;
//             wheel(0, 0, 0);
//             state = 4;
//             break;
//           }
//           if (right == 1 && left == 0) heading(100, -135, 0);
//           else if (left == 1 && right == 0) heading(100, -45, 0);
//         }
//         wheel(0, 0, 0);
//         if (millis() - looptimer > 500) {
//           // fallback ของ state 2
//           if (ballPosX < 160) {
//             looptimer = millis();
//             while (millis() - looptimer <= 600) {
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//               heading(100, 0, 0);
//             }
//             wheel(0, 0, 0);
//           } else {
//             looptimer = millis();
//             while (millis() - looptimer <= 600) {
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//               heading(100, 180, 0);
//             }
//             wheel(0, 0, 0);
//           }
//           while (1) {
//             if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
//               break;
//             } else heading(100, 270, 0);
//           }
//           looptimer = millis();
//           while (millis() - looptimer <= 300) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//             heading(100, 90, 0);
//           }
//           wheel(0, 0, 0);
//           state = 5;
//         }
//       } else if (state == 3) {
//         looptimer = millis();
//         while (millis() - looptimer <= 500) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//           if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
//             left = 1;
//             right = 1;
//             wheel(0, 0, 0);
//             state = 4;
//             break;
//           }
//           if (right == 1 && analogRead(A2) > Sen_Left) {
//             left = 1;
//             wheel(0, 0, 0);
//             state = 4;
//             break;
//           }
//           if (left == 1 && analogRead(A3) > Sen_Right) {
//             right = 1;
//             wheel(0, 0, 0);
//             state = 4;
//             break;
//           }
//           if (right == 1 && left == 0) heading(100, -135, 0);
//           else if (left == 1 && right == 0) heading(100, -45, 0);
//         }
//         wheel(0, 0, 0);
//       } else if (state == 4) {
//         if (ballPosX < 160) {
//           looptimer = millis();
//           while (millis() - looptimer <= 500) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//             heading(100, 90, 0);
//           }
//           wheel(0, 0, 0);
//           looptimer = millis();
//           while (millis() - looptimer <= 650) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 0, 0);
//           }
//           wheel(0, 0, 0);
//           while (analogRead(A2) < Sen_Left && analogRead(A3 < Sen_Right)) {
//             while (analogRead(A2) < Sen_Left && analogRead(A3) < Sen_Right) {
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                 break;
//               }
//               heading(100, 270, 0);
//             }
//             wheel(0, 0, 0);
//             looptimer = millis();
//             while (millis() - looptimer <= 200) {
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                 break;
//               }
//               heading(100, 90, 0);
//             }
//             wheel(0, 0, 0);
//             state = 5;
//           }
//         } else {
//           looptimer = millis();
//           while (millis() - looptimer <= 500) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 90, 0);
//           }
//           wheel(0, 0, 0);
//           looptimer = millis();
//           while (millis() - looptimer <= 650) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 180, 0);
//           }
//           wheel(0, 0, 0);
//           while (analogRead(A2) < Sen_Left && analogRead(A3) < Sen_Right) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 270, 0);
//           }
//           wheel(0, 0, 0);
//           looptimer = millis();
//           while (millis() - looptimer <= 400) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 90, 0);
//           }
//           wheel(0, 0, 0);
//         }
//         state = 5;
//       } else if (state == 5) {
//         int totalRounds = 4;
//         bool seeBall = false;
//       } else {
//         looptimer = millis();
//         while (millis() - looptimer <= 500) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 90, 0);
//         }
//         wheel(0, 0, 0);
//         looptimer = millis();
//         while (millis() - looptimer <= 650) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 180, 0);
//         }
//         wheel(0, 0, 0);
//         while (analogRead(A2) < Sen_Left && analogRead(A3) < Sen_Right) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 270, 0);
//         }
//         wheel(0, 0, 0);
//         looptimer = millis();
//         while (millis() - looptimer <= 400) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 90, 0);
//         }
//         wheel(0, 0, 0);
//       }
//       state = 5;
//     } else if (state == 5) {
//       int totalRounds = 4;
//       bool seeBall = false;
//         for (int round = 0; round < totalRounds && !seeBall; round++) {

//           int dirs[4] = { 30, -30, -30, 30 };

//           for (int i = 0; i < 4 && !seeBall; i++) {
//             looptimer = millis();

//             while (millis() - looptimer <= 300) {
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                 seeBall = true;
//                 break;
//               }
//               holonomic(0, 0, dirs[i]);
//             }

//             wheel(0, 0, 0);
//           }
//         }

//         if (seeBall) {
//           state = 0;  // หรือ state ไล่บอล
//         } else {
//           state = 6;
//         }
//       } else if (state == 6) {
//         int totalRounds = 2;  // ← กำหนดรอบ
//         bool seeBall = false;

//         for (int round = 0; round < totalRounds && !seeBall; round++) {

//           int dirs[4] = { 180, 0, 0, 180 };

//           for (int i = 0; i < 4 && !seeBall; i++) {
//             looptimer = millis();

//             while (millis() - looptimer <= 700) {
//               if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//                 seeBall = true;
//                 break;
//               }
//               heading(70, dirs[i], 0);
//             }

//             wheel(0, 0, 0);
//           }
//         }
//       }
//     } else {
//       looptimer = millis();
//       while (millis() - looptimer <= 200) { AtanTrack3(); }
//       break;
//     }
//   }
// }

// void backtogoal() {
//   int state = 0;
//   bool front = 0;
//   bool right = 0;
//   bool left = 0;
//   int vecCurve = 0;
//   int count = 0;
//   long looptimer;
//   float targetYaw = 0;
//   int stateTurn = 0;
//   oled.clear();
//   while (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
//     if (state == 0) {
//       looptimer = millis();
//       while (millis() - looptimer <= 500) {
//         if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//           break;
//         }
//         heading(100, 270, 0);
//       }
//       wheel(0, 0, 0);
//       state = 1;
//     } else if (state == 1) {
//       if (ballPosX < 160) {
//         vecCurve = -135;
//       } else {
//         vecCurve = -45;
//       }
//       heading(100, vecCurve, 0);
//       if (analogRead(A2) > Sen_Left) {
//         looptimer = millis();
//         while (millis() - looptimer <= 60) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           if (analogRead(A3) > Sen_Right) {
//             wheel(0, 0, 0);
//             right = 1;
//             break;
//           } else heading(30, vecCurve, 0);
//         }
//         wheel(0, 0, 0);
//         left = 1;
//       } else if (analogRead(A3) > Sen_Right) {
//         looptimer = millis();
//         while (millis() - looptimer <= 60) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           if (analogRead(A2) > Sen_Left) {
//             wheel(0, 0, 0);
//             left = 1;
//             break;
//           } else heading(30, vecCurve, 0);
//         }
//         wheel(0, 0, 0);
//         right = 1;
//       }
//       if (left && right) state = 5;
//       else if (left || right) state = 2;

//     } else if (state == 2) {
//       looptimer = millis();
//       while (millis() - looptimer <= 500) {
//         if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//         if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
//           left = 1;
//           right = 1;
//           wheel(0, 0, 0);
//           state = 4;
//           break;
//         }
//         if (right == 1 && analogRead(A2) > Sen_Left) {
//           left = 1;
//           wheel(0, 0, 0);
//           state = 4;
//           break;
//         }
//         if (left == 1 && analogRead(A3) > Sen_Right) {
//           right = 1;
//           wheel(0, 0, 0);
//           state = 4;
//           break;
//         }
//         // if (right == 1 && left == 0) heading(100, -135, 0);
//         // else if (left == 1 && right == 0) heading(100, -45, 0);
//       }
//       wheel(0, 0, 0);
//       if (millis() - looptimer > 500) {
//         // fallback ของ state 2
//         if (ballPosX < 160) {
//           looptimer = millis();
//           while (millis() - looptimer <= 600) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//             heading(100, 0, 0);
//           }
//           wheel(0, 0, 0);
//         } else {
//           looptimer = millis();
//           while (millis() - looptimer <= 600) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//             heading(100, 180, 0);
//           }
//           wheel(0, 0, 0);
//         }
//         while (1) {
//           if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
//             break;
//           } else heading(100, 270, 0);
//         }
//         looptimer = millis();
//         while (millis() - looptimer <= 300) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//           heading(100, 90, 0);
//         }
//         wheel(0, 0, 0);
//         state = 5;
//       }
//     }/* else if (state == 3) {
//       looptimer = millis();
//       while (millis() - looptimer <= 500) {
//         if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//         if (analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right) {
//           left = 1;
//           right = 1;
//           wheel(0, 0, 0);
//           state = 4;
//           break;
//         }
//         if (right == 1 && analogRead(A2) > Sen_Left) {
//           left = 1;
//           wheel(0, 0, 0);
//           state = 4;
//           break;
//         }
//         if (left == 1 && analogRead(A3) > Sen_Right) {
//           right = 1;
//           wheel(0, 0, 0);
//           state = 4;
//           break;
//         }
//         if (right == 1 && left == 0) heading(100, -135, 0);
//         else if (left == 1 && right == 0) heading(100, -45, 0);
//       }
//       wheel(0, 0, 0);
//     }*/ else if (state == 4) {
//       if (ballPosX < 160) {
//         looptimer = millis();
//         while (millis() - looptimer <= 500) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) { break; }
//           heading(100, 90, 0);
//         }
//         wheel(0, 0, 0);
//         looptimer = millis();
//         while (millis() - looptimer <= 650) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 0, 0);
//         }
//         wheel(0, 0, 0);
//         while (analogRead(A2) < Sen_Left && analogRead(A3 < Sen_Right)) {
//           while (analogRead(A2) < Sen_Left && analogRead(A3) < Sen_Right) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 270, 0);
//           }
//           wheel(0, 0, 0);
//           looptimer = millis();
//           while (millis() - looptimer <= 400) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               break;
//             }
//             heading(100, 90, 0);
//           }
//           wheel(0, 0, 0);
//           state = 5;
//         }
//       } else {
//         looptimer = millis();
//         while (millis() - looptimer <= 500) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 90, 0);
//         }
//         wheel(0, 0, 0);
//         looptimer = millis();
//         while (millis() - looptimer <= 650) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 180, 0);
//         }
//         wheel(0, 0, 0);
//         while (analogRead(A2) < Sen_Left && analogRead(A3) < Sen_Right) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 270, 0);
//         }
//         wheel(0, 0, 0);
//         looptimer = millis();
//         while (millis() - looptimer <= 400) {
//           if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//             break;
//           }
//           heading(100, 90, 0);
//         }
//         wheel(0, 0, 0);
//       }
//       state = 5;
//     } else if (state == 5) {
//       int totalRounds = 4;
//       bool seeBall = false;

//       for (int round = 0; round < totalRounds && !seeBall; round++) {

//         int dirs[4] = { 30, -30, -30, 30 };

//         for (int i = 0; i < 4 && !seeBall; i++) {
//           looptimer = millis();

//           while (millis() - looptimer <= 300) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               seeBall = true;
//               break;
//             }
//             holonomic(0, 0, dirs[i]);
//           }

//           wheel(0, 0, 0);
//         }
//       }

//       if (seeBall) {
//         state = 0;  // หรือ state ไล่บอล
//       } else {
//         state = 6;
//       }
//     } else if (state == 6) {
//       int totalRounds = 2;  // ← กำหนดรอบ
//       bool seeBall = false;

//       for (int round = 0; round < totalRounds && !seeBall; round++) {

//         int dirs[4] = { 180, 0, 0, 180 };

//         for (int i = 0; i < 4 && !seeBall; i++) {
//           looptimer = millis();

//           while (millis() - looptimer <= 700) {
//             if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//               seeBall = true;
//               break;
//             }
//             heading(70, dirs[i], 0);
//           }

//           wheel(0, 0, 0);
//         }
//       }
//     }
//   }
//   if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
//     AtanTrack4();
//   } else {
//     state = 1;
//   }
// }


bool front = 0;
bool right = 0;
bool left = 0;

void chksen() {
  front = (analogRead(A1) > Sen_Front) ? 1 : 0;
  left = (analogRead(A2) > Sen_Left) ? 1 : 0;
  right = (analogRead(A3) > Sen_Right) ? 1 : 0;
}

void gg() {
  int state = 0;
  int vecCurve = 0;
  // int vecCurve = (ballPosX < 160) ? -135 : -45;
  int count = 0;
  long looptimer;

  oled.clear();

  while (1) {

    // ====== เห็นบอล → ออกจาก backtogoal ======
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
      looptimer = millis();
      while (millis() - looptimer <= 200) {
        AtanTrack3();
      }
      break;
    }

    // ====== STATE 0: ถอยหลัง 500ms ======
    if (state == 0) {
      looptimer = millis();
      while (millis() - looptimer <= 600) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        chksen();
        heading(100, 270, 0);
        if (left && right) {
          looptimer = millis();
          while (millis() - looptimer <= 500) {
            heading(100, 90, 0);
          }
          wheel(0, 0, 0);
          state = 2;
        }
      }

      wheel(0, 0, 0);

      if (state == 0) {
        vecCurve = (ballPosX < 160) ? -135 : -45;
        state = 1;
      }
      // ====== STATE 1: วิ่งเฉียงหาเส้น ======
    } else if (state == 1) {
      heading(100, vecCurve, 0);

      if (analogRead(A2) > Sen_Left) {
        looptimer = millis();
        while (millis() - looptimer <= 60) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
          if (analogRead(A3) > Sen_Right) {
            wheel(0, 0, 0);
            right = 1;
            break;
          }
          heading(30, vecCurve, 0);
        }
        wheel(0, 0, 0);
        left = 1;

      } else if (analogRead(A3) > Sen_Right) {
        looptimer = millis();
        while (millis() - looptimer <= 60) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
          if (analogRead(A2) > Sen_Left) {
            wheel(0, 0, 0);
            left = 1;
            break;
          }
          heading(30, vecCurve, 0);
        }
        wheel(0, 0, 0);
        right = 1;
      }
      if (left && right) {
        looptimer = millis();
        while (millis() - looptimer <= 500) {
          heading(100, 90, 0);
        }
        wheel(0, 0, 0);
        state = 2;
      } else if (right) {
        vecCurve = -135;
        heading(100, vecCurve, 0);
      } else if (left) {
        vecCurve = -45;
        heading(100, vecCurve, 0);
      }

      ///
    } else if (state == 2) {
      vecCurve = (random(0, 3) >= 2) ? 170 : 10;
      heading(100, vecCurve, 0);
      looptimer = millis();
      while (millis() - looptimer <= 400) {
        heading(100, vecCurve, 0);
        chksen();
        // if (analogRead(A2) > Sen_Left) {
        //   looptimer = millis();
        //   while (millis() - looptimer <= 40) {
        //     if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        //     if (analogRead(A3) > Sen_Right) {
        //       wheel(0, 0, 0);
        //       right = 1;
        //       break;
        //     }
        //     heading(30, vecCurve, 0);
        //   }
        //   wheel(0, 0, 0);
        //   left = 1;

        // } else if (analogRead(A3) > Sen_Right) {
        //   looptimer = millis();
        //   while (millis() - looptimer <= 40) {
        //     if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        //     if (analogRead(A2) > Sen_Left) {
        //       wheel(0, 0, 0);
        //       left = 1;
        //       break;
        //     }
        //     heading(30, vecCurve, 0);
        //   }
        //   wheel(0, 0, 0);
        //   right = 1;
        // }
        if (left && right) {
          break;
        } else if (left) {
          looptimer = millis();
          vecCurve = 10;
        } else if (right) {
          looptimer = millis();
          vecCurve = 170;
        }
      }
      state = 3;
      wheel(0, 0, 0);
      beep();
    } else if (state == 3) {
      int dirs[4] = { 30, -30, -30, 30 };
      bool seeBall = false;

      for (int round = 0; round < 4 && !seeBall; round++) {
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
      state = 0;
    }
  }
}

void backtogoal() {
  int state = 0;
  int vecCurve = 0;
  int count = 0;
  long looptimer;

  oled.clear();

  while (1) {

    // ====== เห็นบอล → ออกจาก backtogoal ======
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
      looptimer = millis();
      while (millis() - looptimer <= 200) {
        AtanTrack3();
      }
      break;
    }

    // ====== STATE 0: ถอยหลัง 500ms ======
    if (state == 0) {
      looptimer = millis();
      while (millis() - looptimer <= 500) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 270, 0);
      }
      wheel(0, 0, 0);
      state = 1;

      // ====== STATE 1: วิ่งเฉียงหาเส้น ======
    } else if (state == 1) {
      vecCurve = (ballPosX < 160) ? -135 : -45;
      heading(100, vecCurve, 0);

      if (analogRead(A2) > Sen_Left) {
        looptimer = millis();
        while (millis() - looptimer <= 60) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
          if (analogRead(A3) > Sen_Right) {
            wheel(0, 0, 0);
            right = 1;
            break;
          }
          heading(30, vecCurve, 0);
        }
        wheel(0, 0, 0);
        left = 1;

      } else if (analogRead(A3) > Sen_Right) {
        looptimer = millis();
        while (millis() - looptimer <= 60) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
          if (analogRead(A2) > Sen_Left) {
            wheel(0, 0, 0);
            left = 1;
            break;
          }
          heading(30, vecCurve, 0);
        }
        wheel(0, 0, 0);
        right = 1;
      }

      if (left && right) {
        state = 5;
      } else if (left || right) {
        state = 2;
      }

      // ====== STATE 2: วิ่งตามเส้นหาอีกข้าง ======
    } else if (state == 2) {
      looptimer = millis();
      while (millis() - looptimer <= 500) {
        chksen();
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        if (left == 1 && right == 1) {
          left = 1;
          right = 1;
          wheel(0, 0, 0);
          state = 4;
          break;
        }
        if (right == 1 && left == 0) heading(100, -135, 0);
        else if (left == 1 && right == 0) heading(100, -45, 0);
      }
      wheel(0, 0, 0);

      // fallback ถ้า timeout
      looptimer = millis();
      int fallbackDir = (ballPosX < 160) ? 0 : 180;
      while (millis() - looptimer <= 600) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, fallbackDir, 0);
      }
      wheel(0, 0, 0);

      // ถอยหาเส้นทั้งสอง
      while (!(analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right)) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 270, 0);
      }
      wheel(0, 0, 0);

      looptimer = millis();
      while (millis() - looptimer <= 300) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);
      state = 5;

      // ====== STATE 4: จัดหุ่นกลับเข้าสนาม ======
    } else if (state == 4) {
      int turnDir = (ballPosX < 160) ? 0 : 180;

      // วิ่งหน้า
      looptimer = millis();
      while (millis() - looptimer <= 500) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);

      // เลี้ยว
      looptimer = millis();
      while (millis() - looptimer <= 650) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, turnDir, 0);
      }
      wheel(0, 0, 0);

      // ถอยหาเส้นทั้งสอง
      while (!(analogRead(A2) > Sen_Left && analogRead(A3) > Sen_Right)) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 270, 0);
      }
      wheel(0, 0, 0);

      // วิ่งหน้านิด
      looptimer = millis();
      while (millis() - looptimer <= 200) {
        if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
        heading(100, 90, 0);
      }
      wheel(0, 0, 0);
      state = 5;

      // ====== STATE 5: ส่ายหน้าซ้ายขวาหาบอล ======
    } else if (state == 5) {
      int dirs[4] = { 30, -30, -30, 30 };
      bool seeBall = false;

      for (int round = 0; round < 4 && !seeBall; round++) {
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

      state = seeBall ? 0 : 6;

      // ====== STATE 6: วิ่งซ้ายขวาหาบอล ======
    } else if (state == 6) {
      int dirs[4] = { 180, 0, 0, 180 };
      bool seeBall = false;

      for (int round = 0; round < 2 && !seeBall; round++) {
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

      // ถ้ายังไม่เจอ → กลับ state 2 แล้วหาใหม่
      if (!seeBall) {
        left = 0;
        right = 0;
        state = 2;
      }
    }
  }
}

long loopTimer;


void BackTouchLine() {

  // state 0 main   state 1
  int goalEstX, goalEstY, goalEstWidth;
  int nubL, nubR, vecCurveV;
  int FoundLeft = 0, FoundRight = 0, FoundCent = 0;
  int state = 0;
  int state3Count = 0;
  int lastVecCurveV = 5;

  while (!(huskylens.updateBlocks() && huskylens.blockSize[1])) {
    // if (analogRead(limPin) < 1000) reload();
    long ImuOUT = millis;
    while (getIMU() && abs(pvYaw) < 2 && !(huskylens.updateBlocks() && huskylens.blockSize[1])) {
      if (millis() - ImuOUT > 200) {
        break;
      }
      heading(0, 0, 0);
    }

    MergedGoal goal = getMergedGoalAll();

    if ((huskylens.updateBlocks() && huskylens.blockSize[1])) {
      wheel(0, 0, 0);
      loopTimer = millis();
      while (millis() - loopTimer <= 200) {
        ballPosX = huskylens.blockInfo[1][0].x;
        ballPosY = huskylens.blockInfo[1][0].y;
        float QuaDrantX = huskylens.blockInfo[1][0].x - 160;
        float QuaDrantY = 200 - huskylens.blockInfo[1][0].y;
        float Setha = atan2(QuaDrantY, QuaDrantX) * (180.0 / PI);
        float SethaPos;
        float DisTanT = sqrt(pow(abs(QuaDrantX), 2) + pow(abs(QuaDrantY), 2));
        if (Setha >= 0) {
          SethaPos = Setha;
        } else if (Setha < 0) {
          SethaPos = 180 + Setha;
        }
        // SetYaw();
        Yaxis_Error = 180 - huskylens.blockInfo[1][0].y;
        Yaxis_D = Yaxis_Error - Yaxis_PvEror;
        Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
        Yaxis_spd = constrain(Yaxis_spd, -100, 100);
        Yaxis_PvEror = Yaxis_Error;

        holonomic(Yaxis_spd, SethaPos, 0);
      }
      break;
    }

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
      count++;
      

      if (goal.y > 70) {
        // beep();
        long looptime = millis();
        while (millis() - looptime <= 500) {
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) break;
          // else if (analogRead(A2) > Sen_Left && analogRead(A3) < Sen_Right && FoundRight == 0) {
          //   FoundLeft = 1;
          //   // getIMU();
          //   holonomic(100, 315, 0);
          //   nubL = millis();
          //   while (millis() - nubL <= 80) {
          //     if (analogRead(A3) > Sen_Right) {
          //       holonomic(0, 0, 0);
          //       FoundRight = 1;
          //       break;
          //     }
          //   }
          // } else if (analogRead(A2) < Sen_Left && analogRead(A3) > Sen_Right && FoundLeft == 0) {
          //   FoundRight = 1;
          //   // getIMU();
          //   holonomic(100, 225, 0);
          //   nubR = millis();
          //   while (millis() - nubR <= 80) {
          //     if (analogRead(A2) > Sen_Left) {
          //       // getIMU();
          //       holonomic(0, 0, 0);
          //       FoundLeft = 1;
          //       break;
          //     }
          //     if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
          //       break;
          //     }
          //   }
          // }
          heading(100, 270, 0);
        }
      }

      loopTimer = millis();
      while (millis() - loopTimer <= 550) {
        getIMU();
        // if (analogRead(A2) > Sen_Left) FoundCent == 1;
        heading(100, 90, 0);
        if ((huskylens.updateBlocks() && huskylens.blockSize[1]) || (FoundRight == 1 && FoundLeft == 1)) {
          break;
        }
      }

      FoundLeft = 0;
      FoundRight = 0;
      if (count == 1) {
        count = 0;
        state = 3;
      }
      // else state = 1;
    } else if (analogRead(A2) > Sen_Left && analogRead(A3) < Sen_Right && FoundRight == 0) {
      FoundLeft = 1;
      // getIMU();
      holonomic(100, 315, 0);
      nubL = millis();
      while (millis() - nubL <= 80) {
        if (analogRead(A3) > Sen_Right) {
          holonomic(0, 0, 0);
          FoundRight = 1;
          break;
        }
      }
      // state = 1;
      //break;
    } else if (analogRead(A2) < Sen_Left && analogRead(A3) > Sen_Right && FoundLeft == 0) {
      FoundRight = 1;
      // getIMU();
      holonomic(100, 225, 0);
      nubR = millis();
      while (millis() - nubR <= 80) {
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
    }
    // else if (analogRead(A3) < Sen_Right && analogRead(A2) < Sen_Left && analogRead(A1) < Sen_Front) {
    //   FoundLeft = 0;
    //   FoundRight = 0;
    //   FoundCent = 0;
    //   // อ่าน goal ด้วย MergedGoal
    //   MergedGoal goal = getMergedGoalAll();
    //   if (goal.found) {
    //     goalEstX = goal.x;
    //     goalEstWidth = goal.w;
    //   } else {
    //     goalEstX = 160;
    //   }
    //   if (!(huskylens.blockSize[2] || huskylens.blockSize[3])) {
    //     vecCurveV = 270;
    //   } else if (goalEstX >= 160 + 50) {
    //     vecCurveV = 315;
    //   } else if (goalEstX <= 160 - 50) {
    //     vecCurveV = 225;
    //   } else {
    //     vecCurveV = 270;
    //   }
    //   getIMU();
    //   heading(100, vecCurveV, 0);
    // }

    else if (analogRead(A3) < Sen_Right && analogRead(A2) < Sen_Left && analogRead(A1) < Sen_Front) {
      FoundLeft = 0;
      FoundRight = 0;
      FoundCent = 0;

      MergedGoal goal = getMergedGoalAll();
      if (goal.found) {
        goalEstX = goal.x;
        goalEstWidth = goal.w;
      } else {
        goalEstX = 160;
      }

      float goalError = goalEstX - 160;
      float slideSpd = 0;
      float goalX_Kp = 0.4;
      float goalX_Kd = 0.15;
      float goalX_PvError = 0;
      if (!(huskylens.blockSize[2] || huskylens.blockSize[3])) {
        // ไม่เห็นโกลเลย → ถอยตรง
        slideSpd = 0;
      } else if (abs(goalError) > 60) {
        // ไกลมาก → slide เต็ม
        slideSpd = (goalError > 0) ? 100 : -100;
      } else if (abs(goalError) > 15) {
        // ใกล้กลาง → PD
        float goalD = goalError - goalX_PvError;
        slideSpd = (goalError * goalX_Kp) + (goalD * goalX_Kd);
        slideSpd = constrain(slideSpd, -60, 60);
      } else {
        // กลางแล้ว → หยุด slide
        slideSpd = 0;
      }
      goalX_PvError = goalError;

      float backSpd = 100;
      float angle_rad = atan2(-backSpd, slideSpd) * (180.0 / PI);
      if (angle_rad < 0) angle_rad += 360;
      float totalSpd = sqrt(pow(backSpd, 2) + pow(slideSpd, 2));
      totalSpd = constrain(totalSpd, 0, 100);

      getIMU();
      heading(totalSpd, angle_rad, 0);
    }

    // else if (analogRead(A3) < Sen_Right && analogRead(A2) < Sen_Left && analogRead(A1) < Sen_Front) {
    //   FoundLeft = 0;
    //   FoundRight = 0;
    //   FoundCent = 0;

    //   MergedGoal goal = getMergedGoalAll();
    //   if (goal.found) {
    //     goalEstX = goal.x;
    //     goalEstWidth = goal.w;
    //   } else {
    //     goalEstX = 160;
    //     goalEstWidth = 0;
    //   }

    //   if (!(huskylens.blockSize[2] || huskylens.blockSize[3])) {
    //     // ไม่เห็นโกลเลย → ถอยตรง
    //     vecCurveV = 270;
    //   } else if (goalEstX >= 160 + 25) {
    //     // โกลอยู่ขวา → สไลขวาก่อน ยังไม่ถอย
    //     vecCurveV = 0;
    //   } else if (goalEstX <= 160 - 25) {
    //     // โกลอยู่ซ้าย → สไลซ้ายก่อน ยังไม่ถอย
    //     vecCurveV = 180;
    //   } else {
    //     // กลางโกลแล้ว → ถอยตรง
    //     vecCurveV = 270;
    //   }

    //   getIMU();
    //   heading(100, vecCurveV, 0);
    // }
    if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
      break;
    }

    if (state == 1) {
      vecCurveV = 90;
      getIMU();
      heading(100, 240, 0);

      if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
        break;
      }
      loopTimer = millis();
      while (millis() - loopTimer <= 1100) {
        if (analogRead(A2) > Sen_Left && (millis() - loopTimer >= 350)) vecCurveV = 25;
        else if (analogRead(A3) > Sen_Right && (millis() - loopTimer >= 350)) vecCurveV = 155;
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
      int dirs[4] = { 40, -40, -40, 40 };
      bool seeBall = false;

      for (int round = 0; round < 2 && !seeBall; round++) {
        for (int i = 0; i < 4 && !seeBall; i++) {
          loopTimer = millis();
          while (millis() - loopTimer <= 360) {
            if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
              seeBall = true;
              break;
            }
            holonomic(0, 0, dirs[i]);
          }
          wheel(0, 0, 0);
        }
      }
      wheel(0, 0, 0);
      loopTimer = millis();
      while (millis() - loopTimer <= 200) {
        ballPosX = huskylens.blockInfo[1][0].x;
        ballPosY = huskylens.blockInfo[1][0].y;
        float QuaDrantX = huskylens.blockInfo[1][0].x - 160;
        float QuaDrantY = 200 - huskylens.blockInfo[1][0].y;
        float Setha = atan2(QuaDrantY, QuaDrantX) * (180.0 / PI);
        float SethaPos;
        float DisTanT = sqrt(pow(abs(QuaDrantX), 2) + pow(abs(QuaDrantY), 2));
        if (Setha >= 0) {
          SethaPos = Setha;
        } else if (Setha < 0) {
          SethaPos = 180 + Setha;
        }
        // SetYaw();
        Yaxis_Error = 180 - huskylens.blockInfo[1][0].y;
        Yaxis_D = Yaxis_Error - Yaxis_PvEror;
        Yaxis_spd = (Yaxis_Error * Yaxis_Kp) + (Yaxis_D * Yaxis_Kd);
        Yaxis_spd = constrain(Yaxis_spd, -100, 100);
        Yaxis_PvEror = Yaxis_Error;

        holonomic(Yaxis_spd, SethaPos, 0);
      }
      state = 0;  ///   ///////////////////////////////////    ////
    } else if (state == 4) {
      count = 0;
      // beep();
      // holonomic(0, 0, 0);
      // if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front) {
      //   holonomic(0, 0, 0);
      //   state = 0;
      //   break;
      // }
      // loopTimer = millis();
      // while (millis() - loopTimer <= 2000) {
      loopTimer = millis();
      while (millis() - loopTimer <= 150) {
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

      loopTimer = millis();
      while (state == 4 && millis() - loopTimer <= 900) {
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
      loopTimer = millis();
      while (state == 4 && millis() - loopTimer <= 100) {
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
      // loopTimer = millis();
      // while (state == 2 && millis() - loopTimer <= 450) {
      //   getIMU();
      //   heading(80, 245, 0);

      //   if (analogRead(A2) > Sen_Left || analogRead(A3) > Sen_Right || analogRead(A1) > Sen_Front) {
      //     holonomic(0, 0, 0);
      //     state = 0;
      //     break;
      //   }
      // }

    } else if (state == 3) {
      count = 0;
      long startTime = millis();
      loopTimer = millis();

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


      while (millis() - loopTimer <= 3000 && !(huskylens.updateBlocks() && huskylens.blockSize[1])) {
        getIMU();
        heading(100, vecCurveV, 0);

        int sensorL = analogRead(A2);
        int sensorR = analogRead(A3);
        unsigned long currentTime = millis();

        bool switched = false;

        if (sensorL > Sen_Left && sensorR < Sen_Right) {  // เจอเส้นซ้าย
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

        else if (sensorL < Sen_Left && sensorR > Sen_Right) {  // เจอเส้นขวา
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

      state = 2;  //// /////////////// /////////////////////////// / // / ///////////////////
    } else if (state == 5) {
      // อ่าน goal ด้วย MergedGoal
      MergedGoal goal = getMergedGoalAll();
      if (goal.found) {
        goalEstX = goal.x;
        goalEstWidth = goal.w;
      } else {
        goalEstX = 160;
      }
      if (goalEstX > 160 + 40) {
        vecCurveV = 300;
      } else if (goalEstX < 160 - 40) {
        vecCurveV = 240;
      } else {
        vecCurveV = 270;
      }
      if (FoundRight == 1 && FoundLeft == 1) {
        FoundLeft = 0;
        FoundRight = 0;
        if (count == 0) {
          state = 1;
          loopTimer = millis();
          while (millis() - loopTimer <= 450) {
            getIMU();
            // if (analogRead(A2) > Sen_Left) FoundCent == 1;
            heading(100, 90, 0);
            if ((huskylens.updateBlocks() && huskylens.blockSize[1]) /*|| FoundCent == 1*/) {
              break;
            }
          }
        } else state = 1;
      } else if (analogRead(A2) > Sen_Left && analogRead(A3) < Sen_Right && FoundRight == 0) {
        FoundLeft = 1;
        vecCurveV = 300;
        nubL = millis();
        while (millis() - nubL <= 100) {
          if (analogRead(A3) > Sen_Right) {
            heading(0, 0, 0);
            FoundRight = 1;
          }
        }
        // state = 1;
        //break;
      } else if (analogRead(A2) < Sen_Left && analogRead(A3) > Sen_Right && FoundLeft == 0) {
        FoundRight = 1;
        vecCurveV = 240;
        nubR = millis();
        while (millis() - nubR <= 150) {
          if (analogRead(A2) > Sen_Left) {
            heading(0, 0, 0);
            FoundLeft = 1;
          }
          if (huskylens.updateBlocks() && huskylens.blockSize[1]) {
            break;
          }
        }
        // state = 1;
        //break;
      }

      getIMU();
      heading(100, vecCurveV, 0);
    }
  }
}
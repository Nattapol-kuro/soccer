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
// <<<<<<< HEAD
//         state = 5;
//       } else if (state == 5) {
//         int totalRounds = 4;
//         bool seeBall = false;
// =======
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
// >>>>>>> 82ec3ec37e199dd12acb82436e556f9ef2ace74a

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
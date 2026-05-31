void S3Def() {
  huskylens.updateBlocks();
  int setpyball = huskylens.blockInfo[1][0].y;
  int setpxball = huskylens.blockInfo[1][0].y;
  long loopTimer = millis();
  while (millis() - loopTimer <= 4879 /*&& huskylens.updateBlocks() && huskylens.blockSize[1]*/) {
    huskylens.updateBlocks();
    // setpyball = huskylens.blockInfo[1][0].y;
    // holonomic(0, 0, 0);
    // if ( abs (setpyball - huskylens.blockInfo[1][0].y) > 20) {
    if (huskylens.blockInfo[1][0].y > 90 && huskylens.updateBlocks()) {
      // sound(3000,100);
      TrackXaxis2();
      break;
    }
    // if(huskylens.updateBlocks() && huskylens.blockSize[1] && (abs(huskylens.blockInfo[1][0].x - setpxball) > 10)) break;
  }
  startDeft = millis();
  while (1) {
    S_B3();
  }
}

void S1Def() {
  huskylens.updateBlocks();
  int setpyball = huskylens.blockInfo[1][0].y;
  int setpxball = huskylens.blockInfo[1][0].y;
  long loopTimer = millis();
  while (millis() - loopTimer <= 4879 /*&& huskylens.updateBlocks() && huskylens.blockSize[1]*/) {
    huskylens.updateBlocks();
    // setpyball = huskylens.blockInfo[1][0].y;
    // holonomic(0, 0, 0);
    // if ( abs (setpyball - huskylens.blockInfo[1][0].y) > 20) {
    if (huskylens.blockInfo[1][0].y > 80 && huskylens.updateBlocks()) {
      // sound(3000,100);
      TrackXaxis2();
      break;
    }
    // if(huskylens.updateBlocks() && huskylens.blockSize[1] && (abs(huskylens.blockInfo[1][0].x - setpxball) > 10)) break;
  }
  startDeft = millis();
  while (1) { S_B1(); }
}
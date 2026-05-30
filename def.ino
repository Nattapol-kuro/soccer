void S3Def() {
  int setpyball = huskylens.blockInfo[1][0].y;
  int setpxball = huskylens.blockInfo[1][0].y;
  long loopTimer = millis();
  while (millis() - loopTimer <= 4879 /*&& huskylens.updateBlocks() && huskylens.blockSize[1]*/) {
    huskylens.updateBlocks();
    // holonomic(0, 0, 0);
    if ((huskylens.blockInfo[1][0].y > 65)) {
      // sound(500,1000);
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

  int setpyball = huskylens.blockInfo[1][0].y;
  int setpxball = huskylens.blockInfo[1][0].y;
  long loopTimer = millis();
  while (millis() - loopTimer <= 4879 /*&& huskylens.updateBlocks() && huskylens.blockSize[1]*/) {
    huskylens.updateBlocks();
    // holonomic(0, 0, 0);
    if ((huskylens.blockInfo[1][0].y > 65)) {
      // sound(500,1000);
      break;
    }
    // if(huskylens.updateBlocks() && huskylens.blockSize[1] && (abs(huskylens.blockInfo[1][0].x - setpxball) > 10)) break;
  }
  while (1) { S_B1(); }
}
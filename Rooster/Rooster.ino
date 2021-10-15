#include <M5Core2.h>

#define THRESHOLD 2

float posX = 0.0f;
float posY = 0.0f;
float posZ = 0.0f;

void setup() {
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.fillScreen(GREEN);
  M5.Lcd.setTextSize(2);
}

void loop() {

  if (detectMovement()) {
    M5.Lcd.fillRect(100, 100, 120, 80, RED);
  } else {
    M5.Lcd.fillRect(100, 100, 120, 80, GREEN);
  }
  delay(100);
}

bool detectMovement() {
  bool result;
  float oldX = posX;
  float oldY = posY;
  float oldZ = posZ;
  float newX, newY, newZ;
  M5.IMU.getAhrsData(&newX, &newY, &newZ);

  posX = newX;
  posY = newY;
  posZ = newZ;

  if (abs(newX - oldX) >  THRESHOLD || abs(newY - oldY) > THRESHOLD || abs(newZ - oldZ) > THRESHOLD) {
    return true;
  } else {
    return false;
  }
}

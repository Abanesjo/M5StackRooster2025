#include <M5Core2.h>

#define THRESHOLD 2

float posX = 0.0f;
float posY = 0.0f;
float posZ = 0.0f;

double screenHeight, screenWidth;
int selectMode;
int xCursor, yCursor;
String placeHolderTime;

void setup() {
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.fillScreen(BLACK);
  screenHeight = M5.Lcd.height();
  screenWidth = M5.Lcd.width();
  xCursor = M5.Lcd.getCursorX();
  yCursor = M5.Lcd.getCursorY();
  M5.Lcd.setTextSize(7);
  M5.Lcd.setTextColor(BLACK, LIGHTGREY);
}

void loop() {
  homeScreenLayout();
  delay(150);
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

void homeScreenLayout()
{                 //Starts        Initial      step         height
  M5.Lcd.fillRect(0.1*screenWidth, 0.125*screenHeight, 0.8*screenWidth, 0.35 * screenHeight, LIGHTGREY);
  
  while (true) {
    
    M5.Lcd.setCursor(0.175 * screenWidth, 0.19 * screenHeight);
    placeHolderTime = "10:49";

    //Printing the Time
    M5.Lcd.print(placeHolderTime);
    //break function if touch screen detected
  }
}

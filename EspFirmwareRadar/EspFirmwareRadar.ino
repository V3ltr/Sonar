#include <TFT_eSPI.h>
#include <SPI.h>

#define TRIG_PIN  5
#define ECHO_PIN  19

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

const int centerX = 120;
const int centerY = 120;
const int maxDistance = 150;
const int maxRadius = 90;

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return maxDistance;
  
  float distance = duration * 0.0343 / 2.0;
  return (distance > maxDistance) ? maxDistance : distance;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  tft.init();
  tft.setRotation(0);
  img.createSprite(240, 240);
}

void loop() {
  float distance = getDistance();

  img.fillSprite(TFT_BLACK);

  img.drawCircle(centerX, centerY, maxRadius, tft.color565(60, 60, 70));

  img.fillCircle(centerX, centerY, 5, TFT_GREEN);

  int mappedDist = map((int)distance, 0, maxDistance, 0, maxRadius);
  int targetX = centerX + (int)(mappedDist * 0.707);
  int targetY = centerY - (int)(mappedDist * 0.707);

  img.fillCircle(targetX, targetY, 6, TFT_RED);
  
  img.setTextColor(TFT_RED, TFT_BLACK);
  img.drawString(String((int)distance) + " cm", targetX - 10, targetY + 10, 2);

  img.pushSprite(0, 0);
  delay(50);
}
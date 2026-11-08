#include <TFT_eSPI.h>
#include <SPI.h>
#include <ESP32Servo.h>

#define TRIG_PIN  5
#define ECHO_PIN  19
#define SERVO_PIN 13

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

const int centerX = 120;
const int centerY = 120;
const int radarRadius = 110;
const int maxDistance = 100;

Servo radarServo;

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

void drawRadarBackground() {
  img.fillSprite(TFT_BLACK);
  
  img.drawCircle(centerX, centerY, radarRadius, TFT_GREEN);
  img.drawCircle(centerX, centerY, radarRadius * 0.75, tft.color565(0, 100, 0));
  img.drawCircle(centerX, centerY, radarRadius * 0.50, tft.color565(0, 100, 0));
  img.drawCircle(centerX, centerY, radarRadius * 0.25, tft.color565(0, 100, 0));
  
  img.drawLine(centerX - radarRadius, centerY, centerX + radarRadius, centerY, tft.color565(0, 80, 0));
  img.drawLine(centerX, centerY - radarRadius, centerX, centerY + radarRadius, tft.color565(0, 80, 0));
}

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  radarServo.attach(SERVO_PIN);
  
  tft.init();
  tft.setRotation(0);
  
  img.createSprite(240, 240);
}

void loop() {
  for (int angle = 0; angle < 360; angle += 2) {
    int servoAngle = (angle <= 180) ? angle : (360 - angle);
    radarServo.write(servoAngle);
    
    float distance = getDistance();
    
    drawRadarBackground();
    
    float rad = (angle - 90) * DEG_TO_RAD;
    
    int sweepX = centerX + radarRadius * cos(rad);
    int sweepY = centerY + radarRadius * sin(rad);
    
    img.drawLine(centerX, centerY, sweepX, sweepY, TFT_GREEN);
    
    if (distance < maxDistance) {
      int targetDist = map((int)distance, 0, maxDistance, 0, radarRadius);
      int targetX = centerX + targetDist * cos(rad);
      int targetY = centerY + targetDist * sin(rad);
      img.fillCircle(targetX, targetY, 4, TFT_RED);
    }
    
    img.setTextColor(TFT_GREEN, TFT_BLACK);
    img.drawString("ANGLE: " + String(angle) + " deg", 10, 220, 2);
    img.drawString("DIST:  " + String((int)distance) + " cm", 130, 220, 2);
    
    img.pushSprite(0, 0);
    
    delay(15);
  }
}
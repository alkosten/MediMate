#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Servo.h>

RTC_DS3231 rtc;
Servo servo;

LiquidCrystal lcd(13, 12, 7, 6, 4, 3);

const int hrAM = 7;
const int minAM = 30;
const int secAM = 0;

const int hrPM = 22;
const int minPM = 55;
const int secPM = 45;

const int button = 5;
int buttonState = 0;

typedef struct {
  int minimum;
  int maximum;
} 

minMax_t;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  rtc.begin();
  servo.attach(9);
  pinMode(button, INPUT);
}

void loop() {
  DateTime rtcTime = rtc.now();
  buttonState = digitalRead(button);

  updateLCD();

  if (rtcTime.hour() == hrAM && rtcTime.minute() == minAM && rtcTime.second() == secAM) {
    lcd.clear();
    rotateAM();
  }

  if (rtcTime.hour() == hrPM && rtcTime.minute() == minPM && rtcTime.second() == secPM) {
    lcd.clear();
    rotatePM();
  }

  if (buttonState == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Yay! Pills Were Taken!");
  }

  if (Serial.available()) {
    char input = Serial.available();
    if (input == 'u') updateRTC();
  }
}

void rotateAM() {
  servo.write(0); 
  lcd.setCursor(0, 1);
  lcd.print("AM Medication");
}

void rotatePM() {
  servo.write(180); 
  lcd.setCursor(0, 1);
  lcd.print("PM Medication");
}

void updateRTC() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Edit Mode");

  const char txt[6][15] = {"year [4-digit]", "month [1~12]", "day [1~31]",
                           "hours [0~23]", "minutes [0~59]", "seconds [0~59]"};

  const minMax_t minMax[] = {
      {2000, 9999},
      {1, 12},    
      {1, 31},    
      {0, 23},   
      {0, 59},     
      {0, 59},   
  };

  long newDate[6];
  DateTime newDateTime;

  for (int i = 0; i < 6; i++) {
    while (1) {
      Serial.print("Enter ");
      Serial.print(txt[i]);
      Serial.print(" (or -1 to abort) : ");

      while (!Serial.available()) {
      }

      String str = Serial.readString();
      if (str == "-1") {
        Serial.println("\nABORTED");
        return;
      }

      newDate[i] = str.toInt();

      if (newDate[i] >= minMax[i].minimum && newDate[i] <= minMax[i].maximum)
        break;

      Serial.print(newDate[i]);
      Serial.print(" is out of range ");
      Serial.print(minMax[i].minimum);
      Serial.print(" - ");
      Serial.println(minMax[i].maximum);
    }

    Serial.println(newDate[i]);
  }

  newDateTime = DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
  if (newDateTime.isValid()) {
    rtc.adjust(newDateTime);
    Serial.println("RTC is Updated");
  } else {
    Serial.println("Date/time entered was invalid");
  }
}

void updateLCD() {
  DateTime rtcTime = rtc.now();
  char timeBuffer[] = "hh:mm:ss";
  lcd.setCursor(0, 0);
  lcd.print("Time");
  lcd.setCursor(5, 0);
  lcd.print(rtcTime.toString(timeBuffer));
}


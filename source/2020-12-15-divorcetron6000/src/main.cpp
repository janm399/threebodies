#include <Arduino.h>
#include <U8g2lib.h>
#include <MFRC522.h>
#include <SPI.h>
#include "binary.h"

#define BUILTIN_LED 2
#define LATCH_PIN 32
#define CLOCK_PIN 25
#define DATA_PIN 27

#define WS ESP8266WebServer
U8G2_SSD1327_MIDAS_128X128_1_4W_HW_SPI u8g2(U8G2_R0, /*cs*/ D2, /*dc*/ D0, /*reset*/ D1);
MFRC522 mfrc522(/*cs*/ 15, /*reset*/7);
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define BASE_ADDRESS 5

WS server(80);

String tagText = "";
bool programTag = false;
bool programmerMode = false;

void setup(void) {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  for (int i = 0; i < 10; i++) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(BUILTIN_LED, LOW);
    delay(200);
  }

  digitalWrite(BUILTIN_LED, HIGH);
  delay(1000);

  programmerMode = true;//analogRead(0) > 500;
  digitalWrite(BUILTIN_LED, LOW);

  if (programmerMode) {
    WiFi.softAP("Divorcetron6000");

    server.on("/", HTTP_GET, [](){
      String body;
      if (programTag) body = "<p>*** “" + tagText + "”</p>";
      server.send(200, "text/html", "<!DOCTYPE HTML><html><head><meta charset='utf-8'/><meta language='zh_CN'/></head><body><h1>NFC</h1><form action='/program' accept-charset='utf-8' method='post'><input type='text' name='text'/><input type='submit'/></form>" + body + "</body></html>");
    });

    server.on("/program", HTTP_POST, [](){
      if (server.hasArg("text")) {
        programTag = true;
        tagText = server.arg("text");
      }
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "");
    });

    server.begin();
  }

  SPI.begin();
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setFontDirection(0);

  mfrc522.PCD_Init();
}

void readText(byte* buffer) {
  byte size;
  for (int i = 0; i < 9; i++) {
    size = 18;
    mfrc522.MIFARE_Read(BASE_ADDRESS + i * 4, &buffer[i * 16], &size);
  }
}

void writeText(String text) {
  byte buffer[144];
  byte size = sizeof(buffer);
  memset(buffer, 0, size);

  text.getBytes(buffer, size);

  for (int i = 0; i < 36; i++) {
    mfrc522.MIFARE_Ultralight_Write(BASE_ADDRESS + i, &buffer[i * 4], 4);
  }
}

int counter = 0;
void loop(void) {
  byte buffer[144 + 2];
  memset(buffer, 0, sizeof(buffer));

  bool present = mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
  if (present) readText(buffer);
  String message = String((char*)buffer);

  if (present) {
    if (message != tagText && programTag) {
      writeText(tagText);
      message = tagText;
      digitalWrite(BUILTIN_LED, LOW);
      delay(1000);
      digitalWrite(BUILTIN_LED, HIGH);
      delay(1000);

      programTag = false;
    }
  }

  u8g2.firstPage();
  do {
    if (present) {
      u8g2.setCursor(0, 20);
      u8g2.print(message);
    } else {
      u8g2.setCursor(0, 20);
      u8g2.print("Ready");
      if (programmerMode && WiFi.softAPgetStationNum() == 0) {
        u8g2.setCursor(0, 50);
        u8g2.print((millis()/1000) % 10);
        u8g2.setCursor(10, 50);
        u8g2.print(WiFi.softAPIP());
        if (programTag) {
          u8g2.setCursor(0, 80);
          u8g2.print(tagText);
        }
      }
    }
  } while (u8g2.nextPage());
  if (present) delay(2000);

  server.handleClient();

  digitalWrite(LATCH_PIN, LOW);
  // BRG BRG BR- G BRG BRG X
  // 010 010 01-00 010 010 0
#define OB1 B10010010
#define OB2 B01001000

  // 100 100 10-00 100 100 0
#define OR1 B01001001
#define OR2 B00100100

  // 001 001 00-01 001 001 0
#define OG1 B00100100
#define OG2 B10010010

  if (counter % 3 == 0) {
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, OB2);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, OB1);
  } else if (counter % 3 == 1) {
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, OR2);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, OR1);
  } else {
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, OG2);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, OG1);
  }

  digitalWrite(LATCH_PIN, HIGH);

  delay(100);
  counter++;
}

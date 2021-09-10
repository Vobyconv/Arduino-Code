#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

int LedPin = 6;
int Leds = 4;
int pad = 8;

int redLevel = 0;
int greenLevel = 0;
int blueLevel = 0;

int lowMesure = 0;
int highMesure = 600;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_NeoPixel TiraLED = Adafruit_NeoPixel(Leds, LedPin, NEO_GRB + NEO_KHZ800);

void iniled() {
  TiraLED.begin();
  TiraLED.setBrightness(100);
  TiraLED.show();
  TiraLED.clear();
}

void iniWhite() {
 for (int i = 0; i <= Leds ; i++) {
    TiraLED.setPixelColor(i, random(20,255), random(10,200), random(10,200));
    TiraLED.show();
    
 }
 Serial.println("white");
}

void pinM(){
  pinMode(pad, INPUT_PULLUP);
}

void touch() { 
    Serial.print(pad);
  if (digitalRead(pad)==LOW) {
    Serial.println(digitalRead(pad));
    Serial.println("tocando");
    lecture();
    delay(10);
    }
  }

void lecture(){
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED
  
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.println(blue);

  int ColorRed = red;
  int ColorGreen = green;
  int ColorBlue = blue;
  int Nclear = clear;
  
  redLevel = map(ColorRed, lowMesure, highMesure, 0, 255);
  greenLevel = map(ColorGreen, lowMesure, highMesure, 0, 255);
  blueLevel = map(ColorBlue, lowMesure, highMesure, 0, 255);
  int brillo = map(Nclear, lowMesure, highMesure, 0, 255);

if (redLevel < 125) {redLevel = 20;} if (redLevel > 200) {redLevel = 255;}
if (greenLevel < 125) {greenLevel = 20;} if (greenLevel > 200) {greenLevel = 255;}
if (blueLevel < 125) {blueLevel = 20;} if (blueLevel > 200) {blueLevel = 255;}

  //Serial.print("C:\t"); Serial.print(clear);
  //Serial.print("\tRlevel:\t"); Serial.print(redLevel);
  //Serial.print("\tGlevel:\t"); Serial.print(greenLevel);
  //Serial.print("\tBlevel:\t"); Serial.println(blueLevel);
  //torch();
  }

void torch(){

    Serial.print("\tRlevel:\t"); Serial.print(redLevel);
    Serial.print("\tGlevel:\t"); Serial.print(greenLevel);
    Serial.print("\tBlevel:\t"); Serial.println(blueLevel);
    
    TiraLED.setBrightness(random(50,150));
    for (int i = 0; i <= Leds ; i++) {
    TiraLED.setPixelColor(i, redLevel, greenLevel, blueLevel);
    TiraLED.show();
  }
  Serial.println("color");
  }
 
void setup() {
  Serial.begin(9600);
  Serial.println("Color View Test!");
  iniled();
  Serial.println("iniled ok");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  iniled();
  pinM();
}


void loop() {
touch();
//lecture();
torch();
//boton();
delay(20);
}

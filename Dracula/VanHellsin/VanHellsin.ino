// C++ code
// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>

//Ordenes basicas ---------------------
int pinBestias = 12;
int puntosLed = 20;
int pinLedSkills = 10;
int puntosLedSkills = 6;

int numBestias = 4;
int poten = A0;
int bestia = 0;
const int ledPorBestia = (puntosLed/numBestias);

int const skillsNum = 6;
int skillsPin[skillsNum] = {2, 3, 4, 5, 6, 7};

void pinM(){ 
  for (int i = 0; i <= skillsNum-1; i++) {
    pinMode(skillsPin[i], INPUT_PULLUP);
  }  
}

void pinMcomp(){ 
  for (int i = 0; i <= skillsNum-1; i++) {
  Serial.print(F("numero pin _ "));
  Serial.print(skillsPin[i]);
  Serial.print(F("  estado de boton _ "));
  Serial.println(digitalRead(skillsPin[i]));
  }  
}

//tiras -------------------
Adafruit_NeoPixel tiraBestias = Adafruit_NeoPixel(puntosLed, pinBestias, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tiraSkills = Adafruit_NeoPixel(puntosLedSkills, pinLedSkills, NEO_GRB + NEO_KHZ800);


void iniled() {
  tiraBestias.begin();
  tiraBestias.setBrightness(200);
  tiraBestias.show();
  tiraBestias.clear();
  
  tiraSkills.begin();
  tiraSkills.setBrightness(200);
  tiraSkills.show();
  tiraSkills.clear();
}



const uint32_t verde = Adafruit_NeoPixel::Color(5, 250, 5);
const uint32_t azul = Adafruit_NeoPixel::Color(5, 5, 250);
const uint32_t rojo = Adafruit_NeoPixel::Color(250, 5, 5);
const uint32_t amarillo = Adafruit_NeoPixel::Color(250, 250, 5);
const uint32_t blanco = Adafruit_NeoPixel::Color(200, 200, 200);
  
void hombreLobo(){
  int posicion = 1;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, verde);
      tiraBestias.show();
  }
}

void aracne(){
  int posicion = 2;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, verde);
      tiraBestias.show();
  }
}

void chupacabras(){
  int posicion = 3;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, rojo);
    tiraBestias.show();
  }
}

void serpiente(){
  int posicion = 4;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, amarillo);
      tiraBestias.show();
  }
}

//Selector ------
void selector() {
    int rosca = analogRead(A0);
    int bestia = map(rosca, 0, 1030, 1, numBestias+1);
    Serial.print(F("rosca = "));
    Serial.print(rosca);
  Serial.print(F(" / num Bestia = "));
    Serial.println(bestia);

  int posicion = bestia;
  int factor = posicion-1;
  
    for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, azul);
    tiraBestias.show();
    }
      tiraBestias.clear();
}

void exitoLobo() {
  int loboOK[] = { 1, 1, 1, 0, 0, 0};
  int total = skillsNum;
  int exito = 0;
  for (int i = 0; i <= total-1 ; i++) {
    if (digitalRead(skillsPin[i]) == loboOK[i]) {
    exito++;
    Serial.print(i);
    Serial.println(F(" // Coincidencia //"));  
    }
    else {
    Serial.print(i);
    Serial.println(F(" // Falla //"));
    } 
  }
  if (exito == total) {hombreLobo();}
}

void exitoAracne() {
  int aracneOK[] = { 0, 0, 1, 0, 0, 1};
  int total = skillsNum;
  int exito = 0;
  for (int i = 0; i <= total-1 ; i++) {
    if (digitalRead(skillsPin[i]) == aracneOK[i]) {
    exito++;
    Serial.print(i);
    Serial.println(F(" // Coincidencia //"));  
    }
    else {
    Serial.print(i);
    Serial.println(F(" // Falla //"));
    } 
  }
  if (exito == total) {aracne();}
}

// Skills ---------------------------
  void pruebaSkills() {
  
  int skillsOnOff[skillsNum] = {0, 1, 2, 3, 4, 5};  
   
  for (int i = 0; i < skillsNum; i++) {
    if (digitalRead(skillsPin[i]) == 0) {
    tiraSkills.setPixelColor(skillsOnOff[i], rojo);
    tiraSkills.show();
    }
    else {
    tiraSkills.setPixelColor(skillsOnOff[i], verde);
    tiraSkills.show();
    }
    Serial.print(F("skill  : "));
    Serial.print(skillsOnOff[i]);
    Serial.print(F(" / valor  : "));
    Serial.println(digitalRead(skillsPin[i]));
    }
}


void setup()
{
  Serial.begin(9600);
  Serial.println(F(" "));
  Serial.println(F("++ Cazando Bestias ++"));
  iniled();
  pinM();
}

void loop()
{
  pruebaSkills();
  selector(); 
  pinMcomp();
  exitoLobo();
  exitoAracne();
}

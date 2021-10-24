// C++ code
// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>

//Ordenes basicas ---------------------
int pinBestias = 10;
int puntosLed = 36;
int pinLedSkills = 12;
int puntosLedSkills = 12;
int rele01 = 9;
int rele02 = 11;
int rele03 = 13;

const int numBestias = 4;
int poten = A0;
int bestia = 0;
const int ledPorBestia = (puntosLed/numBestias);
int victorias[numBestias] = {0, 0, 0, 0};


int const skillsNum = 12;
int skillsPin[skillsNum] = {2, 3, 4, 5, 6, 7, 8, A1, A2, A3, A4, A5};
int apuntado[4] = { 3, 12, 21, 30};

void pinM(){ 
  for (int i = 0; i <= skillsNum-1; i++) {
    pinMode(skillsPin[i], INPUT_PULLUP);
  }  
}

void pinR(){ 
    pinMode(rele01, OUTPUT);
    pinMode(rele02, OUTPUT);
    pinMode(rele03, OUTPUT);
}

void pinMcomp(){ 
  for (int i = 0; i <= skillsNum-1; i++) {
//  Serial.print(F("numero pin _ "));
//  Serial.print(skillsPin[i]);
//  Serial.print(F("  estado de boton _ "));
//  Serial.println(digitalRead(skillsPin[i]));
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
  tiraSkills.setBrightness(100);
  tiraSkills.show();
  tiraSkills.clear();
}

const uint32_t verde = Adafruit_NeoPixel::Color(5, 250, 5);
const uint32_t azul = Adafruit_NeoPixel::Color(5, 5, 250);
const uint32_t rojo = Adafruit_NeoPixel::Color(250, 5, 5);
const uint32_t amarillo = Adafruit_NeoPixel::Color(250, 250, 5);
const uint32_t blanco = Adafruit_NeoPixel::Color(200, 200, 200);
const uint32_t naranja = Adafruit_NeoPixel::Color(250, 50, 5);


void aracneOn(){
  int posicion = 1;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, verde);
      tiraBestias.show();
  }
}

void moscaOn(){
  int posicion = 2;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, amarillo);
      tiraBestias.show();
  }
}

void loboOn(){
  int posicion = 3;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, verde);
      tiraBestias.show();
  }
}

void chupacabrasOn(){
  int posicion = 4;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, naranja);
    tiraBestias.show();
  }
}
// numeracion  -------  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12
int aracneOK[] =      { 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0};
int moscaOK[] =       { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1};
int loboOK[] =        { 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0};
int chupacabrasOK[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};


//Selector ------
void selector() {
    int rosca = analogRead(A0);
    bestia = map(rosca, 0, 1030, 1, numBestias+1);
    Serial.print(F("rosca = "));
    Serial.print(rosca);
    Serial.print(F(" / num Bestia = "));
    Serial.println(bestia);

  int posicion = bestia;
  int factor = posicion-1;
  
    //for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(apuntado[bestia-1], azul);
    tiraBestias.show();
    //}
    tiraBestias.clear();
}

// Victoria -------------------------


void exitoAracne() {
  int total = skillsNum;
  int exito = 0;
  
  if (bestia == 1){
    for (int i = 0; i <= total-1 ; i++) {
      if (digitalRead(skillsPin[i]) == aracneOK[i]) {
      exito++;
      //  Serial.print(i);
      //  Serial.println(F(" // Coincidencia //"));  
      }
      else {
      // Serial.print(i);
      // Serial.println(F(" // Falla //"));
      } 
      }
    if (exito == total) {
    victorias[bestia] = 1;
    }
  }
 
  if (victorias[0] == 1) {
  aracneOn();
  }
  
  Serial.print(F("victoria  -  "));
  Serial.print(bestia);
  Serial.print(F(" /  estado  -  "));
  Serial.println(victorias[bestia-1]);
}

void exitoMosca() {
  int total = skillsNum;
  int exito = 0;
  
  if (bestia == 2){
    for (int i = 0; i <= total-1 ; i++) {
      if (digitalRead(skillsPin[i]) == moscaOK[i]) {
      exito++;
  //  Serial.print(i);
  //  Serial.println(F(" // Coincidencia //"));  
      }
      else {
  // Serial.print(i);
  // Serial.println(F(" // Falla //"));
      } 
    }
 
  if (exito == total) {
  victorias[bestia] = 1;
  }
  }
 
  if (victorias[1] == 1) {
  moscaOn();
  }
  Serial.print(F("victoria  -  "));
  Serial.print(bestia);
  Serial.print(F(" /  estado  -  "));
  Serial.println(victorias[bestia-1]);
}
  
void exitoLobo() {
  int total = skillsNum;
  int exito = 0;
  
  if (bestia == 3){
    for (int i = 0; i <= total-1 ; i++) {
      if (digitalRead(skillsPin[i]) == loboOK[i]) {
      exito++;
      //  Serial.print(i);
      //  Serial.println(F(" // Coincidencia //"));  
      }
      else {
      // Serial.print(i);
      // Serial.println(F(" // Falla //"));
      } 
    }
  
  if (exito == total) {
  victorias[bestia] = 1;
  }
  }
  
  if (victorias[2] == 1) {
  loboOn();
  }
  Serial.print(F("victoria  -  "));
  Serial.print(bestia);
  Serial.print(F(" /  estado  -  "));
  Serial.println(victorias[bestia-1]);
}

void exitoChupacabras() {
  int total = skillsNum;
  int exito = 0;
  
  if (bestia == 4){
    for (int i = 0; i <= total-1 ; i++) {
      if (digitalRead(skillsPin[i]) == chupacabrasOK[i]) {
      exito++;
      //  Serial.print(i);
      //  Serial.println(F(" // Coincidencia //"));  
      }
      else {
      // Serial.print(i);
      // Serial.println(F(" // Falla //"));
      }
    }
    if (exito == total) {
    victorias[bestia] = 1;
    }
  }
  if (victorias[3] == 1) {
  chupacabrasOn();
  }
  Serial.print(F("victoria  -  "));
  Serial.print(bestia);
  Serial.print(F(" /  estado  -  "));
  Serial.println(victorias[bestia-1]);
}

void exitoTodos() {
  exitoAracne();
  exitoMosca();
  exitoLobo();
  exitoChupacabras();
  }



// Skills ---------------------------
  void pruebaSkills() {
  
  int skillsOnOff[skillsNum] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};  
   
  for (int i = 0; i < skillsNum; i++) {
    if (digitalRead(skillsPin[i]) == 1) {
    tiraSkills.setPixelColor(skillsOnOff[i], verde);
    tiraSkills.show();
    }
    else {
    tiraSkills.setPixelColor(skillsOnOff[i], naranja);
    tiraSkills.show();
    }
  //  Serial.print(F("skill  : "));
  //  Serial.print(skillsOnOff[i]);
  //  Serial.print(F(" / valor  : "));
  //  Serial.println(digitalRead(skillsPin[i]));
    }
}

void openReles(){
  if (victorias[1]+victorias[2]+victorias[3]+victorias[4] == 2){
  digitalWrite(rele01, HIGH);  
  Serial.println(F(" Open01 "));
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println(F(" "));
  Serial.println(F("++ Cazando Bestias ++"));
  iniled();
  pinM();
  pinR();
}

void loop()
{
  pruebaSkills();
  selector(); 
  pinMcomp();
  exitoTodos();
  openReles();
  digitalWrite(rele02, HIGH);
}

#include <Adafruit_NeoPixel.h>

int tira1 = 3;
int tira2 = 4;
int tira5 = 5;
int puntosled = 4;
int puntosheart1 = 6;
int puntosheart2 = 15;
int interruptor = 7;
int pulsador=0;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(puntosled, tira1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(puntosheart1, tira2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(puntosheart2, tira5, NEO_GRB + NEO_KHZ800);

int mediluz = A1;
int medipot = A2;

int corto = 250;
int largo = 1000;
int espera = 1000;
int vuelta = 20;
int pulsar = 35;
int bombeo = 11;
int factor = 4;

void iniled(){
  strip1.begin();
  strip1.setBrightness(200);
  strip1.show();
  strip1.clear();

  strip2.begin();
  strip2.setBrightness(200);
  strip2.show();
  strip2.clear();

  strip5.begin();
  strip5.setBrightness(200);
  strip5.show();
  strip5.clear();
}

void a(){punto(); raya(); delay(espera);}
void b(){raya(); punto(); punto(); punto(); delay(espera);}
void c(){raya(); punto(); raya(); punto(); delay(espera);}
void d(){raya(); punto(); punto(); delay(espera);}
void e(){punto(); delay(espera);}
void f(){punto(); punto(); raya(); punto(); delay(espera);}
void g(){raya(); raya(); punto(); delay(espera);}
void h(){punto(); punto(); punto(); punto(); delay(espera);}
void i(){punto(); punto(); delay(espera);}
void j(){punto(); raya(); raya();raya(); delay(espera);}
void k(){raya(); punto(); raya(); delay(espera);}
void l(){punto(); raya(); punto(); punto(); delay(espera);}
void m(){raya(); raya(); delay(espera);}
void n(){raya(); punto(); delay(espera);}
void o(){raya(); raya(); raya(); delay(espera);}
void p(){punto(); raya(); raya(); punto(); delay(espera);}
void q(){raya(); raya(); punto(); raya(); delay(espera);}
void r(){punto(); raya(); punto(); delay(espera);}
void s(){punto(); punto(); punto(); delay(espera);}
void t(){raya(); delay(espera);}
void u(){punto(); punto; raya(); delay(espera);}
void v(){punto(); punto(); punto(); raya(); delay(espera);}
void w(){punto(); raya(); raya(); delay(espera);}
void x(){raya(); punto(); punto(); raya(); delay(espera);}
void y(){raya(); punto(); raya(); raya(); delay(espera);}
void z(){raya(); raya(); punto(); punto(); delay(espera);}

void punto(){
  for (int i = 0; i <= puntosled; i++) {
    strip1.setPixelColor(i, 150, 155, 250);
    strip1.show();
  }
  delay(corto);
  strip1.clear();
  strip1.show();
  delay(corto);
}

void raya(){
  for (int i = 0; i <= puntosled; i++) {
    strip1.setPixelColor(i, 150, 155, 250);
    strip1.show();
  }
  delay(largo);
  strip1.clear();
  strip1.show();
  delay(corto);
}

void start(){
 for (int i = 0; i <= 10; i++) {
  strip1.setPixelColor(i, 150+i*10, 1, 1);
  strip1.show();
 }
  delay(1000);
  strip1.clear();
  strip1.show();
  delay(1000);
}

void beat(){
  for (int i = 0; i <= bombeo ; i++) {
      strip2.setPixelColor(i, 50+i*factor, 50+i*factor, 50+i*factor+20);
      strip2.show();
      delay(pulsar);
 }
  
 for (int i = 0; i <= bombeo ; i++) {
      strip2.setPixelColor(i, 255-i*factor, 255-i*factor, 255-i*factor-20);
      strip2.show();
      delay(pulsar);
      }
 Serial.println("palpitar");
}

void beatone(){
  for (int i = 0; i <= bombeo ; i++) {
      strip2.setPixelColor(i, 50+i*factor, 50+i*factor, 50+i*factor+20);
      strip2.show();
      delay(pulsar);
 }

 Serial.println("palpitar");
}

void snake(){
  for (int i = 0; i <= puntosheart2 ; i++) {
      strip5.setPixelColor(i, 210, 210, 255);
      strip5.show();
      delay(vuelta);
      strip5.setPixelColor(i, 130, 130, 160);
      strip5.show();
      delay(vuelta);
    
      }
    Serial.println("lucecicas");
}

void sensorluz(){
 int poten = analogRead(A2); 
 int medipot = map(poten, 0, 1023, 0, 20);
  Serial.print("{potpoten: ");
  Serial.print(poten);
  Serial.print("/ sensibilidad: ");
  Serial.println(medipot);
  
 int mediluz = analogRead(A1); 
 int medipos = map(mediluz, 300, 800, 0, 20);
  Serial.print("} {luz: ");
  Serial.print(mediluz);
  Serial.print("/ medida: ");
  Serial.println(medipos);

  if (medipos > medipot){
  //un parpadeo rojo de inicio
    start();
  //emision   
      j(); Serial.print("- j -");
      a(); Serial.print(" a -");
      r(); Serial.print(" r -");
      v(); Serial.print(" v -");
      i(); Serial.print(" i -");
      s(); Serial.println(" s -");
  }
     else {
     strip1.clear();
     strip1.show();  
     }
     delay(5);
}

void setup(){
  Serial.begin(9600);
  Serial.println("+++ Industrias Stark +++ iniciando a jarvis ...");
  pinMode(interruptor, INPUT_PULLUP);
  iniled();
}

void loop(){
  pulsador = digitalRead(interruptor);
  if (pulsador==LOW) { 
    Serial.println("Core conected");
    beat();
    snake();
    sensorluz();
  }
  else{
    Serial.println("Disconected");
    strip1.clear();
    strip1.show(); 

    strip2.clear();
    strip2.show(); 

    strip5.clear();
    strip5.show(); 
    delay(corto);
    }
}

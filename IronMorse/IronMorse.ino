#include <Adafruit_NeoPixel.h>

int tira1 = 3;
int puntosled = 4;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(puntosled, tira1, NEO_GRB + NEO_KHZ800);

int mediluz = A1;
int medipot = A2;

int corto = 250;
int largo = 750;
int espera = 1000;

void iniled(){
  strip1.begin();
  strip1.setBrightness(200);
  strip1.show();
  strip1.clear();
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
    strip1.setPixelColor(i, 10, 255, 10);
    strip1.show();
  }
  delay(corto);
  strip1.clear();
  strip1.show();
  delay(corto);
}

void raya(){
  for (int i = 0; i <= puntosled; i++) {
    strip1.setPixelColor(i, 10, 255, 10);
    strip1.show();
  }
  delay(largo);
  strip1.clear();
  strip1.show();
  delay(corto);
}

void start(){
 for (int i = 0; i <= puntosled; i++) {
  strip1.setPixelColor(i, 150+i*20, 1, 1);
  strip1.show();
 }
  delay(1000);
  strip1.clear();
  strip1.show();
  delay(1000);
}

void sensorluz(){
 int poten = analogRead(A2); 
 int medipot = map(poten, 0, 1024, 0, 20);
  Serial.print("{potpoten: ");
  Serial.print(poten);
  Serial.print("/ sensibilidad: ");
  Serial.println(medipot);
  
 int mediluz = analogRead(A1); 
 int medipos = map(mediluz, 900, 1020, 0, 20);
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
     delay(500);
}

void setup(){
  Serial.begin(9600);
  Serial.println("+++ Industrias Stark +++ iniciando a jarvis ...");
  iniled();
}

void loop()
{
sensorluz();
}

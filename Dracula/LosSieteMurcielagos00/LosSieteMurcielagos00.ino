// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

const int NUM_MURCIELAGOS = 7;
const int NUM_LEDS = 2;

//Ordenes basicas ---------------------
int tiraEyes = 10;
int rele = 12;
int puntosEyes = 14;
int sensibilidad = 6;
int tope = 20;
int limiteChillido = 48;
int agudo =1100;
int tiempoChillido = 4;

Adafruit_NeoPixel stripBat = Adafruit_NeoPixel(puntosEyes, tiraEyes, NEO_GRB + NEO_KHZ800);

//pines de murcielagos
int batSensores[NUM_MURCIELAGOS] = {A0, A1, A2, A3, A4, A5, A6};

//chillidos
int batSound[NUM_MURCIELAGOS] = {2, 3, 4, 5, 6, 7, 8, };

//puntitos de Ojos murcielago
int batLeds[NUM_MURCIELAGOS][NUM_LEDS] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}};

//Contadores de luz
int batLights[NUM_MURCIELAGOS] = {0, 0, 0, 0, 0, 0, 0};

//Contadores de victoria
bool solucionNum[NUM_MURCIELAGOS] = { false, false, false, false, false, false, false };

//Para omitir comprobaciones
bool batCheck[NUM_MURCIELAGOS] = { false, false, false, false, false, false, false };


//inicio de tioras led
void iniled() 
{
    stripBat.begin();
    stripBat.setBrightness(10);
    stripBat.show();
    stripBat.clear();
}

// Funciones de murcielagos
void batDark(int murci)
{
  int chillido = random(0, 50);
    //Serial.print((String)" / Chillido: " + chillido);
  
    if (chillido >= 40)
    {
      stripBat.setPixelColor(batLeds[murci][0], random(100), 0, 0);
      stripBat.setPixelColor(batLeds[murci][1], random(100), 0, 0);
      stripBat.show();
    }
    //else
    //{
        //stripBat.setPixelColor(batLeds[murci][0], 0, 0, 0);
        //stripBat.setPixelColor(batLeds[murci][1], 0, 0, 0);
        //stripBat.show();
    //}
}

void batLight(int murci)
{
  stripBat.setPixelColor(batLeds[murci][0], 200, 200, 0);
    stripBat.setPixelColor(batLeds[murci][1], 200, 200, 0);
    stripBat.show();  
}

void batBright(int murci)
{
    stripBat.setPixelColor(batLeds[murci][0], 0, 0, 200);
    stripBat.setPixelColor(batLeds[murci][1], 0, 0, 200);
    stripBat.show();
}

void pinM()
{
    pinMode(rele, OUTPUT);
}

void win() 
{
  digitalWrite(rele, HIGH);
    delay(1000); // Wait for 1000 millisecond(s)
}

void releOpen() 
{
  int qttWin = 0;
  
    for(int i = 0; i < NUM_MURCIELAGOS; i++)
    {
      if(solucionNum[i] == true)
      {
        qttWin++; 
      } 
      else
      {
        qttWin = 0;
          break;
      }
    }
  
    if (qttWin == NUM_MURCIELAGOS) 
    {
      win();
      Serial.println("Rele abierto");
    }
    else 
    {
      Serial.println("Rele cerrado");
    }
}

void checkBats(int murci) 
{
  
  int batSensor = analogRead(batSensores[murci]);
    int batLevel = map(batSensor, 0, 700, 0, 10);
       
    //Serial.print((String)"/Murci " + murci + ": " + batSensor);
    //Serial.print((String)" /Medida: " + batLevel);
    //Serial.print((String)" /Estado: " + batLights[murci]);
   
    int chillido = random(0, 50);
    //Serial.print((String)" /Chillido: " + chillido);
    if (chillido >= limiteChillido)
    {
      tone(batSound[murci], agudo, tiempoChillido);
    }
  
    if (batLevel >= sensibilidad) 
    {
        //Serial.print((String)" / UP / medidor luz: " + batLights[murci]); 
        batLight(murci); 
        batCheck[murci] = true;
        batLights[murci] = batLights[murci] + 1;
        tone(batSound[murci], agudo+batLights[murci]*50, tiempoChillido*3);
    }
    else 
    {
        batCheck[murci] = false;
      //Serial.print((String)" /  DOWN / medidor luz: ");  
    }
  
    if (batLights[murci] >= tope) 
    {
      //Serial.println((String)" / bat Bright /" + batLights[murci]);
      batBright(murci);
      solucionNum[murci] = true;
    }
    else 
  {
      batDark(murci);
      //Serial.println((String)" /  bat Evil /");
      solucionNum[murci] = false;   
    }
  
    //Serial.println((String)"/");
}

void murcielagos() 
{
  for(int i = 0; i < NUM_MURCIELAGOS; i++)
    {      
      if(!solucionNum[i])
      {
        checkBats(i);    
      }
    }
}

void ojitos() 
{
  int murcielagoRND = 0;
    int ojosRND = 0;
    int resetRND = 0;
  
    resetRND = random(14);
    murcielagoRND = random(0, NUM_MURCIELAGOS);
  
    if(resetRND <= 4 && !batCheck[murcielagoRND])
    {
      resetOjos();
    }
  
    if(!batCheck[murcielagoRND])
    {
      stripBat.setPixelColor(batLeds[murcielagoRND][0], 200, 0, 0);
      stripBat.setPixelColor(batLeds[murcielagoRND][1], 200, 0, 0);
      stripBat.show();
    }  
}

void resetOjos()
{
    for(int i = 0; i < NUM_MURCIELAGOS; i++)
    {
      if(!batCheck[i])
      {
          stripBat.setPixelColor(batLeds[i][0], 0, 0, 0);
          stripBat.setPixelColor(batLeds[i][1], 0, 0, 0);
          stripBat.show(); 
      }
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println("++Starting Murcielagos++");
    iniled();
    //pinMode(batSound0,OUTPUT); 
}

void loop()
{
    // Miradas
    ojitos();
    murcielagos();
    //releOpen();
    delay(80);
}

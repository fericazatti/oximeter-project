#include "timmer.h"
#include "bluetooth.h"

#define RXD2 39
#define TXD2 17
#define led 2

byte pack[5], imenu=1;
const byte push1 = 23;
boolean segundo=false;
int   totalInterruptCounter=0;

boolean  estadoled=false;

byte verificar() {
  byte serial_reset;
  if ((pack[0] > 128) && (pack[0]<210)){
    serial_reset = 0; 
  } else {
    serial_reset = 1;
  }
  return serial_reset;
}

void lectura() {
  if (Serial2.available()>4){
    Serial2.readBytes(pack,5);
    digitalWrite(led,LOW);
    UpdateData();
  }
}

void menu(){
  digitalWrite(led, HIGH);  
  imenu++;
  if(imenu>5){
    imenu=1; 
  }
}

void UpdateData()  {
  switch (imenu){
  case 1: 
    Serial.println(pack[0]);
    break;
  case 2: 
    Serial.println(pack[1]);
    break;
  case 3: 
    Serial.println(pack[2]);
    break;
  }
  if (segundo)
  {
    switch (imenu)
    {
    case 4: 
      Serial.println(pack[3]);
      BTwrite(pack[3]);
      break;   
    case 5: 
      Serial.println(pack[4]); 
      BTwrite(pack[4]);
      break;
    }
    segundo = false;
  }
} 

////////////////////SETUP////////////////////////

void setup() {
  Serial.begin(115200);
  pinMode (led, OUTPUT);
  pinMode(push1, INPUT_PULLUP);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(push1) , menu, FALLING);
  
  //setup timmer
  setup_timmer();
  
  //setup bluethooth
  setup_bluetooth();
    
}

////////////////////MAIN////////////////////////

void loop() { 
  lectura();
  if (verificar())
  {
    byte a = Serial2.read();
    pack[0]=143;
  }
  if (interruptCounter > 0) 
  {
    incrementatimmer();
    segundo = true;
    totalInterruptCounter++;
  }
  confirmBT();
  delay(1);
}

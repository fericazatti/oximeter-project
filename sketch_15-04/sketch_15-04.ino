#include "BluetoothSerial.h"

#define RXD2 39
#define TXD2 17
#define led 2

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


BluetoothSerial SerialBT;
boolean confirmRequestPending = true;
byte pack[5], imenu=1;
const byte push1 = 23;
boolean segundo=false;

volatile int interruptCounter;
int totalInterruptCounter;
boolean  estadoled=false;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void BTConfirmRequestCallback(uint32_t numVal)
{
  confirmRequestPending = true;
  Serial.println(numVal);
}

void BTAuthCompleteCallback(boolean success)
{
  confirmRequestPending = false;
  if (success)
  {
    Serial.println("Pairing success!!");
  }
  else
  {
    Serial.println("Pairing failed, rejected by user!!");
  }
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

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
      SerialBT.write(pack[3]);
      break;   
    case 5: 
      Serial.println(pack[4]); 
      SerialBT.write(pack[4]);

      break;
    }
    segundo = false;
  }
} 

void setup() {
  Serial.begin(115200);
  pinMode (led, OUTPUT);
  pinMode(push1, INPUT_PULLUP);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(push1) , menu, FALLING);
  //setup timmer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 500000, true);
  timerAlarmEnable(timer);
  //setup bluethooth
  Serial.begin(115200);
  SerialBT.enableSSP();
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  
}

void confirmBT()
{
    if (confirmRequestPending)
  {
    if (Serial.available())
    {
      int dat = Serial.read();
      if (dat == 'Y' || dat == 'y')
      {
        SerialBT.confirmReply(true);
      }
      else
      {
        SerialBT.confirmReply(false);
      }
    }
  }
}

void loop() { 
  lectura();
  if (verificar()){
//    digitalWrite(led, HIGH);  
    byte a = Serial2.read();
    pack[0]=143;
  }
  if (interruptCounter > 0) 
  {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    segundo = true;
    totalInterruptCounter++;
  }
  confirmBT();
  delay(1);
}

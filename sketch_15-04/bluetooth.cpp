#include "BluetoothSerial.h"
#include <Arduino.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
boolean confirmRequestPending = true;

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
void setup_bluetooth()
{
  SerialBT.enableSSP();
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

}

void BTwrite (byte dato)
{
  SerialBT.write(dato);  
}

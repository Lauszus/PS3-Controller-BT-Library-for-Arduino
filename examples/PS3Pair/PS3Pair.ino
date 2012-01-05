#include <ps3_usb.h>

unsigned char BDADDR[6] = { 0x00, 0x1F, 0x81, 0x00, 0x08, 0x30 };

PS3_USB PS3;// create an object for the PS3 Game Controller

void setup() 
{
  Serial.begin(115200);
  Serial.println("\r\nConnect a PS3 Controller to change the bluetooth address");
  PS3.init();
} 

void loop() 
{ 
  PS3.task();// perform the regular USB routines

  if (PS3.statConnected())// report received ?
  { 
    if(PS3.MoveConnected())
      PS3.setMoveBDADDR(BDADDR);
    else
      PS3.setBDADDR(BDADDR);
    printBDADDR(BDADDR);
    while(1);
  }
} 

void printBDADDR(unsigned char* bdaddr)
{
  Serial.print("The bluetooth address was set to: ");
  
  for(int i=0;i<5;i++)
  {
    if(bdaddr[i] < 16)
      Serial.print("0");
    Serial.print(bdaddr[i], HEX);
    Serial.print(":");
  }
  if(bdaddr[5] < 16)
    Serial.print("0");
  Serial.println(bdaddr[5], HEX);
}



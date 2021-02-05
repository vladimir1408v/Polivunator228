#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(11, 12); // RX, TX

String bufferSIM;

void setup()  
{
  Serial.begin(9600);
  mySerial.begin(4800);
}



void loop() // выполняется циклически
{
  if (mySerial.available())
  {
    char readSIM = mySerial.read();
    bufferSIM = bufferSIM + readSIM;
    
    if(readSIM == '\r')
    {
      if(bufferSIM.substring(1, 6) == "+CMTI")
      {
        mySerial.write("AT+CMGR=15\r");
      }

      for(int i = 0; i < bufferSIM.length(); i++)
        Serial.write(bufferSIM[i]);
      
      bufferSIM = "";
    }
    
  }
  if (Serial.available())
  {
        mySerial.write(Serial.read());
  }
}

#include <SoftwareSerial.h>

class SmsWork
{
public:
  SoftwareSerial* mySerial;
  String bufferSIM;
  bool ismess = false;
  void (*pt2Func)(String ) = NULL;

  SmsWork(){
    mySerial = new SoftwareSerial(9, 8);
    mySerial->begin(4800);
    mySerial->write("AT+CMGF=1\r");
  }

  void Work()
  {
    if (mySerial->available())
    {
      char readSIM = mySerial->read();
      bufferSIM = bufferSIM + readSIM;

      if (readSIM == '\r')
      {

        //for (int i = 0; i < bufferSIM.length(); i++)
          //Serial.write(bufferSIM[i]);
          
        if (bufferSIM.substring(1, 6) == "+CMTI")
        {
          String num = bufferSIM.substring(13);
          num = "AT+CMGR="+num+"\r";
          mySerial->write(num.c_str());
        }

        if(ismess)
        {
            ismess = false;
            pt2Func(bufferSIM.substring(1).substring(0, bufferSIM.length()-2));
        }

        if (bufferSIM.substring(1, 6) == "+CMGR")
            ismess = true;

        bufferSIM = "";
      }
    }
    
    if (Serial.available())
      mySerial->write(Serial.read());
    
  }
};

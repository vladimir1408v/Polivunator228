#include <SoftwareSerial.h>

class SmsWork
{
public:
  SoftwareSerial* mySerial;
  String bufferSIM;
  bool ismess = false;
  void (*pt2Func)(String ) = NULL;

  SmsWork(){
    mySerial = new SoftwareSerial(11, 12);
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

SmsWork sms_device;

void receivesms(String str)
{
  if(str == "Offer jtdcb its boon scrub")
    Serial.write("EEES");
}

void setup()
{
  sms_device.pt2Func = &receivesms;
  Serial.begin(9600);
}

void loop() // выполняется циклически
{
  sms_device.Work();
}
#include <SoftwareSerial.h>

class SmsWork
{
public:
  SoftwareSerial* mySerial;
  String bufferSIM;

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
      bool ismess = false;

      if (readSIM == '\r')
      {
        if (bufferSIM.substring(1, 6) == "+CMTI")
        {
          String num = bufferSIM.substring(13);
          num = "AT+CMGR="+num+"\r";
          mySerial->write(num.c_str());
        }

        if(ismess)
        {
            ismess = false;
            Serial.write(bufferSIM.c_str());
        }

        if (bufferSIM.substring(1, 6) == "+CMGR")
        {
            ismess = true;
        }


        for (int i = 0; i < bufferSIM.length(); i++)
          Serial.write(bufferSIM[i]);

        bufferSIM = "";
      }
    }
    if (Serial.available())
    {
      mySerial->write(Serial.read());
    }
  }
};

SmsWork sms_device;

void setup()
{
  Serial.begin(9600);
}

void loop() // выполняется циклически
{
  sms_device.Work();
}

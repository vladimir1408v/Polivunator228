#include <SoftwareSerial.h>

class SmsWork
{
public:
  SoftwareSerial* mySerial;
  String bufferSIM;
  bool ismess = false;
  void (*pt2Func)(String ) = NULL; // Кэлбек входящего сообщения

  SmsWork(){
    // Инициализируем работу виртуального Serial порта
    mySerial = new SoftwareSerial(10, 11);
    mySerial->begin(4800);
    // Устанавливаем режим работы GSM модуля в режим текстовых сообщений
    mySerial->write("AT+CMGF=1\r");
  }

  /* Функция Work вызывается в цикле Loop, и проверяет
     входящие сообщения, если получено новое смс то
     вызывается функция кэлбэк, заданая в функции Setup*/
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
            mySerial->write("AT+CMGD=1\r");
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

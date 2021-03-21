#include "DHT.h"
#include "GSM_POLIV.h"
#include <LCD_1602_RUS.h>
#include <IRremote.h>
#include <SmartDelay.h>
#include <iarduino_RTC.h>

#define KEY_OK 16726215
#define KEY_LEFT 16716015
#define KEY_RIGHT 16734885
#define KEY_UP 16718055
#define KEY_DOWN 16730805
#define KEY_0 16750695
#define KEY_Z 16738455
#define KEY_R 16756815
#define KEY_1 16753245
#define KEY_2 16736925
#define KEY_3 16769565
#define KEY_4 16720605
#define KEY_5 16712445
#define KEY_6 16761405
#define KEY_7 16769055
#define KEY_8 16754775
#define KEY_9 16748655

enum Page{
    MENU_HOME,
    MENU_SETUP,
    MENU_LCD_BRIGLIGHT,
    MENU_POLIV1,
    MENU_POLIV2,
    MENU_POLIV3,
    MENU_INFO_POLIV1,
    MENU_INFO_POLIV2,
    MENU_INFO_POLIV3,
    MENU_SET_PARAMETERS
};

// Объявляем объекты библиотек и передаём в конструктор параметры инициализации
LCD_1602_RUS lcd(0x27, 16, 2);
DHT dht(4, DHT11);
SmsWork sms_device;
iarduino_RTC time(RTC_DS3231);
IRrecv irrecv(12);
decode_results results;

// Инициализируем таймеры
SmartDelay ReadTemperature(1000000UL);
SmartDelay ReadPoliv(1000000UL);

// Задекларируем функции
void LcdPrint(int x, int y, String Str, boolean clear);
void Key();
void Menu();
bool CheckTimeAvtopoliv(int pin, long int timeSetup);
void SetTime(int key);

int MENU_LCD = 0;
boolean g_bMenuReftach = true;
boolean g_bLcdBrigtlight = true;
// Таймер полива
int g_iTemp = 0;
int g_iHumidity = 0;
long int g_iTimeSec1 = 0;
long int g_iTimeSec2 = 0;
long int g_iTimeSec3 = 0;

String g_sTimeSetupSettings1 = "09:00";
String g_sTimeSetupSettings2 = "10:00";
String g_sTimeSetupSettings3 = "11:00";
String g_sTimeSetupSettingsSetup = "";
bool   g_GlobalPoliv = false;

int g_iSetParameters = 0;
int g_iStepSetTime = 0;
String g_sActualTime = "";

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void receivesms(String str)
{
  if (str == "ON")
  {
      digitalWrite(5, HIGH);
      g_iTimeSec1 = -1000;
      digitalWrite(6, HIGH);
      g_iTimeSec2 = -1000;
      digitalWrite(7, HIGH);
      g_iTimeSec3 = -1000;

      g_GlobalPoliv = true;

      g_bMenuReftach = true;

  } 
  else if (str == "OFF")
  {
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);

      g_GlobalPoliv = false;

      g_bMenuReftach = true;

  }
  if(getValue(str, ' ', 1).length() == 4)
    str = "0"+str;
  else if(getValue(str, ' ', 0) == "0")
      g_sTimeSetupSettings1 = getValue(str, ' ', 1);
  else if(getValue(str, ' ', 0) == "1")
      g_sTimeSetupSettings2 = getValue(str, ' ', 1);
  else if(getValue(str, ' ', 0) == "2")
      g_sTimeSetupSettings3 = getValue(str, ' ', 1);

}

void setup()
{
    pinMode(5, OUTPUT); // Полив
    pinMode(6, OUTPUT); // Полив
    pinMode(7, OUTPUT); // Полив

    lcd.begin();
    if (g_bLcdBrigtlight)
    {
        lcd.backlight();
    }
    else
    {
        lcd.noBacklight();
    }
    Serial.begin(9600);
    Serial.println("Start");
    irrecv.enableIRIn(); // Start the receiver
    dht.begin(); // Запускаем датчик
    time.begin();
    //time.settime(0,45,21,21,02,21,7);
    
    sms_device.pt2Func = &receivesms;
}

void loop()
{
    // Время
    if(millis()%1000==0){ // если прошла 1 секунда
      g_sActualTime = time.gettime("H:i"); // Получаем время
      delay(1); // приостанавливаем на 1 мс
    }
    
    Key();
    Menu();
    sms_device.Work();
    if(ReadTemperature.Now()){
        // Сохраняем предыдущую температуру для проверки
        int l_iTemp = g_iTemp;
        int l_iHumidity = g_iHumidity;
        // Считываем температуру
        g_iHumidity = dht.readHumidity(); 
        g_iTemp = dht.readTemperature();
        if((MENU_LCD == MENU_HOME) && (l_iTemp != g_iTemp || l_iHumidity != g_iHumidity)){
            g_bMenuReftach = true;
        }
    }
    // Проверяем нужно ли что ни будь полить
    if(ReadPoliv.Now()){
        CheckTimeAvtopoliv(5, g_sTimeSetupSettings1);
        CheckTimeAvtopoliv(6, g_sTimeSetupSettings2);
        CheckTimeAvtopoliv(7, g_sTimeSetupSettings3);

        if(MENU_LCD == MENU_INFO_POLIV1 || MENU_LCD == MENU_INFO_POLIV2 || MENU_LCD == MENU_INFO_POLIV3){
            g_bMenuReftach = true;
        }
    }
}

bool CheckTimeAvtopoliv(int pin, String timeSetup){
  bool l_bResult = false;
  if(g_GlobalPoliv)
    return l_bResult;
  if(timeSetup == g_sActualTime){
    digitalWrite(pin, HIGH);
    l_bResult = true;
  } else{
    digitalWrite(pin, LOW);
  }
  return l_bResult;
}

void Key()
{
    if (irrecv.decode(&results)) // если данные пришли выполняем команды
    {
        //Serial.println(results.value); // отправляем полученные данные на порт
        switch (MENU_LCD)
        {
        case MENU_HOME:
            if(results.value == KEY_1){
                if(g_iTimeSec1 != -1000){
                    digitalWrite(5, HIGH);
                    g_iTimeSec1 = -1000;
                }
                else{
                    digitalWrite(5, LOW);
                    //g_iTimeSec1 = g_iTimeSetupSettings1;
                }
            }
            if(results.value == KEY_2){
                if(g_iTimeSec2 != -1000){
                    digitalWrite(6, HIGH);
                    g_iTimeSec2 = -1000;
                }
                else{
                    digitalWrite(6, LOW);
                    //g_iTimeSec2 = g_iTimeSetupSettings2;
                }
            }
            if(results.value == KEY_3){
                if(g_iTimeSec3 != -1000){
                    digitalWrite(7, HIGH);
                    g_iTimeSec3 = -1000;
                }
                else{
                    digitalWrite(7, LOW);
                    //g_iTimeSec3 = g_iTimeSetupSettings3;
                }
            }
            if (results.value == KEY_RIGHT)
            {
                MENU_LCD = MENU_SETUP;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_INFO_POLIV1;
            }
            break;
        case MENU_INFO_POLIV1:
            if (results.value == KEY_DOWN)
            {
                MENU_LCD = MENU_INFO_POLIV2;
            }
            if (results.value == KEY_RIGHT)
            {
                MENU_LCD = MENU_HOME;
            }
            break;
        case MENU_INFO_POLIV2:
            if (results.value == KEY_UP)
            {
                MENU_LCD = MENU_INFO_POLIV1;
            }
            if (results.value == KEY_DOWN)
            {
                MENU_LCD = MENU_INFO_POLIV3;
            }
            if (results.value == KEY_RIGHT)
            {
                MENU_LCD = MENU_HOME;
            }
            break;
        case MENU_INFO_POLIV3:
            if (results.value == KEY_UP)
            {
                MENU_LCD = MENU_INFO_POLIV2;
            }
            if (results.value == KEY_RIGHT)
            {
                MENU_LCD = MENU_HOME;
            }
            break;
        case MENU_LCD_BRIGLIGHT:
            if (results.value == KEY_OK)
            {
                g_bLcdBrigtlight = !g_bLcdBrigtlight;
                MENU_LCD = MENU_LCD_BRIGLIGHT;
            }
            if (results.value == KEY_DOWN)
            {
                MENU_LCD = MENU_POLIV1;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_SETUP;
            }
            break;
        case MENU_POLIV1:
            if (results.value == KEY_UP)
            {
                MENU_LCD = MENU_LCD_BRIGLIGHT;
            }
            if (results.value == KEY_DOWN)
            {
                MENU_LCD = MENU_POLIV2;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_SETUP;
            }
            if (results.value == KEY_OK)
            {
                MENU_LCD = MENU_SET_PARAMETERS;
                g_iSetParameters = MENU_POLIV1;
            }
            break;
        case MENU_POLIV2:
            if (results.value == KEY_UP)
            {
                MENU_LCD = MENU_POLIV1;
            }
            if (results.value == KEY_DOWN)
            {
                MENU_LCD = MENU_POLIV3;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_SETUP;
            }
            if (results.value == KEY_OK)
            {
                MENU_LCD = MENU_SET_PARAMETERS;
                g_iSetParameters = MENU_POLIV2;
            }
            break;
        case MENU_POLIV3:
            if (results.value == KEY_UP)
            {
                MENU_LCD = MENU_POLIV2;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_SETUP;
            }
            if (results.value == KEY_OK)
            {
                MENU_LCD = MENU_SET_PARAMETERS;
                g_iSetParameters = MENU_POLIV3;
            }
            break;
        case MENU_SET_PARAMETERS:
            if(results.value == KEY_1){
                SetTime(1);
            }
            if(results.value == KEY_2){
                SetTime(2);
            }
            if(results.value == KEY_3){
                SetTime(3);
            }
            if(results.value == KEY_4){
                SetTime(4);
            }
            if(results.value == KEY_5){
                SetTime(5);
            }
            if(results.value == KEY_6){
                SetTime(6);
            }
            if(results.value == KEY_7){
                SetTime(7);
            }
            if(results.value == KEY_8){
                SetTime(8);
            }
            if(results.value == KEY_9){
                SetTime(9);
            }
            if(results.value == KEY_0){
                SetTime(0);
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = g_iSetParameters;
                g_iStepSetTime = 0;
                g_sTimeSetupSettingsSetup = "";
            }
            if (results.value == KEY_OK)
            {
            }
            break;
        case MENU_SETUP:
            if (results.value == KEY_OK)
            {
                MENU_LCD = MENU_LCD_BRIGLIGHT;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_HOME;
            }
            break;
        }
        if (results.value == KEY_0)
        {
            if(MENU_LCD != MENU_SET_PARAMETERS){
                MENU_LCD = MENU_HOME;
            }
        }
        if (results.value == KEY_Z)
        {
            digitalWrite(5, HIGH);
            g_iTimeSec1 = -1000;
            digitalWrite(6, HIGH);
            g_iTimeSec2 = -1000;
            digitalWrite(7, HIGH);
            g_iTimeSec3 = -1000;
        }
        if (results.value == KEY_R)
        {
            digitalWrite(5, LOW);
            //g_iTimeSec1 = g_iTimeSetupSettings1;
            digitalWrite(6, LOW);
            //g_iTimeSec2 = g_iTimeSetupSettings2;
            digitalWrite(7, LOW);
            //g_iTimeSec3 = g_iTimeSetupSettings3;
        }
        g_bMenuReftach = true;
        irrecv.resume();
    }
}

void SetTime(int key){
    // Проверяем данные
    if(g_iStepSetTime == 0 && key > 2)
        return;
    else if(g_iStepSetTime == 1){
        if((g_sTimeSetupSettingsSetup + String(key)).substring(0, 2).toInt() >= 24)
            return;
    } else if(g_iStepSetTime == 2 && key > 5)
        return;
    else if(g_iStepSetTime == 3){
        if((g_sTimeSetupSettingsSetup + ":" + String(key)).substring(3, 5).toInt() >= 60)
            return;
    }

    if(g_iStepSetTime != 2)
        g_sTimeSetupSettingsSetup += String(key);
    else 
        g_sTimeSetupSettingsSetup += ":" + String(key);
    
    g_iStepSetTime++;

    if(g_iStepSetTime > 3){
        g_iStepSetTime = 0;

        switch (g_iSetParameters)
            {
            case MENU_POLIV1:
                g_sTimeSetupSettings1 = g_sTimeSetupSettingsSetup;
                break;
            case MENU_POLIV2:
                g_sTimeSetupSettings2 = g_sTimeSetupSettingsSetup;
                break;
            case MENU_POLIV3:
                g_sTimeSetupSettings3 = g_sTimeSetupSettingsSetup;
                break;
        }
    }
}

void SetLcdTime(String time){
    lcd.print(time);
    if(MENU_LCD == MENU_SET_PARAMETERS){
        lcd.setCursor(0,1);
        switch (g_iStepSetTime)
        {
            case 0:
                lcd.print("Часы *_:__");
                break;
            case 1:
                lcd.print("Часы -*:__");
                break;
            case 2:
                lcd.print("Минуты --:*_");
                break;
            case 3:
                lcd.print("Минуты --:-*");
                break;
        }
    }
}

void Menu()
{
    if (g_bMenuReftach)
    {
        switch (MENU_LCD)
        {
        case MENU_HOME:
            LcdPrint(0, 0, "Температура: ", true);
            lcd.print(g_iTemp);
            lcd.print("С");
            lcd.setCursor(0, 1);
            lcd.print("Влажность:   ");
            lcd.print(g_iHumidity);
            lcd.print("%");
            break;
        case MENU_SETUP:
            LcdPrint(0, 0, "Настройки", true);
            break;
        case MENU_POLIV1:
            LcdPrint(0, 0, "Огурцы", true);
            break;
        case MENU_POLIV2:
            LcdPrint(0, 0, "Помидоры", true);
            break;
        case MENU_POLIV3:
            LcdPrint(0, 0, "Перец", true);
            break;
        case MENU_INFO_POLIV1:
            LcdPrint(0, 0, "Огурцы: ", true);
            SetLcdTime(g_sTimeSetupSettings1);
            LcdPrint(0, 1, "Время: " + g_sActualTime, false);
            break;
        case MENU_INFO_POLIV2:
            LcdPrint(0, 0, "Помидоры: ", true);
            SetLcdTime(g_sTimeSetupSettings2);
            LcdPrint(0, 1, "Время: " + g_sActualTime, false);
            break;
        case MENU_INFO_POLIV3:
            LcdPrint(0, 0, "Перец: ", true);
            SetLcdTime(g_sTimeSetupSettings3);
            LcdPrint(0, 1, "Время: " + g_sActualTime, false);
            break;
        case MENU_SET_PARAMETERS:
            switch (g_iSetParameters)
            {
            case MENU_POLIV1:
                LcdPrint(0, 0, "Огурцы: ", true);
                SetLcdTime(g_sTimeSetupSettingsSetup);
                break;
            case MENU_POLIV2:
                LcdPrint(0, 0, "Помидоры: ", true);
                SetLcdTime(g_sTimeSetupSettingsSetup);
                break;
            case MENU_POLIV3:
                LcdPrint(0, 0, "Перец: ", true);
                SetLcdTime(g_sTimeSetupSettingsSetup);
                break;
            }
            break;
        case MENU_LCD_BRIGLIGHT:
            if (g_bLcdBrigtlight)
            {
                LcdPrint(0, 0, "Подсветка ВЫКЛ", true);
                lcd.backlight();
            }
            else
            {
                LcdPrint(0, 0, "Подсветка ВКЛ", true);
                lcd.noBacklight();
            }
            break;
        }
    }
    g_bMenuReftach = false;
}

void LcdPrint(int x, int y, String Str, boolean clear)
{
    if(clear){
        lcd.clear();
    }
    lcd.setCursor(x, y);
    lcd.print(Str);
}

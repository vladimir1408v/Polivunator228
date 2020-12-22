#include "DHT.h"
#include <LCD_1602_RUS.h>
#include <IRremote.h>
#include <SmartDelay.h>
#include <GSM.h>


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

LCD_1602_RUS lcd(0x27, 16, 2);
DHT dht(4, DHT11);

SmartDelay ReadTemperature(1000000UL);
SmartDelay ReadPoliv(1000000UL);

int hTime = 0;
int mTime = 0;
unsigned long int sTime = 0;

// Задекларируем функции
void LcdPrint(int x, int y, String Str, boolean clear);
void Key();
void Menu();
int CheckAvtopoliv(int g_iTimeSec, int pin, int timeSetup);
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
long int g_iTimeSetupSettings1 = 10;
long int g_iTimeSetupSettings2 = 120;
long int g_iTimeSetupSettings3 = 1200;
int g_iSetParameters = 0;
int g_iStepSetTime = 0;


IRrecv irrecv(A0);
decode_results results;

void setup()
{
    g_iTimeSec1 = g_iTimeSetupSettings1;
    g_iTimeSec2 = g_iTimeSetupSettings2;
    g_iTimeSec3 = g_iTimeSetupSettings3;
    pinMode(5, OUTPUT); // Полив
    pinMode(6, OUTPUT); // Полив
    pinMode(7, OUTPUT); // Полив

    lcd.init();
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
}

void loop()
{
    Key();
    Menu();
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
        //Serial.println(g_iTimeSec1);
        //Serial.println(g_iTimeSec2);
        //Serial.println(g_iTimeSec3);

        g_iTimeSec1 = CheckAvtopoliv(g_iTimeSec1, 5, g_iTimeSetupSettings1);
        g_iTimeSec2 = CheckAvtopoliv(g_iTimeSec2, 6, g_iTimeSetupSettings2);
        g_iTimeSec3 = CheckAvtopoliv(g_iTimeSec3, 7, g_iTimeSetupSettings3);

        if(MENU_LCD == MENU_INFO_POLIV1 || MENU_LCD == MENU_INFO_POLIV2 || MENU_LCD == MENU_INFO_POLIV3){
            g_bMenuReftach = true;
        }
    }
}

long int CheckAvtopoliv(long int g_iTimeSec, int pin, long int timeSetup){
        // Декрементируем таймер если включен автополив 
        if(g_iTimeSec != -1000){
            g_iTimeSec--;
        }
        if(g_iTimeSec == 0){
            digitalWrite(pin, HIGH);
            return 0;
        }
        if(g_iTimeSec == -5){
            digitalWrite(pin, LOW);
            return timeSetup;
        }
        return g_iTimeSec;
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
                    g_iTimeSec1 = g_iTimeSetupSettings1;
                }
            }
            if(results.value == KEY_2){
                if(g_iTimeSec2 != -1000){
                    digitalWrite(6, HIGH);
                    g_iTimeSec2 = -1000;
                }
                else{
                    digitalWrite(6, LOW);
                    g_iTimeSec2 = g_iTimeSetupSettings2;
                }
            }
            if(results.value == KEY_3){
                if(g_iTimeSec3 != -1000){
                    digitalWrite(7, HIGH);
                    g_iTimeSec3 = -1000;
                }
                else{
                    digitalWrite(7, LOW);
                    g_iTimeSec3 = g_iTimeSetupSettings3;
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
            g_iTimeSec1 = g_iTimeSetupSettings1;
            digitalWrite(6, LOW);
            g_iTimeSec2 = g_iTimeSetupSettings2;
            digitalWrite(7, LOW);
            g_iTimeSec3 = g_iTimeSetupSettings3;
        }
        g_bMenuReftach = true;
        irrecv.resume();
    }
}

void SetTime(int key){
    switch (g_iStepSetTime)
    {
        case 0:
            if(key > 4){
                return;
            }
            hTime = key*10;
            break;
        case 1:
            hTime = hTime + key;
            break;
        case 2:
            if(key > 5){
                return;
            }
            mTime = key*10;
            break;
        case 3:
            mTime = mTime+key;
            break;
    }
    sTime = ((hTime*60)+mTime);
    sTime = sTime*60;
    Serial.println("=================");
    Serial.println(g_iStepSetTime);
    Serial.println(hTime);
    Serial.println(mTime);
    Serial.println(sTime);
    switch (g_iSetParameters)
        {
        case MENU_POLIV1:
            g_iTimeSetupSettings1 = sTime;
            g_iTimeSec1 = sTime;
            break;
        case MENU_POLIV2:
            g_iTimeSetupSettings2 = sTime;
            g_iTimeSec2 = sTime;
            break;
        case MENU_POLIV3:
            g_iTimeSetupSettings3 = sTime;
            g_iTimeSec3 = sTime;
            break;
    }
    g_iStepSetTime++;
    if(g_iStepSetTime > 3){
        g_iStepSetTime = 0;
    }
}

void SetLcdTime(long int time){
    if((time < 0) && ((MENU_LCD == MENU_INFO_POLIV1) || (MENU_LCD == MENU_INFO_POLIV2) || (MENU_LCD == MENU_INFO_POLIV3))){
        lcd.print("полив!");
        return;
    }
    unsigned long int s,m,h=0;
    s = time;
    h = s / 3600;
    m = (s - h * 3600) / 60;
    s = s - h * 3600 - m * 60;
    if(h){
        lcd.print(h);
        lcd.print("ч");
    }
    if(m){
        lcd.print(m);
        lcd.print("м");
    }
    if(MENU_LCD != MENU_SET_PARAMETERS){
        lcd.print(s);
        lcd.print("с");
    }
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
            SetLcdTime(g_iTimeSec1);
            break;
        case MENU_INFO_POLIV2:
            LcdPrint(0, 0, "Помидоры: ", true);
            SetLcdTime(g_iTimeSec2);
            break;
        case MENU_INFO_POLIV3:
            LcdPrint(0, 0, "Перец: ", true);
            SetLcdTime(g_iTimeSec3);
            break;
        case MENU_SET_PARAMETERS:
            switch (g_iSetParameters)
            {
            case MENU_POLIV1:
                LcdPrint(0, 0, "Огурцы: ", true);
                SetLcdTime(g_iTimeSetupSettings1);
                break;
            case MENU_POLIV2:
                LcdPrint(0, 0, "Помидоры: ", true);
                SetLcdTime(g_iTimeSetupSettings2);
                break;
            case MENU_POLIV3:
                LcdPrint(0, 0, "Перец: ", true);
                SetLcdTime(g_iTimeSetupSettings3);
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
    lcd.setCursor(0, 0);
    lcd.print(Str);
}
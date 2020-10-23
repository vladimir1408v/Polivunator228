#include "DHT.h"
#include <LCD_1602_RUS.h>
#include <IRremote.h>

#define KEY_OK 16726215
#define KEY_LEFT 16716015
#define KEY_RIGHT 16734885
#define KEY_UP 16718055
#define KEY_DOWN 16730805

#define MENU_HOME 0
#define MENU_SETUP 1
#define MENU_LCD_BRIGLIGHT 2
#define MENU_POLIV 3

LCD_1602_RUS lcd(0x27, 16, 2);
DHT dht(4, DHT11);

void LcdPrint(int x, int y, String Str);
void Key();
void Menu();

int MENU_LCD = 0;
boolean g_bMenuReftach = true;
boolean g_bLcdBrigtlight = true;

int g_iTemp = 0;

IRrecv irrecv(A0);
decode_results results;

void setup()
{
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
    
    //delay(2000);
    float h = dht.readHumidity(); 
    // Считываем температуру
    int l_iTemp = dht.readTemperature();
    if(l_iTemp != g_iTemp){
        g_iTemp = l_iTemp;
        g_bMenuReftach = true;
    }
}

void Key()
{
    if (irrecv.decode(&results)) // если данные пришли выполняем команды
    {
        //Serial.println(results.value); // отправляем полученные данные на порт
        switch (MENU_LCD)
        {
        case MENU_HOME:
            if (results.value == KEY_RIGHT)
            {
                MENU_LCD = MENU_SETUP;
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
                MENU_LCD = MENU_POLIV;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_SETUP;
            }
            break;
        case MENU_POLIV:
            if (results.value == KEY_UP)
            {
                MENU_LCD = MENU_LCD_BRIGLIGHT;
            }
            if (results.value == KEY_LEFT)
            {
                MENU_LCD = MENU_SETUP;
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
        g_bMenuReftach = true;
        irrecv.resume();
    }
}

void Menu()
{
    if (g_bMenuReftach)
    {
        switch (MENU_LCD)
        {
        case MENU_HOME:
            LcdPrint(0, 0, "Температура: ");
            lcd.print(g_iTemp);
            break;
        case MENU_SETUP:
            LcdPrint(0, 0, "Настройки");
            break;
        case MENU_POLIV:
            LcdPrint(0, 0, "Полив АВТ.");
            break;
        case MENU_LCD_BRIGLIGHT:
            if (g_bLcdBrigtlight)
            {
                LcdPrint(0, 0, "Подсветка ВЫКЛ");
                lcd.backlight();
            }
            else
            {
                LcdPrint(0, 0, "Подсветка ВКЛ");
                lcd.noBacklight();
            }
            break;
        }
    }
    g_bMenuReftach = false;
}

void LcdPrint(int x, int y, String Str)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Str);
}
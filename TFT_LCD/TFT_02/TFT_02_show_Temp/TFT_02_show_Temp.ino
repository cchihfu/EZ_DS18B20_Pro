#include <SimpleDs18b20.h>
#include <EasyTransfer.h> //https://github.com/madsci1016/Arduino-EasyTransfer
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h> 

char temperatureChar[10];
//Arduino Uno/Nano Pins:
//GND = GND
//VCC = 5v
//DC = 8
//RES = 9
//CS = 10
//SDA = 11
//SCL = 13 

#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); 
SimpleDs18b20 ds01(7);
void setup(void)
{
  Serial.begin(9600);
  tft.initR(INITR_GREENTAB); //Initializes ST7735.
  tft.fillScreen(ST7735_BLACK); 
  printUI();
  /***********************************************************************
  //Use of GREENTAB rather than BLACKTAB avoids static at the bottom and right-hand side of the display.
  *設定顏色參數
  *設定參數->實際顏色
  1.BLACK   ->黑
  2.BLUE    ->Red(紅）
  3.RED     ->Blue(藍)
  4.GREEN   ->綠
  5.CYAN    ->Yellow(黃)
  6.MAGENTA ->粉紅
  7.YELLOW  ->CYAN（青）
  8.WHITE   ->白
  **************************************************************************/
 }

void loop()
{
    String temperatureString = String(ds01.GetTemperature(),2);
    temperatureString.toCharArray(temperatureChar,10);
    tft.fillRect(5,20,90,34,ST7735_BLACK);
    printText(temperatureChar, ST7735_RED,5,26,3);
  
   delay(2000);
}
void printText(char *text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}
void printUI()
{
    printText("TEMPERATURE", ST7735_MAGENTA,30,5,1);  // Temperature Static Text
    printText("o", ST7735_CYAN,97,19,2);
    printText("C", ST7735_YELLOW,110,26,3);
}

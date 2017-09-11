


// adapted sketch by niq_ro from http://nicuflorica.blogspot.ro
// version 3.0

//#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

//#include <radio.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif


// Color definitions
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5
#define DARKGREEN       0x03E0

uint8_t errorCode = 0;
char buffer[30];
unsigned int RDS[4];
char seg_RDS[8];
char seg_RDS1[64];
char indexRDS1;
 int mezcla;
//radio.debugEnable();


#define __CS 10
#define __DC 8


TFT_ILI9163C display = TFT_ILI9163C(__CS, __DC, 9);

#include <TEA5767.h>
// from https://github.com/andykarpov/TEA5767
#include <Wire.h>
#include <Button.h>
// from http://arduino-info.wikispaces.com/file/view/Button.zip/405390486/Button.zip



TEA5767 Radio;
double old_frequency;
double frequency;
int search_mode = 0;
int search_direction;
int mute =0;
unsigned long last_pressed;

Button btn_forward(7, PULLUP);
Button btn_backward(6, PULLUP);
//Button btn_mute(4, PULLUP);

byte f1, f2, f3, f4;  // number for each zone of frequency
double frecventa;
int frecventa10;      // frequency x 10 

#include <EEPROM.h>
//http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/


void setup () {


// read value of last frequency
f1 = EEPROM.read(101);
f2 = EEPROM.read(102);
f3 = EEPROM.read(103);
f4 = EEPROM.read(104);

// recover number
frecventa = 100*f1 + 10*f2 + f3 + 0.1*f4;

  
  Wire.begin();
  Radio.init();
//  Radio.set_frequency(104.5); 
   Radio.set_frequency(frecventa); 
  Serial.begin(9600);

  display.begin();
  

  display.fillScreen();


  // Print a logo message to the LCD.
  display.setRotation(1);
  display.fillScreen();
  display.fillScreen(RED);
  display.fillScreen(GREEN);
  display.fillScreen(BLUE);
  display.fillScreen();
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,0);
  display.println("Adaptat Vlad Gheorghe");
  
  display.setTextSize(2);
  display.setCursor(1, 16);
  display.setTextColor(WHITE, BLACK);
  display.print("FM");  
  display.setTextSize(1);
  display.setCursor(30, 20);
  display.setTextColor(WHITE, BLACK);
  display.print(" radio ");  
  display.setCursor(0, 32);
  display.setTextColor(BLACK);
  display.print("with TEA5767");
  display.setCursor(0, 40);
  display.print("version ");
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 40);
  display.print("versiunea 3.2");
display.setCursor(0, 55);
  display.print("cu afisare nume canal");
//  display.display();
  delay (3000);
   display.fillScreen(RED);
  display.fillScreen(GREEN);
  display.fillScreen(BLUE);
  display.fillScreen(CYAN);
  display.fillScreen(MAGENTA);
  display.fillScreen(YELLOW);
 // display.fillScreen();
  display.fillScreen();

}

void loop () {

  unsigned char buf[5];
  int stereo;
  int signal_level;
  double current_freq;
  unsigned long current_millis = millis();
  
  if (Radio.read_status(buf) == 1) {
    current_freq =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
    stereo = Radio.stereo(buf);
    signal_level = Radio.signal_level(buf);
    display.setCursor(0,0);
   display.setTextSize(2);
    display.setTextColor(CYAN,BLACK);
    display.print("FM ");
    display.setTextSize(1);
    display.setCursor(30,5);
    display.print("Radio cu TEA5767");
    display.setTextColor(BLUE);
    display.setCursor(0,17);
    display.print("Adaptat Vlad Gheorghe");
   display.setTextSize(4);
   display.setTextColor(ORANGE,BLACK);
   display.setCursor(0,30);
   if (current_freq < 100) display.print(" ");
   display.print(current_freq,1);
   display.setTextSize(2);
 display.setCursor(91,63); display.setTextColor(RED,BLACK);
   display.print("MHz");
   display.setCursor(15,70);
   display.setTextSize(1);
   display.setTextColor(GREENYELLOW,BLACK);
   if (stereo) display.print("Stereo");  else display.print("Mono  ");
     display.setTextColor(BLACK,YELLOW);
   // display level of FM signal..
  display.setCursor(0,115);
   display.setTextSize(1);
   display.setTextColor(GREEN, BLACK);
   if (signal_level<10) display.print(" ");
    display.print("Level: ");
   display.print(signal_level);display.setTextColor(GREEN, BLACK);
   display.print("/15");
   printpost(current_freq);

  
  
  
  // display.display();
   delay (50);
//display.fillScreen();

/*
// draw a signal level triangle...
   display.drawLine(80, 30, 80, 45, BLACK);
   display.drawLine(80, 45, 50, 45, BLACK);
  display.drawLine(50, 45, 80, 30, BLACK);
*/
/*
int sl = signal_level;
   for (int x = 0; x < sl; x++)
   { 
    //display.clearScreen();
    
   display.drawLine(80+3*x, 120, 80+3*x, 120-x, GREEN);
  // display.display();
   }
*/
show_signal_level(signal_level);
// draw an antenna
   display.drawLine(95, 82, 95, 95, YELLOW);
   display.drawLine(96, 82, 96, 95, YELLOW);
   display.drawLine(92, 82, 95, 86, YELLOW);
   display.drawLine(91, 82, 95, 87, YELLOW);
   display.drawLine(99, 82, 96, 86, YELLOW);
   display.drawLine(100, 82, 96, 87, YELLOW);

  }

  if (search_mode == 1) {
      if (Radio.process_search (buf, search_direction) == 1) {
          search_mode = 0;
      }
           if (Radio.read_status(buf) == 1) {  
      frecventa =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
      frecventa10 = 10*frecventa;
 f1 = frecventa10/1000;
 frecventa10 = frecventa10 - 1000*f1;
 f2 = frecventa10/100;
 frecventa10 = frecventa10 - 100*f2;
 f3 = frecventa10/10;
 f4 = frecventa10 - 10*f3;
EEPROM.write(101,f1);
EEPROM.write(102,f2);
EEPROM.write(103,f3);
EEPROM.write(104,f4);
frecventa = 100*f1 + 10*f2 + f3 + 0.1*f4;
Radio.set_frequency(frecventa);
      }

  }
  
  if (btn_forward.isPressed()) {
    last_pressed = current_millis;
    search_mode = 1;
    search_direction = TEA5767_SEARCH_DIR_UP;
    Radio.search_up(buf);
    delay(500);
  }
    
  if (btn_backward.isPressed()) {
    last_pressed = current_millis;
    search_mode = 1;
    search_direction = TEA5767_SEARCH_DIR_DOWN;
    Radio.search_down(buf);
    delay(500);
  } 
   delay(500);
  
}
void show_signal_level(int level){
  
  byte xs=98;
  byte ys=120;
  for(int i=0;i<15;i++){
    if(i%2!=0)display.drawLine(xs+i,ys,xs+i,ys-i,level>=i?WHITE:BLACK);
  }
}
void printpost(double current_freq)
{
 // switch(current_freq)
  {
    if (current_freq==101.1) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Actualitati");}
   
    if (current_freq==103.1) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Cultural   ");}
   
    if (current_freq==105.0) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Viva       ");}
   
    if (current_freq==106.5) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Europa FM  ");}
   
    
if (current_freq==88.4) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Impact FM   ");}
   
   if (current_freq==91.1) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Magic FM   ");}
   
   if (current_freq==91.9) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("KISS FM     ");}
   
   if (current_freq==94.9) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("Radio HIT   ");}
   
   if (current_freq==96.3) { display.setCursor(5,90);
   display.setTextSize(1);
   
   display.setTextColor(YELLOW,BLACK);display.print("Radio Iasi  ");}
   
   if (current_freq==97.9) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("RFI          ");}
   
   if (current_freq==99.2) { display.setCursor(5,90);
   display.setTextSize(1);
   display.setTextColor(YELLOW,BLACK);display.print("DIGI FM      ");}
   
   
    
 
 } 
}


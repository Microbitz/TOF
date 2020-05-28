#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

const int WIDTH = 128;
const int HEIGHT = 64;
const int LENGTH = WIDTH;
int Medida;
int x;
int y[LENGTH];
bool Grafico = false;
const int buttonPin = 6;  
int buttonState = 0;  

//funciones de Pantalla.........................................................

void clearY() {
  for (int i = 0; i < LENGTH; i++) {
    y[i] = -1;
  }
}

void drawY() {
  u8g.drawPixel(0, y[0]);
  for (int i = 1; i < LENGTH; i++) {
    if (y[i] != -1) {
      u8g.drawLine(i - 1, y[i - 1], i, y[i]);
    } else {
      break;
    }
  }
}

void draw(void) {
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 20);
  u8g.print("Longitud (MM):");
  u8g.setFont(u8g_font_fur25);
  u8g.setPrintPos(30, 60);
  u8g.print(Medida);
}

//.................................................................................

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous(1);

  //Boton..........................................................................
  pinMode(5, OUTPUT);
  pinMode(buttonPin, INPUT);

  
 //Configuracion de Pantalla.........................................................
  x = 0;
  clearY();
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
  //..................................................................................
}

void loop()
{
  //Arranco sensor....................................................................
  Medida = sensor.readRangeContinuousMillimeters();
  Serial.print(Medida);
  if (sensor.timeoutOccurred()) {
    Serial.print(" TIMEOUT");
  }
  Serial.println();
  if (sensor.readRangeContinuousMillimeters() < 7500) {
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(5, LOW);
  }

  //Pantalla_OLED.....................................................................

  if (Medida > 320 ) Medida = 320;

  if (Grafico == true) {
    y[x] = map(Medida, 0, 320, HEIGHT - 1, 0);;
    u8g.firstPage();
    do {
      drawY();
    } while ( u8g.nextPage() );
    x++;
    if (x >= WIDTH) {
      x = 0;
      clearY();
    }
  }

  if (Grafico == false) {
    u8g.firstPage();
    do {
      draw();
    } while ( u8g.nextPage() );
  }

  //Boton.........................................................

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Grafico = !Grafico;
    delay(1000);
    clearY();
  }
}

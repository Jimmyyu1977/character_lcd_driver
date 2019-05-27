//******************************************************************************
// Ez a "charlcd" LCD kijelz� kurzorhaszn�lati lehet�s�g�t mutatja be.
// k�sz�t�: Roberto Benjami (robertodebenjami(at)gmail(dot)com)
//
// M�k�d�se:
// - A kijelz�n a karakterk�szletet visszafel� g�rd�ti, mik�zben a kurzort el�re viszi.
//   A kurzort e k�zben ki-be kapcsolgatja, �s a form�j�t is v�ltogatja.
//
// charlcd.h sz�ks�ges be�ll�t�sai:
// - egyszeri friss�t�si �zemm�d (LCDMODEONCEBUSY vagy LCDMODEONCEDELAY vagy LCDMODEONCEIRQ)
// - kurzor (LCDCURSOR be)
// k�sz�t�: Roberto Benjami (robertodebenjami(at)gmail(dot)com)
//******************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/ina90.h>
#include <string.h>
#include <stdlib.h>
#include "charlcd.h"

// led l�bak defini�l�sa (ha nincs, t�r�lj�k a LEDS defin�ci�t)
#define  LEDS
#define  LED1ON     PORTB |=  (1 << 5)
#define  LED1OFF    PORTB &= ~(1 << 5)
#define  LED1IO     DDRB  |=  (1 << 5)
#define  LED1       (PORTB & (1 << 5))

//******************************************************************************
void mwait(void)
{
  #define DELAYNUM (CpuClock/1000/14)
  static unsigned char cc;
  volatile unsigned int dh, dl;

  #ifdef LEDS
  if(LED1) LED1ON; else LED1OFF;
  #endif

  dh = 1000;
  while(dh--)
  {
    dl = DELAYNUM;
    while(dl--);
  }

  LcdText[0]++;
  for(cc = 0; cc < sizeof(LcdText) - 2; cc++)
    LcdText[cc + 1] = LcdText[cc] + 1;
}

//******************************************************************************
int main(void)
{
  unsigned char c;
  #ifdef LEDS
  LED1IO;
  #endif

  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t

  while(1)
  {
	LcdRefreshAll();
    mwait();
    LcdCursorPos++;
    LcdCursorOff();

    LcdRefreshAll();
    mwait();
    LcdCursorPos++;
    LcdCursorOn();

    LcdRefreshAll();
    mwait();
    LcdCursorPos++;
	LcdCursorBlink();

    LcdRefreshAll();
    mwait();
    LcdCursorPos++;
	LcdCursorUnBlink();

	if(LcdCursorPos >= sizeof(LcdText) - 2) LcdCursorPos = 0;
  }
}

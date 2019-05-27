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

#include <p18cxxx.h>
#include <string.h>
#include <stdlib.h>
#include <timers.h>
#include "charlcd.h"
#include "bootloaders.h"

// led l�bak defini�l�sa (ha nincs, t�r�lj�k a LEDS defin�ci�t)
#define  LEDS
#define LED1       B, 0
#define LED2       B, 1

#define GLUE(a, b)   a##b
#define SET_(what, p, m)       GLUE(what, p) |= (1 << (m))
#define CLR_(what, p, m)       GLUE(what, p) &= ~(1 << (m))
#define GET_(/* PORT, */ p, m) GLUE(PORT, p) & (1 << (m))
#define IOIN(x)                SET_(TRIS, x)
#define IOOUT(x)               CLR_(TRIS, x)
#define SET(x)                 SET_(LAT, x)
#define CLR(x)                 CLR_(LAT, x)
#define GET(/* PORT, */ x)     GET_(x)

//******************************************************************************
void mwait(void)
{
  #define CpuClock 4000000
  #define DELAYNUM (CpuClock/1000/14)
  static unsigned char cc;
  volatile unsigned int dh, dl;

  #ifdef  LEDS
  if(GET(LED1)) CLR(LED1); else SET(LED2);
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

void main(void)
{
  unsigned char c;

  // ha sz�ks�g van AD l�b digit�lis I/O l�bra t�rt�n� �tkapcsol�sra
  #ifdef InitAD
  InitAD();
  #endif

  // ledek be�ll�t�sa
  #ifdef  LEDS
  CLR(LED1); CLR(LED2); IOOUT(LED1); IOOUT(LED2);
  #endif

  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t

  // megszak�t�s be�ll�t�sok
  RCONbits.IPEN = 1;                    // k�tszint� IRQ eng
  INTCONbits.GIEL = 1;                  // glob�lis LO IRQ eng
  INTCONbits.GIEH = 1;                  // glob�lis IRQ eng

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

#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode()
{
}

#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode()
{
  #ifdef LCDTIMERINT
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

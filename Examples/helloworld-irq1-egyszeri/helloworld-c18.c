//******************************************************************************
// 8 bites 18Fxxx helloworld demo az charlcd tesztel�s�hez
// k�sz�t�: Roberto Benjami (robertodebenjami(at)gmail(dot)com)
//
// charlcd.h sz�ks�ges be�ll�t�sai:
// - LCDMODEONCEIRQ �zemm�d
// - a t�bbi be�ll�t�s b�rmi lehet, csak a felhaszn�lt mikrovez�rl� is t�mogassa
//   (pl.a be�ll�tott I/O l�b �s a TIMER l�tezzen)
// - adott esetben az I/O l�bak anal�gr�l -> digit�lisra t�rt�n� �t�ll�t�s�r�l gondoskodjunk!
// - k�t kijelz� (LCDSTEREO) haszn�lata eset�n, csak az els� kijelz�n fog megjelenni az felirat
//   (ha hosszabb sz�veget �runk az LcdText-be, akkor a m�sodik kijelz�re is �t fog l�gni)
//******************************************************************************

#include <p18cxxx.h>
#include <string.h>
#include "charlcd.h"

void YourHighPriorityISRCode(void);
void YourLowPriorityISRCode(void);

void main(void)
{
  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t
  //                             "01020304050607080910111213141516"
  memcpypgm2ram((char *)LcdText, "   Hello world                  ", 32); // kezd�k�perny� felt�lt�se
  LcdRefreshAll();
  while(1)
  {
  }
}

#pragma code high_vector_section = 0x8
void InterruptVectorHigh(void)
{
  _asm goto YourHighPriorityISRCode _endasm
}

#pragma code low_vector_section = 0x18
void InterruptVectorLow(void)
{
  _asm goto YourLowPriorityISRCode _endasm
}

#pragma code

#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode(void)
{
}

#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode(void)
{
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
}

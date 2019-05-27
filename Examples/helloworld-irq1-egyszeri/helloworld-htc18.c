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

#include <htc.h>
#include <string.h>
#include "charlcd.h"

void main(void)
{
  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t
  //                      "01020304050607080910111213141516"
  memcpy((char *)LcdText, "   Hello world                  ", 32); // kezd�k�perny� felt�lt�se
  LcdRefreshAll();
  while(1)
  {
  }
}

#if LCDTIMERPR18 == 0
void interrupt low_priority irql(void)
{
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
}
#endif

#if (LCDTIMERPR18 == -1) || (LCDTIMERPR18 == 1)
void interrupt irqh(void)
{
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
}
#endif

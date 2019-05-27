//******************************************************************************
// LCD kijelz� sebess�g�t m�r� program
// k�sz�t�: Roberto Benjami (robertodebenjami(at)gmail(dot)com)
//
// M�k�d�se:
// - TIMER1 seg�ts�g�vel megm�ri mennyi ideig tart az LCD friss�t�se BUSY flag figyel�ses �zemm�dban.
//   Ki�rja az ebb�l ad�d� k�pkocka/m�sodperc (FPS) �s a karakter/m�sodperc (CPS) �rt�k�t.
//
// charlcd.h sz�ks�ges be�ll�t�sai:
// - Egyszeri friss�t�si �zemm�d megszak�t�s n�lk�l, BUSY flag figyel�ssel (LCDMODEONCEBUSY)
//******************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "charlcd.h"

#if (1UL * LCDWIDTH * LCDLINES < 80)
#define  FPSPOZ     0
#define  CPSPOZ     16
#else  // (1UL * LCDWIDTH * LCDLINES) < 80
#define  FPSPOZ     82
#define  CPSPOZ     98
#endif // else (1UL * LCDWIDTH * LCDLINES) < 80

#ifndef LCDMODEONCEBUSY
//#error "Rossz be�ll�t�sok az lcdchar.h-ban"
#endif

//******************************************************************************
unsigned int t1cycle;                   // 16 bites TIMER1 �rt�ke ide ker�l

//----------------------------------------------------------------------------
// TIMER 1 �r�sa, olvas�sa (param�ter: �r�s konstans vagy 16bites v�ltoz�, olvas�s 16bites v�ltoz�)
#define SETTIMER1(t1)   {TCNT1H = t1 >> 8; TCNT1L = t1;}
#define GETTIMER1(t1)   {t1 = TCNT1L; t1 += TCNT1H << 8;}

//----------------------------------------------------------------------------
#define DELAYNUM (CpuClock/1000/14)
void delaymms(unsigned char ms)
{
  volatile unsigned int dm;

  while(ms--)
  {
    dm = DELAYNUM;
    while(dm--);
  }
}

//----------------------------------------------------------------------------
void lcdspeedscreen(void)
{
  static unsigned long ul;
  if(!(TIFR1 & (1 << TOV1)))
  {
    memcpy((char *)LcdText + FPSPOZ, "FPS:      ", 10);
    memcpy((char *)LcdText + CPSPOZ, "CPS:      ", 10);
    ul = (SystemClock/64UL) / ((unsigned long)t1cycle);
    ultoa(ul, (char *)LcdText +  FPSPOZ + 4, 10);
    ultoa(ul * (1UL * LCDLINES * LCDWIDTH), (char *)LcdText +  CPSPOZ + 4, 10);
  }
  else
  {
    memcpy((char *)LcdText + FPSPOZ, "FPS: ovf  ", 10);
    memcpy((char *)LcdText + CPSPOZ, "CPS: ovf  ", 10);
    TIFR1 &= ~(1 << TOV1);
  }
}

//----------------------------------------------------------------------------
void init(void)
{
  // 1. id�z�t� inicializ�l�sa
  // 64-es oszt�s:
  #define TMCLKSEL         3
  TCCR1B = (TMCLKSEL<<CS10);            // Clk = ClkIO/1..8..64..256..1024
}

//******************************************************************************
int main(void)
{
  // ha sz�ks�g van AD l�b digit�lis I/O l�bra t�rt�n� �tkapcsol�sra
  #ifdef InitAD
  InitAD();
  #endif

  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t

  #if (1UL * LCDWIDTH * LCDLINES) < 80 
  // 2x16 karakteres -------------------
  //                         "01020304050607080910111213141516"
  // memcpy((char *)LcdText, " LCD CPU used       meter       ", 32); // kezd�k�perny� felt�lt�se
  #else  // (1UL * LCDWIDTH * LCDLINES) < 80
  // 4x40 karakteres -------------------
  //                                   "0204060810121416182022242628303234363840"
  memcpy((char *)LcdText +   0, "****************************************", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText +  40, "*            LCD speed meter           *", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText +  80, "*                                      *", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText + 120, "****************************************", 40); // kezd�k�perny� felt�lt�se
  #endif // else (1UL * LCDWIDTH * LCDLINES) < 80

  init();

  while(1)
  {
    // m�r�s ---------------------------
    SETTIMER1(0);
    TIFR1 &= ~(1 << TOV1);
    //T1CONbits.TMR1ON = 1;               // bekapcs
    LcdRefreshAll();
    //T1CONbits.TMR1ON = 0;               // kikapcs
    GETTIMER1(t1cycle);

    // megjelen�t�s -----------------------
    lcdspeedscreen();
	delaymms(100);                        // 100ms v�rakoz�s
  }
}

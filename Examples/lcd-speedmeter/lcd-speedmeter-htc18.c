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

#include <htc.h>
#include <string.h>
#include <stdlib.h>
#include "GenericTypeDefs.h"
#include "charlcd.h"

#if (1UL * LCDWIDTH * LCDLINES < 80)
#define  FPSPOZ     0
#define  CPSPOZ     16
#else  // (1UL * LCDWIDTH * LCDLINES) < 80
#define  FPSPOZ     82
#define  CPSPOZ     98
#endif // else (1UL * LCDWIDTH * LCDLINES) < 80

#ifndef LCDMODEONCEBUSY
#error "Rossz be�ll�t�sok az lcdchar.h-ban"
#endif

//******************************************************************************
unsigned int t1cycle;                   // 16 bites TIMER1 �rt�ke ide ker�l

//----------------------------------------------------------------------------
// TIMER 1 �r�sa, olvas�sa (param�ter: �r�s konstans vagy 16bites v�ltoz�, olvas�s 16bites v�ltoz�)
#define SETTIMER1(t1)   {TMR1H = t1 >> 8; TMR1L = t1;}
#define CPTIMER1(t1)    {TMR1H = ((WORD_VAL*)&t1)->byte.HB; TMR1L = t1;}
#define GETTIMER1(t1)   {(((WORD_VAL*)&t1)->byte.LB) = TMR1L; (((WORD_VAL*)&t1)->byte.HB) = TMR1H;}

//----------------------------------------------------------------------------
void lcdspeedscreen(void)
{
  static unsigned long ul;
  if(!PIR1bits.TMR1IF)
  {
    memcpy((char *)LcdText + FPSPOZ, "FPS:      ", 10);
    memcpy((char *)LcdText + CPSPOZ, "CPS:      ", 10);
    ul = (SystemClock/4UL) / ((unsigned long)t1cycle << 3);
    ltoa((char *)LcdText +  FPSPOZ + 4, ul, 10);
    ltoa((char *)LcdText +  CPSPOZ + 4, ul * (1UL * LCDLINES * LCDWIDTH), 10);
  }
  else
  {
    memcpy((char *)LcdText + FPSPOZ, "FPS: ovf  ", 10);
    memcpy((char *)LcdText + CPSPOZ, "CPS: ovf  ", 10);
    PIR1bits.TMR1IF = 0;
  }
}

//----------------------------------------------------------------------------
void init(void)
{
  // 1. id�z�t� inicializ�l�sa
  T1CONbits.T1OSCEN = 0;                // �rakvarc tilt�s
  T1CONbits.TMR1CS = 0;                 // forr�s �rajel = CPU clk
  T1CONbits.T1CKPS = 3;                 // el�oszt� : 1:8
  T1CONbits.TMR1ON = 0;                 // kikapcs
}

//******************************************************************************
void main(void)
{
  unsigned char c;

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
    T1CONbits.TMR1ON = 1;               // bekapcs
    LcdRefreshAll();
    T1CONbits.TMR1ON = 0;               // kikapcs
    GETTIMER1(t1cycle);

    // megjelen�t�s -----------------------
    lcdspeedscreen();
  }
}

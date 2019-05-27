//******************************************************************************
// "charlcd" LCD kijelz� friss�t�s�nek processzorhaszn�lat m�r� program, htc, xc8 fordit�khoz
// k�sz�t�: Roberto Benjami (robertodebenjami(at)gmail(dot)com)
//
// M�k�d�se:
// - k�t sz�ml�l�t n�vel, az egyiket LCD friss�t�s k�zben,
//   a m�sikat az LCD friss�t�s�nek kikapcsol�sa ut�n
//   ki�rja a k�t besz�molt �rt�ket, valamint azt, hogy az els� m�r�s eredm�nye 
//   h�ny sz�zal�kkal kevesebb a m�sodikn�l.
//
// charlcd.h sz�ks�ges be�ll�t�sai:
// - megszak�t�sos folyamatos friss�t�si �zemm�d (LCDMODECONTIRQ)
// - ne az 1.timert haszn�ljuk (LCDTIMERNUM)
//
// letesztelhetj�k:
// - LCDFRAMEPERSEC v�ltoztat�s�nak a hat�s�t
// - 4/8 bites �zemm�d hat�s�t
// - villogtat�s ki/be hat�s�t (LCDBLINKCHAR)
// - a k�l�nb�z� CPU �rajel hat�s�t
//******************************************************************************

#if defined(__XC__)
#include <xc.h>
#else
#include <htc.h>
#endif
#include <string.h>
#include <stdlib.h>
#include "charlcd.h"

// led l�bak defini�l�sa
#define  LED1       B, 0
#define  LED2       B, 1

#if (!defined LCDMODECONTIRQ) || LCDTIMERNUM == 1
#error "Rossz be�ll�t�sok az charlcd.h-ban"
#endif

#define GLUE(a, b)             a ## b
#define SET_(what, p, m)       GLUE(what, p) |= (1 << (m))
#define CLR_(what, p, m)       GLUE(what, p) &= ~(1 << (m))
#define GET_(p, m)             GLUE(PORT, p) & (1 << (m))
#define IOIN(x)                SET_(TRIS, x)
#define IOOUT(x)               CLR_(TRIS, x)
#ifdef _PIC14E
#define SET(x)                 SET_(LAT, x)
#define CLR(x)                 CLR_(LAT, x)
#else
#define SET(x)                 SET_(PORT, x)
#define CLR(x)                 CLR_(PORT, x)
#endif
#define GET(x)                 GET_(x)

//******************************************************************************
void init(void)
{
  // 1. id�z�t� inicializ�l�sa
  T1CONbits.T1OSCEN = 1;                // �rakvarc eng
  T1CONbits.TMR1CS = 1;                 // forr�s �rajel = CPU clk
  T1CONbits.T1CKPS = 0;                 // el�oszt� : 1:1
  T1CONbits.TMR1ON = 0;                 // kikapcs

  // ledek be�ll�t�sa
  #ifdef  LED1
  CLR(LED1); IOOUT(LED1);
  #endif
  #ifdef  LED2
  CLR(LED2); IOOUT(LED2);
  #endif
}
//******************************************************************************

void main(void)
{
  static unsigned long cycle1, cycle2;  // 32 bites ciklussz�ml�l�
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
  #ifdef LCDBLINKCHAR
  for(c = 24; c < 32; c++)
    LcdBlinkChar(c);
  #endif // LCDBLINKCHAR
  #else  // (1UL * LCDWIDTH * LCDLINES) < 80
  // 4x40 karakteres -------------------
  //                                   "0204060810121416182022242628303234363840"
  memcpy((char *)LcdText +   0, "****************************************", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText +  40, "*          LCD CPU used meter          *", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText +  80, "*                                      *", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText + 120, "****************************************", 40); // kezd�k�perny� felt�lt�se
  #ifdef LCDBLINKCHAR
  for(c = 41; c < 78; c++)
    LcdBlinkChar(c);                    // m�sodik sor sz�veg villog
  #endif // LCDBLINKCHAR
  #endif // else (1UL * LCDWIDTH * LCDLINES) < 80

  // megszak�t�s be�ll�t�sok
  INTCONbits.PEIE = 1;                  // perif�ria IRQ eng
  INTCONbits.GIE = 1;                   // glob�lis IRQ eng

  init();
  while(1)
  {
    cycle1 = 0; cycle2 = 0;             // sz�ml�l�k null�z�sa
    
    // m�r�s 1 ---------------------------
    LcdRefreshStart();                  // kijelz�friss�t�s menjen
    #ifdef  LED1
    CLR(LED1);
    #endif
    #ifdef  LED2
    SET(LED2);
    #endif
    TMR1H = 0; TMR1L = 0;               // timer1 (�ra) null�z�s
    PIR1bits.TMR1IF = 0;                // timer1 t�lcsordul�sjelz�s t�rl�s
    T1CONbits.TMR1ON = 1;               // TMR1 start
    while(!PIR1bits.TMR1IF) cycle1++;   // T1 t�lcsordul�s�ig sz�mol
    LcdRefreshStop();                   // kijelz�friss�t�s �lj
    
    // m�r�s 2 ---------------------------
    #ifdef  LED1
    SET(LED1);
    #endif
    #ifdef  LED2
    CLR(LED2);
    #endif
    TMR1H = 0; TMR1L = 0;               // timer1 (�ra) null�z�s
    PIR1bits.TMR1IF = 0;                // timer1 t�lcsordul�sjelz�s t�rl�s
    T1CONbits.TMR1ON = 1;               // TMR1 start
    while(!PIR1bits.TMR1IF) cycle2++;   // T1 t�lcsordul�s�ig sz�mol

    // megjelen�t�s -----------------------

    #if (1UL * LCDWIDTH * LCDLINES) < 80 
    // 2x16 karakteres -------------------
    memcpy((char *)LcdText, "                        -CpuSpd-", 32); // kezd�k�perny� felt�lt�se
    ltoa((char *)LcdText +  0, cycle1, 10);
    ltoa((char *)LcdText +  8, cycle2, 10);
    ltoa((char *)LcdText + 16, 100-((100*cycle1+50)/cycle2), 10);
    #else  // (1UL * LCDWIDTH * LCDLINES) < 80
    // 4x40 karakteres -------------------
    //                            "0204060810121416182022242628303234363840"
    memcpy((char *)LcdText +  80, "*With:        Without:              %  *", 40); // kezd�k�perny� felt�lt�se
    ltoa((char *)LcdText +  86, cycle1, 10);
    ltoa((char *)LcdText + 102, cycle2, 10);
    ltoa((char *)LcdText + 113, 100-((100*cycle1+50)/cycle2), 10);
    #endif // else (1UL * LCDWIDTH * LCDLINES) < 80 
  }
}

//******************************************************************************
// PIC16 megszak�t�s kiszolg�l�s
static void interrupt isr(void)
{
  #if (!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

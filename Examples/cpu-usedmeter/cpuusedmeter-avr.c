//******************************************************************************
// "charlcd" LCD kijelz� friss�t�s�nek processzorhaszn�lat m�r� program
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
// - 4/8 bites �zemm�d (LCD4BITMODE) hat�s�t
// - villogtat�s ki/be hat�s�t (LCDBLINKCHAR)
// - a k�l�nb�z� CPU �rajel hat�s�t
//******************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/ina90.h>
#include <string.h>
#include <stdlib.h>
#include "charlcd.h"

// led l�bak defini�l�sa
// #define  LEDS
#define  LED1ON     PORTB |=  (1 << 2)
#define  LED2ON     PORTB |=  (1 << 3)
#define  LED1OFF    PORTB &= ~(1 << 2)
#define  LED2OFF    PORTB &= ~(1 << 3)
#define  LED1IO     DDRB |=  (1 << 2)
#define  LED2IO     DDRB |=  (1 << 3)
#define  LED1       (PORTB & (1 << 2))
#define  LED2       (PORTB & (1 << 3))

// 1 megszak�t�s/sec
#define  TIMECLK    1

// �rajel forr�s frekvencia
#define  TimerSrcClock (1UL*SystemClock)

#if (!defined LCDMODECONTIRQ)
#error "A tesztprogram LCDMODECONTIRQ �zemm�dban m�k�dik!"
#endif

//******************************************************************************
void init(void)
{
  // id�z�t� inicializ�l�sa
  // timer 1
  #define  TIMEDIV ((TimerSrcClock+TIMECLK/2)/TIMECLK-1)
  #if TIMEDIV < 65536
  #define TMCLKDIV         1
  #define TMCLKSEL         1
  #elif TIMEDIV < (65536*8)
  #define TMCLKDIV         8
  #define TMCLKSEL         2
  #elif TIMEDIV < (65536*64)
  #define TMCLKDIV        64
  #define TMCLKSEL         3
  #elif TIMEDIV < (65536*256)
  #define TMCLKDIV       256
  #define TMCLKSEL         4
  #elif TIMEDIV < (65536*1024)
  #define TMCLKDIV      1024
  #define TMCLKSEL         5
  #endif // TIMEDIV

  #define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)
  OCR1AH = TMCOMP >> 8;                 // comparator HI
  OCR1AL = (unsigned char)TMCOMP;       // comparator LO
  TCCR1B = (0<<WGM12)|(TMCLKSEL<<CS10); // mode0, Clk = ClkIO/1..8..64..256..1024

  #ifdef  LEDS
  LED1OFF; LED2OFF; LEDIO1; LEDIO2;
  #endif

}
//******************************************************************************

int main(void)
{
  static unsigned long cycle1, cycle2;  // 32 bites ciklussz�ml�l�
  unsigned char c;

  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t

  #if (1UL * LCDWIDTH * LCDLINES) < 80 
  // 2x16 karakteres -------------------
  #ifdef LCDBLINKCHAR
  for(c = 24; c < 32; c++)
    LcdBlinkChar(c);
  #endif // LCDBLINKCHAR
  #else  // (1UL * LCDWIDTH * LCDLINES) < 80
  // 4x40 karakteres -------------------
  memcpy((char *)LcdText +   0, "****************************************", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText +  40, "*          LCD CPU used meter          *", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText +  80, "*                                      *", 40); // kezd�k�perny� felt�lt�se
  memcpy((char *)LcdText + 120, "****************************************", 40); // kezd�k�perny� felt�lt�se
  #ifdef LCDBLINKCHAR
  for(c = 41; c < 78; c++)
    LcdBlinkChar(c);                    // m�sodik sor sz�veg villog
  #endif // LCDBLINKCHAR
  #endif // else (1UL * LCDWIDTH * LCDLINES) < 80

  init();
  while(1)
  {
    cycle1 = 0; cycle2 = 0;             // sz�ml�l�k null�z�sa
    
    // m�r�s 1 ---------------------------
    LcdRefreshStart();                  // kijelz�friss�t�s menjen
    #ifdef  LEDS
    LED1OFF; LED2ON;
    #endif
    TCNT1H = 0; TCNT1L = 0;             // timer1 (�ra) null�z�s
	#ifdef TIFR
    TIFR = (1 << OCF1A);                // timer1 t�lcsordul�sjelz�s t�rl�s
    while(!(TIFR & (1<<OCF1A))) cycle1++;// id� letelt�ig sz�mol
	#else
    TIFR1 = (1 << OCF1A);               // timer1 t�lcsordul�sjelz�s t�rl�s
    while(!(TIFR1 & (1<<OCF1A))) cycle1++;// id� letelt�ig sz�mol
	#endif
    LcdRefreshStop();
    
    // m�r�s 2 ---------------------------
    #ifdef  LEDS
    LED1ON; LED2OFF;
    #endif
    TCNT1H = 0; TCNT1L = 0;             // timer1 (�ra) null�z�s
	#ifdef TIFR
    while(!(TIFR & (1<<OCF1A))) cycle2++;// id� letelt�ig sz�mol
    TIFR = (1 << OCF1A);                // timer1 t�lcsordul�sjelz�s t�rl�s
	#else
    TIFR1 = (1 << OCF1A);               // timer1 t�lcsordul�sjelz�s t�rl�s
    while(!(TIFR1 & (1<<OCF1A))) cycle2++;// id� letelt�ig sz�mol
	#endif

    // megjelen�t�s -----------------------
    #if (1UL * LCDWIDTH * LCDLINES) < 80 
    // 2x16 karakteres -------------------
    memcpy((char *)LcdText, "                                ", 32);
    ltoa(cycle1,  (char *)LcdText +  0, 10);
    ltoa(cycle2,  (char *)LcdText +  8, 10);
    ltoa(100-(100*cycle1/cycle2), (char *)LcdText + 16, 10);
    #else  // (1UL * LCDWIDTH * LCDLINES) < 80
    // 4x40 karakteres -------------------
    //                            "0204060810121416182022242628303234363840"
    memcpy((char *)LcdText +  80, "*With:        Without:              %  *", 40);
    ltoa(cycle1,  (char *)LcdText +  86, 10);
    ltoa(cycle2,  (char *)LcdText + 102, 10);
    ltoa(100-(100*cycle1/cycle2), (char *)LcdText + 113, 10);
    #endif // else (1UL * LCDWIDTH * LCDLINES) < 80 
  }
}

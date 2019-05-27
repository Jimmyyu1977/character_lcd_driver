//******************************************************************************
// "charlcd" LCD kijelz� friss�t�s�nek processzorhaszn�lat m�r� program
// pic18fxxx vez�rl�h�z, c18 fordit�hoz
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
//
// letesztelhetj�k:
// - LCDFRAMEPERSEC v�ltoztat�s�nak a hat�s�t
// - 4/8 bites �zemm�d (LCD4BITMODE) hat�s�t
// - villogtat�s ki/be hat�s�t (LCDBLINKCHAR)
// - a k�l�nb�z� CPU �rajel hat�s�t
//******************************************************************************

#include <p18cxxx.h>
#include <string.h>
#include <stdlib.h>
#include <timers.h>
#include "charlcd.h"

// bootloader n�lk�l: 0x0000, MCHPUSB bootloader: 0x0800, HID bootloader: 0x1000
#define RESET_VECTOR_ADDRESS             0x0

// led l�bak defini�l�sa
#define  LED1(f)    f(B, 0)
#define  LED2(f)    f(B, 1)

#if (!defined LCDMODECONTIRQ)
#error "A tesztprogram LCDMODECONTIRQ �zemm�dban m�k�dik!"
#endif

#if LCDTIMERNUM == 1
#error "Az LCD-t ne az 1. timerrel haszn�ljuk!"
#endif

#define TRISBIT(p, m)         TRIS ## p ## bits.TRIS ## p ## m
#define LATBIT(p, m)          LAT ## p ## bits.LAT ## p ## m
#define PORTBIT(p, m)         PORT ## p ## bits.R ## p ## m
#define TRISX(p, m)           TRIS ## p
#define LATX(p, m)            LAT ## p
#define PORTX(p, m)           PORT ## p

#define GPIOX_MODEPININ(x)    x(TRISBIT) = 1
#define GPIOX_MODEPINOUT(x)   x(TRISBIT) = 0
#define GPIOX_SETBIT(x)       x(LATBIT) = 1
#define GPIOX_CLRBIT(x)       x(LATBIT) = 0
#define GPIOX_GETBIT(x)       x(PORTBIT)

//******************************************************************************
void init(void)
{
  // 1. id�z�t� inicializ�l�sa
  T1CONbits.RD16 = 1;                   // 16bites �r�s/olv
  T1CONbits.T1OSCEN = 1;                // �rakvarc eng
  T1CONbits.TMR1CS = 1;                 // forr�s �rajel = �rakvarc
  T1CONbits.T1CKPS = 0;                 // el�oszt� : 1:1
  T1CONbits.TMR1ON = 0;                 // kikapcs

  // ledek be�ll�t�sa
  #ifdef  LED1
  GPIOX_CLRBIT(LED1); GPIOX_MODEPINOUT(LED1);
  #endif
  #ifdef  LED2
  GPIOX_CLRBIT(LED2); GPIOX_MODEPINOUT(LED2);
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
    LcdRefreshStart();                  // kijelz?friss�t�s menjen
    #ifdef  LED1
    GPIOX_CLRBIT(LED1);
    #endif
    #ifdef  LED2
    GPIOX_SETBIT(LED2);
    #endif
    TMR1H = 0; TMR1L = 0;               // timer1 (�ra) null�z�s
    PIR1bits.TMR1IF = 0;                // timer1 t�lcsordul�sjelz�s t�rl�s
    T1CONbits.TMR1ON = 1;               // TMR1 start
    while(!PIR1bits.TMR1IF) cycle1++;   // T1 t�lcsordul�s�ig sz�mol
    LcdRefreshStop();                   // kijelz?friss�t�s �lj
    
    // m�r�s 2 ---------------------------
    #ifdef  LED1
    GPIOX_SETBIT(LED1);
    #endif
    #ifdef  LED2
    GPIOX_CLRBIT(LED2);
    #endif
    TMR1H = 0; TMR1L = 0;               // timer1 (�ra) null�z�s
    PIR1bits.TMR1IF = 0;                // timer1 t�lcsordul�sjelz�s t�rl�s
    T1CONbits.TMR1ON = 1;               // TMR1 start
    while(!PIR1bits.TMR1IF) cycle2++;   // T1 t�lcsordul�s�ig sz�mol

    // megjelen�t�s -----------------------

    #if (1UL * LCDWIDTH * LCDLINES) < 80 
    // 2x16 karakteres -------------------
    memcpypgm2ram((char *)LcdText, "                        -CpuSpd-", 32); // kezd�k�perny� felt�lt�se
    ltoa(cycle1, (char *)LcdText +  0);
    ltoa(cycle2, (char *)LcdText +  8);
    ltoa(100-((100*cycle1+50)/cycle2), (char *)LcdText + 16);
    #else  // (1UL * LCDWIDTH * LCDLINES) < 80
    // 4x40 karakteres -------------------
    //                            "0204060810121416182022242628303234363840"
    memcpypgm2ram((char *)LcdText +  80, "*With:        Without:              %  *", 40); // kezd�k�perny� felt�lt�se
    ltoa(cycle1, (char *)LcdText +  86);
    ltoa(cycle2, (char *)LcdText + 102);
    ltoa(100-((100*cycle1+50)/cycle2), (char *)LcdText + 113);
    #endif // else (1UL * LCDWIDTH * LCDLINES) < 80 
  }
}

#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode()
{
  #if (LCDTIMERPR18 == -1) || (LCDTIMERPR18 == 1)
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode()
{
  #if LCDTIMERPR18 == 0
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

#if RESET_VECTOR_ADDRESS == 0x1000
  #define HIGH_INTERRUPT_VECTOR_ADDRESS  0x1008
  #define LOW_INTERRUPT_VECTOR_ADDRESS   0x1018
#elif RESET_VECTOR_ADDRESS == 0x800
  #define HIGH_INTERRUPT_VECTOR_ADDRESS   0x808
  #define LOW_INTERRUPT_VECTOR_ADDRESS    0x818
#else
  #define HIGH_INTERRUPT_VECTOR_ADDRESS    0x08
  #define LOW_INTERRUPT_VECTOR_ADDRESS     0x18
#endif

#if RESET_VECTOR_ADDRESS != 0x0
extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code RESET_VECTOR = RESET_VECTOR_ADDRESS
void _reset (void)
{
  _asm goto _startup _endasm
}
#endif

#pragma code HIGH_INTERRUPT_VECTOR = HIGH_INTERRUPT_VECTOR_ADDRESS
void High_ISR (void)
{
  _asm goto YourHighPriorityISRCode _endasm
}

#pragma code LOW_INTERRUPT_VECTOR = LOW_INTERRUPT_VECTOR_ADDRESS
void Low_ISR (void)
{
  _asm goto YourLowPriorityISRCode _endasm
}

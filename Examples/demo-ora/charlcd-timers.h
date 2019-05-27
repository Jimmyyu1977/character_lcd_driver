/* 
 * File:   charlcd-timers.h
 * Author: Benjami
 *
 * Created on 2017. okt�ber 27., 17:00
 */

#ifndef CHARLCD_TIMERS_H
#define	CHARLCD_TIMERS_H

#ifdef	__cplusplus
extern "C" {
#endif

//==============================================================================
// timer interf�sz
//==============================================================================

// timer kiv�laszt�sa
#define TIMERNUM LCDTIMERNUM

// frekvencia megad�sa
#define TIMECLK  LCDCHARPERSEC

// ennyivel kell elosztani a rendszer�rajelet
#define TIMEDIV ((TimerSrcClock+TIMECLK/2)/TIMECLK)

// PIC24: timer IRQ priorit�sa (1=legalacsonyabb, 7=legmagasabb)
#define TIMERPR24      1

// timer megszak�t�s kiszolg�l� szubrutin max. fut�si ideje (CPU utas�t�ssz�m)
#define TIMESUBCIKL_AVR    50
#define TIMESUBCIKL_PIC16 200 
#define TIMESUBCIKL_PIC18 200
#define TIMESUBCIKL_PIC24  50

//==============================================================================
// timer
//==============================================================================

// AVR
#if defined(__AVR__)
#include <avr/interrupt.h>
#include <compat/ina90.h>

// glob�lis interrupt bekapcsol�sa inicializ�l�skor
#ifdef  LCDGLOBALINTAUTOSTART
#define GLOBALINTSTART sei()
#else
#define GLOBALINTSTART ;
#endif

// a megszak�t�s kiszolg�l� f�ggv�ny v�grehajt�si idej�t�l f�gg�en
#if TIMEDIV < TIMESUBCIKL_AVR
#error "AVR: timer frekvencia t�l magas"
#endif

// a megszak�t�st nem sz�ks�ges nyugt�zni
#define  TIMERCOMPLESS ;

//------------------------------------------------------------------------------
// AVR timer0
#if TIMERNUM == 0

// oszt�
#if TIMEDIV < 256
#define TMCLKDIV         1
#define TMCLKSEL         1
#elif TIMEDIV < (256*8)
#define TMCLKDIV         8
#define TMCLKSEL         2
#elif TIMEDIV < (256*64)
#define TMCLKDIV        64
#define TMCLKSEL         3
#elif TIMEDIV < (256*256)
#define TMCLKDIV       256
#define TMCLKSEL         4
#elif TIMEDIV < (256*1024)
#define TMCLKDIV      1024
#define TMCLKSEL         5
#else
#error "AVR: timer frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

#if defined OCR0 || defined OCR0A
// kompar�toros timer0
#ifdef  TCCR0A
#ifdef  TCCR0B // (A/B regiszteres TIMER0)
#define TIMERINIT {\
  OCR0A = TMCOMP;                       /* comparator */\
  TCCR0A = (1<<WGM01);                  /* MODE1 */\
  TCCR0B = (TMCLKSEL<<CS00);            /* frekvencia oszt�s */\
  GLOBALINTSTART;}
#else // TCCR0B (A regiszteres TIMER0)
#define TIMERINIT {\
  OCR0A = TMCOMP;                       /* comparator */\
  TCCR0A = (1<<CTC0)|(TMCLKSEL<<CS00);  /* CTC mode, frekvencia oszt�s */\
  GLOBALINTSTART;}
#endif // else TCCR0B
#else  // TCCR0A
#define TIMERINIT {\
  OCR0 = TMCOMP;                        /* comparator */\
  TCCR0 = (1<<WGM01)|(TMCLKSEL<<CS00);  /* CTC mode, frekvencia oszt�s */\
  GLOBALINTSTART;}
#endif // TCCR0A

// timer megszak�t�s kiszolg�l� f�ggv�ny
#ifdef  TIMER0_COMP_vect
#define TIMERINTPROCESS  ISR (TIMER0_COMP_vect)
#else
#define TIMERINTPROCESS  ISR (TIMER0_COMPA_vect)
#endif

#else   // defined OCR0 || defined OCR0A
// kompar�tor n�lk�li timer0

#define TIMERINIT {\
  TCNT0 = 255 - TMCOMP;                 /* comparator helyett kezd��rt�k */\
  TCCR0 = TMCLKSEL<<CS00;               /* frekvencia oszt�s */\
  TIMSK |= (1<<TOIE0);                  /* overflow IRQ eng */\
  GLOBALINTSTART;}

#define TIMERINTPROCESS  ISR (TIMER0_OVF_vect)

// hardver kompar�tort p�tolni kell
#undef  TIMERCOMPLESS
#define TIMERCOMPLESS      TCNT0 += (255 - TMCOMP)

#endif  // defined OCR0 || defined OCR0A

//------------------------------------------------------------------------------
// AVR timer1
#elif TIMERNUM == 1

// oszt�
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
#else
#error "AVR: timer frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#define TIMERINIT {\
  OCR1AH = TMCOMP >> 8;                 /* comparator HI */\
  OCR1AL = (unsigned char)TMCOMP;       /* comparator LO */\
  TCCR1A = (0<<WGM10);                  /* mode4 (CTC) */\
  TCCR1B = (1<<WGM12)|(TMCLKSEL<<CS10); /* mode4, Clk = ClkIO/1..8..64..256..1024 */\
  GLOBALINTSTART;}
  
// timer megszak�t�s kiszolg�l� f�ggv�ny
#define TIMERINTPROCESS  ISR (TIMER1_COMPA_vect)

//------------------------------------------------------------------------------
// AVR timer2
#elif TIMERNUM == 2

// oszt�
#if TIMEDIV < 256
#define TMCLKDIV         1
#define TMCLKSEL         1
#elif TIMEDIV < (256*8)
#define TMCLKDIV         8
#define TMCLKSEL         2
#elif TIMEDIV < (256*32)
#define TMCLKDIV        32
#define TMCLKSEL         3
#elif TIMEDIV < (256*64)
#define TMCLKDIV        64
#define TMCLKSEL         4
#elif TIMEDIV < (256*128)
#define TMCLKDIV       128
#define TMCLKSEL         5
#elif TIMEDIV < (256*256)
#define TMCLKDIV       256
#define TMCLKSEL         6
#elif TIMEDIV < (256*1024)
#define TMCLKDIV      1024
#define TMCLKSEL         7
#else
#error "AVR: timer a frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#ifdef TCCR2A
#define TIMERINIT {\
  OCR2A = TMCOMP;                       /* comparator */\
  TCCR2A = (1<<WGM21);                  /* mode 1 (CTC) */\
  TCCR2B = (TMCLKSEL<<CS20);            /* oszt� */\
  GLOBALINTSTART;}
#else  // TCCR2A
#define TIMERINIT {\
  OCR2  = TMCOMP;                       /* comparator */\
  TCCR2  = (1<<WGM21)|(TMCLKSEL<<CS20); /* mode (CTC), oszt� */\
  GLOBALINTSTART;}
#endif // TCCR2A
  
// timer megszak�t�s kiszolg�l� f�ggv�ny
#ifdef  TIMER2_COMP_vect
#define TIMERINTPROCESS ISR (TIMER2_COMP_vect)
#else
#define TIMERINTPROCESS ISR (TIMER2_COMPA_vect)
#endif

#else  // TIMERNUM
#error "AVR: csak a timer 0, 1, 2 v�laszthat�"
#endif // TIMERNUM

// nem sz�ks�ges a cs�ldordul�st jelz� bitet t�r�lni
#define TIMERIRQACK ;

// AVR
//==============================================================================
// PIC16
#elif (defined(_PIC14) || defined(_PIC14E))

// a megszak�t�s kiszolg�l� f�ggv�ny v�grehajt�si idej�t�l f�gg�en
#if TIMEDIV < TIMESUBCIKL_PIC16
#error "PIC16: timer frekvencia t�l magas"
#endif

// glob�lis interrupt bekapcsol�sa inicializ�l�skor
#ifdef  LCDGLOBALINTAUTOSTART
#define GLOBALINTSTART  INTCONbits.GIE = 1
#else
#define GLOBALINTSTART ;
#endif

//------------------------------------------------------------------------------
// PIC16 timer0
#if TIMERNUM == 0

// oszt�
#if TIMEDIV < 256
#define TMCLKDIV       1
#define TMCLKSEL       OPTION_REGbits.PSA = 1
#define TMCOMPCOR      2
#elif TIMEDIV < (256*2)
#define TMCLKDIV       2
#define TMCLKSEL       OPTION_REGbits.PS = 0; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      1
#elif TIMEDIV < (256*4)
#define TMCLKDIV       4
#define TMCLKSEL       OPTION_REGbits.PS = 1; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#elif TIMEDIV < (256*8)
#define TMCLKDIV       8
#define TMCLKSEL       OPTION_REGbits.PS = 2; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#elif TIMEDIV < (256*16)
#define TMCLKDIV       16
#define TMCLKSEL       OPTION_REGbits.PS = 3; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#elif TIMEDIV < (256*32)
#define TMCLKDIV       32
#define TMCLKSEL       OPTION_REGbits.PS = 4; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#elif TIMEDIV < (256*64)
#define TMCLKDIV       64
#define TMCLKSEL       OPTION_REGbits.PS = 5; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#elif TIMEDIV < (256*128)
#define TMCLKDIV       128
#define TMCLKSEL       OPTION_REGbits.PS = 6; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#elif TIMEDIV < (256*256)
#define TMCLKDIV       256
#define TMCLKSEL       OPTION_REGbits.PS = 7; OPTION_REGbits.PSA = 0
#define TMCOMPCOR      0
#else
#error "PIC16: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer kompar�tor hi�ny�nak p�tl�sa
#define TIMERCOMPLESS  TMR0 += (TMCOMPCOR-TMCOMP)

// timer inicializ�l�s
#define TIMERINIT {\
  OPTION_REGbits.T0CS = 0;              /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  GLOBALINTSTART;                       /* glob�lis IRQ eng */\
  INTCONbits.PEIE = 1;                  /* perif�ria IRQ eng */\
  TMR0 = (255+TMCOMPCOR-TMCOMP);}       /* timer kezd��rt�k felt�lt�se */

// timer bekapcsol�s (nem bekapcsolhat�, csak IRQ-t lehet enged�lyezni)
#undef  LcdRefreshStart()
#define LcdRefreshStart()  if(!INTCONbits.T0IE){TMR0 = (255+TMCOMPCOR-TMCOMP); INTCONbits.T0IE = 1;}

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK INTCONbits.T0IF = 0

//------------------------------------------------------------------------------
// PIC16 timer1
#elif TIMERNUM == 1

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T1CONbits.T1CKPS = 0
#define TMCOMPCOR      1
#elif TIMEDIV < (65536*2)
#define TMCLKDIV       2
#define TMCLKSEL       T1CONbits.T1CKPS = 1
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*4)
#define TMCLKDIV       4
#define TMCLKSEL       T1CONbits.T1CKPS = 2
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T1CONbits.T1CKPS = 3
#define TMCOMPCOR      0
#else
#error "PIC16: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer kompar�tor hi�ny�nak p�tl�sa
#define TIMERCOMPLESS  TMR1 += (65535+TMCOMPCOR-TMCOMP)

// timer inicializ�l�s
#define TIMERINIT {\
  T1CONbits.TMR1CS = 0;                 /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  PIE1bits.TMR1IE = 1;                  /* megszak�t�s enged�lyez�s */\
  GLOBALINTSTART;                       /* glob�lis IRQ eng */\
  INTCONbits.PEIE = 1;                  /* perif�ria IRQ eng */\
  TMR1 = 65535+TMCOMPCOR-TMCOMP;}       /* timer kezd?�rt�k felt�lt�se */

// timer bekapcsol�s
#undef  LcdRefreshStart()
#define LcdRefreshStart()  if(!T1CONbits.TMR1ON){TMR1 = 65535+TMCOMPCOR-TMCOMP; T1CONbits.TMR1ON = 1;}

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK PIR1bits.TMR1IF = 0

//------------------------------------------------------------------------------
// PIC16 timer2
#elif TIMERNUM == 2

// oszt�
#if TIMEDIV < 256
#define TMCLKDIV       1
#define TMCLKSEL       T2CON = 0b00000000;
#elif TIMEDIV < (256*2)
#define TMCLKDIV       2
#define TMCLKSEL       T2CON = 0b00001000;
#elif TIMEDIV < (256*4)
#define TMCLKDIV       4
#define TMCLKSEL       T2CON = 0b00000001;
#elif TIMEDIV < (256*8)
#define TMCLKDIV       8
#define TMCLKSEL       T2CON = 0b00001001;
#elif TIMEDIV < (256*16)
#define TMCLKDIV       16
#define TMCLKSEL       T2CON = 0b00000010;
#elif TIMEDIV < (256*16*2)
#define TMCLKDIV       32
#define TMCLKSEL       T2CON = 0b00001010;
#elif TIMEDIV < (256*16*4)
#define TMCLKDIV       64
#define TMCLKSEL       T2CON = 0b00011010;
#elif TIMEDIV < (256*16*8)
#define TMCLKDIV       128
#define TMCLKSEL       T2CON = 0b00111010;
#elif TIMEDIV < (256*16*16)
#define TMCLKDIV       256
#define TMCLKSEL       T2CON = 0b01111010;
#else
#error "PIC16: timer frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#define TIMERINIT {\
  TMCLKSEL;                             /* oszt� */\
  PIE1bits.TMR2IE = 1;                  /* timer enged�lyez�s */\
  GLOBALINTSTART;                       /* glob�lis IRQ eng */\
  INTCONbits.PEIE = 1;                  /* perif�ria IRQ eng */\
  PR2 = TMCOMP;}

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK PIR1bits.TMR2IF = 0

// mivel timer kompar�tor van, nem kell szoftverb?l p�tolni
#define TIMERCOMPLESS  ;

#else  // TIMERNUM
#error "PIC16: csak timer 0, 1, 2 v�laszthat�"
#endif // TIMERNUM

// PIC16
//==============================================================================
// PIC18
#elif (defined(__18CXX) || defined(__PICC18__))

// PIC18: timer IRQ priorit�sa (0 = alacsony, 1 = magas)
#if LCDTIMERPR18 == -1
// priorit�s n�lk�li megszak�t�s
#define TIMERPR18     1
#define PR18ONOFF {RCONbits.IPEN = 0; INTCONbits.PEIE = 1;}
#define PR18GIE   INTCONbits.GIE = 1
#elif LCDTIMERPR18 == 0
// alacsony priorit�s
#define TIMERPR18     0
#define PR18ONOFF RCONbits.IPEN = 1
#define PR18GIE   INTCONbits.GIEL = 1; INTCONbits.GIEH = 1
#elif LCDTIMERPR18 == 1
// magas priorit�s
#define TIMERPR18     1
#define PR18ONOFF RCONbits.IPEN = 1
#define PR18GIE   INTCONbits.GIEL = 1; INTCONbits.GIEH = 1
#else
#error "LCD TIMERPR18 csak -1, 0, 1 lehet!"
#endif

// glob�lis interrupt bekapcsol�sa inicializ�l�skor
#ifndef  LCDGLOBALINTAUTOSTART
#undef   PR18GIE
#define  PR18GIE  ;
#endif

// a megszak�t�s kiszolg�l� f�ggv�ny v�grehajt�si idej�t?l f�gg?en
#if TIMEDIV < TIMESUBCIKL_PIC18
#error "PIC18: timer frekvencia t�l magas"
#endif

//------------------------------------------------------------------------------
// PIC18 timer0
#if TIMERNUM == 0

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T0CONbits.PSA = 1  /* el?oszt� tilt�s */
#define TMCOMPCOR      7
#elif TIMEDIV < (65536*2)
#define TMCLKDIV       2
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 0; T0CONbits.T0PS1 = 0; T0CONbits.T0PS0 = 0  /* el?oszt� eng, oszt�s = 2 */
#define TMCOMPCOR      4
#elif TIMEDIV < (65536*4)
#define TMCLKDIV       4
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 0; T0CONbits.T0PS1 = 0; T0CONbits.T0PS0 = 1  /* el?oszt� eng, oszt�s = 4 */
#define TMCOMPCOR      2
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 0; T0CONbits.T0PS1 = 1; T0CONbits.T0PS0 = 0  /* el?oszt� eng, oszt�s = 8 */
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*16)
#define TMCLKDIV       16
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 0; T0CONbits.T0PS1 = 1; T0CONbits.T0PS0 = 1  /* el?oszt� eng, oszt�s = 16 */
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*32)
#define TMCLKDIV       32
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 1; T0CONbits.T0PS1 = 0; T0CONbits.T0PS0 = 0  /* el?oszt� eng, oszt�s = 32 */
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*64)
#define TMCLKDIV       64
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 1; T0CONbits.T0PS1 = 0; T0CONbits.T0PS0 = 1  /* el?oszt� eng, oszt�s = 64 */
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*128)
#define TMCLKDIV       128
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 1; T0CONbits.T0PS1 = 1; T0CONbits.T0PS0 = 0  /* el?oszt� eng, oszt�s = 128 */
#define TMCOMPCOR      0
#elif TIMEDIV < (65536*256)
#define TMCLKDIV       256
#define TMCLKSEL       T0CONbits.PSA = 0; T0CONbits.T0PS2 = 1; T0CONbits.T0PS1 = 1; T0CONbits.T0PS0 = 1  /* el?oszt� eng, oszt�s = 256 */
#define TMCOMPCOR      0
#else
#error "PIC18: timer frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer kompar�tor hi�ny�nak p�tl�s�hoz �tmeneti v�ltoz�
union
{
  struct
  {
    unsigned char LO;
    unsigned char HI;
  };
  unsigned short W;
}TimerTemp;

// timer kompar�tor hi�ny�nak p�tl�sa
#define TIMERCOMPLESS {                        \
  TimerTemp.LO = TMR0L;                        \
  TimerTemp.HI = TMR0H;                        \
  TimerTemp.W += 65535 + TMCOMPCOR - TMCOMP; \
  TMR0H = TimerTemp.HI;                        \
  TMR0L = TimerTemp.LO;}

// timer inicializ�l�s
#define TIMERINIT {\
  T0CONbits.T08BIT = 0;                 /* 16bites timer */\
  T0CONbits.T0CS = 0;                   /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  INTCON2bits.TMR0IP = TIMERPR18;       /* priorit�s */\
  INTCONbits.TMR0IE = 1;                /* megszak�t�s enged�lyez�s */\
  PR18ONOFF;                            /* k�tszint? priorit�s� ki vagy be */\
  PR18GIE;                              /* glob�lis IRQ */\
  TMR0L = 65535+TMCOMPCOR-TMCOMP;       /* timerL kezd?�rt�k felt�lt�se bitid?re */\
  TMR0H = (65535+TMCOMPCOR-TMCOMP) >> 8;}/* timerH kezd?�rt�k felt�lt�se bitid?re */

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK INTCONbits.TMR0IF = 0

//------------------------------------------------------------------------------
// PIC18 timer1
#elif TIMERNUM == 1

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T1CONbits.T1CKPS1 = 0; T1CONbits.T1CKPS0 = 0 /* el?oszt� 1:1 */
#define TMCOMPCOR      5
#elif TIMEDIV < (65536*2)
#define TMCLKDIV       2
#define TMCLKSEL       T1CONbits.T1CKPS1 = 0; T1CONbits.T1CKPS0 = 1 /* el?oszt� 1:2 */
#define TMCOMPCOR      3
#elif TIMEDIV < (65536*4)
#define TMCLKDIV       4
#define TMCLKSEL       T1CONbits.T1CKPS1 = 1; T1CONbits.T1CKPS0 = 0 /* el?oszt� 1:4 */
#define TMCOMPCOR      2
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T1CONbits.T1CKPS1 = 1; T1CONbits.T1CKPS0 = 1 /* el?oszt� 1:8 */
#define TMCOMPCOR      1
#else
#error "PIC18: timer frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer kompar�tor hi�ny�nak p�tl�s�hoz �tmeneti v�ltoz�
union
{
  struct
  {
    unsigned char LO;
    unsigned char HI;
  };
  unsigned short W;
}TimerTemp;

// timer kompar�tor hi�ny�nak p�tl�sa
#define TIMERCOMPLESS {                        \
  TimerTemp.LO = TMR1L;                        \
  TimerTemp.HI = TMR1H;                        \
  TimerTemp.W += 65535 + TMCOMPCOR - TMCOMP;   \
  TMR1H = TimerTemp.HI;                        \
  TMR1L = TimerTemp.LO;}

// timer inicializ�l�s
#define TIMERINIT {\
  T1CONbits.RD16 = 1;                   /* 16bites �r�s/olv */\
  T1CONbits.TMR1CS = 0;                 /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPR1bits.TMR1IP = TIMERPR18;          /* priorit�s */\
  PIE1bits.TMR1IE = 1;                  /* megszak�t�s enged�lyez�s */\
  PR18ONOFF;                            /* k�tszint? priorit�s� ki vagy be */\
  PR18GIE;                              /* glob�lis IRQ */\
  TMR1L = 65535+TMCOMPCOR-TMCOMP;       /* timerL kezd?�rt�k felt�lt�se bitid?re */\
  TMR1H = (65535+TMCOMPCOR-TMCOMP) >> 8;}/* timerH kezd?�rt�k felt�lt�se bitid?re */

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK PIR1bits.TMR1IF = 0

//------------------------------------------------------------------------------
// PIC18 timer2
#elif TIMERNUM == 2

#if TIMEDIV < 256
#define TMCLKDIV       1
#define TMCLKSEL       T2CON = 0b00000000;
#elif TIMEDIV < (256*2)
#define TMCLKDIV       2
#define TMCLKSEL       T2CON = 0b00001000;
#elif TIMEDIV < (256*4)
#define TMCLKDIV       4
#define TMCLKSEL       T2CON = 0b00000001;
#elif TIMEDIV < (256*8)
#define TMCLKDIV       8
#define TMCLKSEL       T2CON = 0b00001001;
#elif TIMEDIV < (256*16)
#define TMCLKDIV       16
#define TMCLKSEL       T2CON = 0b00000010;
#elif TIMEDIV < (256*16*2)
#define TMCLKDIV       32
#define TMCLKSEL       T2CON = 0b00001010;
#elif TIMEDIV < (256*16*4)
#define TMCLKDIV       64
#define TMCLKSEL       T2CON = 0b00011010;
#elif TIMEDIV < (256*16*8)
#define TMCLKDIV       128
#define TMCLKSEL       T2CON = 0b00111010;
#elif TIMEDIV < (256*16*16)
#define TMCLKDIV       256
#define TMCLKSEL       T2CON = 0b01111010;
#else
#error "PIC18: timer frekvencia t�l alacsony (C18)"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// mivel timer kompar�tor van, nem kell szoftverb?l p�tolni
#define TIMERCOMPLESS ;

// timer inicializ�l�s
#define TIMERINIT \
  TMCLKSEL;                             /* oszt� */\
  PR2 = TMCOMP;                         /* kompar�tor */\
  IPR1bits.TMR2IP = TIMERPR18;          /* priorit�s */\
  PIE1bits.TMR2IE = 1;                  /* timer megszak�t�s enged�lyez�s */\
  PR18ONOFF;                            /* k�tszint? priorit�s� ki vagy be */\
  PR18GIE;                              /* glob�lis IRQ */

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK PIR1bits.TMR2IF = 0

//------------------------------------------------------------------------------
// PIC18 timer3
#elif TIMERNUM == 3

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T3CONbits.T3CKPS0 = 0; T3CONbits.T3CKPS1 = 0 /* el�oszt� 1:1 */
#define TMCOMPCOR      5
#elif TIMEDIV < (65536*2)
#define TMCLKDIV       2
#define TMCLKSEL       T3CONbits.T3CKPS0 = 0; T3CONbits.T3CKPS1 = 1 /* el�oszt� 1:2 */
#define TMCOMPCOR      3
#elif TIMEDIV < (65536*4)
#define TMCLKDIV       4
#define TMCLKSEL       T3CONbits.T3CKPS0 = 1; T3CONbits.T3CKPS1 = 0 /* el�oszt� 1:4 */
#define TMCOMPCOR      2
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T3CONbits.T3CKPS0 = 1; T3CONbits.T3CKPS1 = 1 /* el�oszt� 1:8 */
#define TMCOMPCOR      1
#else
#error "PIC18: timer frekvencia t�l alacsony"
#endif // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer kompar�tor hi�ny�nak p�tl�s�hoz �tmeneti v�ltoz�
union
{
  struct
  {
    unsigned char LO;
    unsigned char HI;
  };
  unsigned short W;
}TimerTemp;

// timer kompar�tor hi�ny�nak p�tl�sa
#define TIMERCOMPLESS {                        \
  TimerTemp.LO = TMR3L;                        \
  TimerTemp.HI = TMR3H;                        \
  TimerTemp.W += 65535 + TMCOMPCOR - TMCOMP;   \
  TMR3H = TimerTemp.HI;                        \
  TMR3L = TimerTemp.LO;}

// timer inicializ�l�s
#define TIMERINIT {\
  T3CONbits.RD16 = 1;                   /* 16bites �r�s/olv */\
  T3CONbits.TMR3CS = 0;                 /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPR2bits.TMR3IP = TIMERPR18;          /* priorit�s */\
  PIE2bits.TMR3IE = 1;                  /* megszak�t�s enged�lyez�s */\
  PR18ONOFF;                            /* k�tszint? priorit�s� ki vagy be */\
  PR18GIE;                              /* glob�lis IRQ */\
  TMR3L = 65535+TMCOMPCOR-TMCOMP;       /* timerL kezd?�rt�k felt�lt�se bitid?re */\
  TMR3H = (65535+TMCOMPCOR-TMCOMP) >> 8;}/* timerH kezd?�rt�k felt�lt�se bitid?re */

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK PIR2bits.TMR3IF = 0

#else  // TIMERNUM
#error "PIC18: csak timer 0, 1, 2, 3 v�laszthat�"
#endif // TIMERNUM

// PIC18
//==============================================================================
// PIC24
#elif defined(__C30__)

// a megszak�t�s kiszolg�l� f�ggv�ny v�grehajt�si idej�t?l f�gg?en
#if TIMEDIV < TIMESUBCIKL_PIC24
#error "PIC24: timer frekvencia t�l magas"
#endif

// mivel timer kompar�tor van, nem kell szoftverb?l p�tolni
#define  TIMERCOMPLESS ;

//------------------------------------------------------------------------------
// PIC24 timer1
#if TIMERNUM == 1

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T1CONbits.TCKPS = 0
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T1CONbits.TCKPS = 1
#elif TIMEDIV < (65536*64)
#define TMCLKDIV       64
#define TMCLKSEL       T1CONbits.TCKPS = 2
#elif TIMEDIV < (65536*256)
#define TMCLKDIV       256
#define TMCLKSEL       T1CONbits.TCKPS = 3
#else
#error "PIC24: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#define TIMERINIT {\
  T1CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* el?oszt� */\
  IPC0bits.T1IP = TIMERPR24;            /* priorit�s */\
  PR1 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC0bits.T1IE = 1;}                   /* timer IRQ enged�lyez�s */

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK IFS0bits.T1IF = 0

// timer megszak�t�s kiszolg�l� f�ggv�ny
#define TIMERINTPROCESS void __attribute__ ((interrupt, auto_psv)) _T1Interrupt(void)

//------------------------------------------------------------------------------
// PIC24 timer2
#elif TIMERNUM == 2

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T2CONbits.TCKPS = 0
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T2CONbits.TCKPS = 1
#elif TIMEDIV < (65536*64)
#define TMCLKDIV       64
#define TMCLKSEL       T2CONbits.TCKPS = 2
#elif TIMEDIV < (65536*256)
#define TMCLKDIV       256
#define TMCLKSEL       T2CONbits.TCKPS = 3
#else
#error "PIC24: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#define TIMERINIT {\
  T2CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPC1bits.T2IP = TIMERPR24;            /* priorit�s */\
  PR2 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC0bits.T2IE = 1;}                   /* timer IRQ enged�lyez�s */

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK IFS0bits.T2IF = 0

// timer megszak�t�s kiszolg�l� f�ggv�ny
#define TIMERINTPROCESS void __attribute__ ((interrupt, auto_psv)) _T2Interrupt(void)

//------------------------------------------------------------------------------
// PIC24 timer3
#elif TIMERNUM == 3

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T3CONbits.TCKPS = 0
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T3CONbits.TCKPS = 1
#elif TIMEDIV < (65536*64)
#define TMCLKDIV       64
#define TMCLKSEL       T3CONbits.TCKPS = 2
#elif TIMEDIV < (65536*256)
#define TMCLKDIV       256
#define TMCLKSEL       T3CONbits.TCKPS = 3
#else
#error "PIC24: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#if defined(__dsPIC30F__)
#define TIMERINIT {\
  T3CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPC1bits.T3IP = TIMERPR24;            /* priorit�s */\
  PR3 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC0bits.T3IE = 1;}                   /* timer IRQ enged�lyez�s */
#else
#define TIMERINIT {\
  T3CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPC2bits.T3IP = TIMERPR24;            /* priorit�s */\
  PR3 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC0bits.T3IE = 1;}                   /* timer IRQ enged�lyez�s */
#endif

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK IFS0bits.T3IF = 0

// timer megszak�t�s kiszolg�l� f�ggv�ny
#define TIMERINTPROCESS void __attribute__ ((interrupt, auto_psv)) _T3Interrupt(void)

//------------------------------------------------------------------------------
// PIC24 timer4
#elif TIMERNUM == 4

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T4CONbits.TCKPS = 0
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T4CONbits.TCKPS = 1
#elif TIMEDIV < (65536*64)
#define TMCLKDIV       64
#define TMCLKSEL       T4CONbits.TCKPS = 2
#elif TIMEDIV < (65536*256)
#define TMCLKDIV       256
#define TMCLKSEL       T4CONbits.TCKPS = 3
#else
#error "PIC24: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#if defined(__dsPIC30F__)
#define TIMERINIT {\
  T4CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPC5bits.T4IP = TIMERPR24;            /* priorit�s */\
  PR4 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC1bits.T4IE = 1;}                   /* timer IRQ enged�lyez�s */
#else
#define TIMERINIT {\
  T4CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* oszt� */\
  IPC6bits.T4IP = TIMERPR24;            /* priorit�s */\
  PR4 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC1bits.T4IE = 1;}                   /* timer IRQ enged�lyez�s */
#endif

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK IFS1bits.T4IF = 0

// timer megszak�t�s kiszolg�l� f�ggv�ny
#define TIMERINTPROCESS void __attribute__ ((interrupt, auto_psv)) _T4Interrupt(void)

//------------------------------------------------------------------------------
// PIC24 timer5
#elif TIMERNUM == 5

// oszt�
#if TIMEDIV < 65536
#define TMCLKDIV       1
#define TMCLKSEL       T5CONbits.TCKPS = 0
#elif TIMEDIV < (65536*8)
#define TMCLKDIV       8
#define TMCLKSEL       T5CONbits.TCKPS = 1
#elif TIMEDIV < (65536*64)
#define TMCLKDIV       64
#define TMCLKSEL       T5CONbits.TCKPS = 2
#elif TIMEDIV < (65536*256)
#define TMCLKDIV       256
#define TMCLKSEL       T5CONbits.TCKPS = 3
#else
#error "PIC24: timer frekvencia t�l alacsony"
#endif  // TIMEDIV

// timer kompar�torba ennyit kell berakni hogy a k�v�nt frekvencia legyen
#define TMCOMP (((TimerSrcClock/TMCLKDIV)+TIMECLK/2)/TIMECLK-1)

// timer inicializ�l�s
#if defined(__dsPIC30F__)
#define TIMERINIT {\
  T5CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* el?oszt� */\
  IPC5bits.T5IP = TIMERPR24;            /* priorit�s */\
  PR5 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC1bits.T5IE = 1;}                   /* timer IRQ enged�lyez�s */
#else
#define TIMERINIT {\
  T5CONbits.TCS = 0;                    /* forr�s �rajel = systemclock */\
  TMCLKSEL;                             /* el?oszt� */\
  IPC7bits.T5IP = TIMERPR24;            /* priorit�s */\
  PR5 = TMCOMP;                         /* id?z�t? be�ll�t�sa a bitid?h�z */\
  IEC1bits.T5IE = 1;}                   /* timer IRQ enged�lyez�s */
#endif

// timer megszak�t�s nyugt�z�s
#define TIMERIRQACK IFS1bits.T5IF = 0

// timer megszak�t�s kiszolg�l� f�ggv�ny
#define TIMERINTPROCESS void __attribute__ ((interrupt, auto_psv)) _T5Interrupt(void)

#else  // TIMERNUM
#error "PIC24: csak timer 1, 2, 3, 4, 5 v�laszthat�"
#endif // TIMERNUM

#endif // defined(__C30__)

//==============================================================================
// timer (v�ge)
//==============================================================================

#ifdef	__cplusplus
}
#endif

#endif	/* CHARLCD_TIMERS_H */

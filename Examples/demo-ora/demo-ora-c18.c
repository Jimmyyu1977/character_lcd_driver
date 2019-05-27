//******************************************************************************
// 8 bites 18Fxxx PIC �ra demo az charlcd tesztel�s�hez, c18 fordit�hoz
// k�sz�t�: Roberto Benjami (robertodebenjami(at)gmail(dot)com)
//
// charlcd.h sz�ks�ges be�ll�t�sai:
// - a be�ll�t�s b�rmi lehet, csak a felhaszn�lt mikrovez�rl� is t�mogassa,
//   (pl. a be�ll�tott I/O l�b �s a TIMER l�tezzen)
//   �s az egym�st�l f�gg� be�ll�t�sokn�l ne legyen �sszef�rhetetl�ns�g
//   (pl. kurzor csak egyszeri friss�t�si m�dban legyen),
// - adott esetben a felhaszn�lt I/O l�bak anal�gr�l -> digit�lisra t�rt�n� �t�ll�t�s�r�l gondoskodjunk!
//   (ezt vagy a hardvareprofile.h InitAD() defin�ci�j�ba, vagy ak�r ide a main ut�n is be�rhatjuk)
// - legal�bb 32 karakter legyen a kijelz�(k�)n, (legal�bb 2x16, LCDSTEREO eset�n 1x16)
//******************************************************************************

#include <p18cxxx.h>
#include <string.h>
#include <stdlib.h>
#include <timers.h>
#include "charlcd.h"

// bootloader n�lk�l: 0x0000, MCHPUSB bootloader: 0x0800, HID bootloader: 0x1000
#define RESET_VECTOR_ADDRESS             0x0

// �rajel forr�s frekvencia
#define  TimerSrcClock (1UL*SystemClock/4)

// 1000 megszak�t�s/sec
#define  TIMECLK    1000

// led l�bak defini�l�sa (ha nincs, t�r�lj�k a LEDS defin�ci�t)
#define  LEDS
#define  LED1       LATBbits.LATB0
#define  LED2       LATBbits.LATB1
#define  LEDIO1     TRISBbits.TRISB0
#define  LEDIO2     TRISBbits.TRISB1

// melyik id�z�t�n j�rjon az �ra ? (hogy ne �tk�zz�n az lcdchar �ltal felhaszn�ltal)
#if (LCDTIMERNUM == 0)
#define  CLOCKTIMERNUM   1
#else
#define  CLOCKTIMERNUM   0
#endif

unsigned int ev = 2013;
unsigned char honap = 12;
unsigned char nap = 24;
unsigned char ora = 23;
unsigned char perc = 58;
unsigned char masodperc = 0;
rom unsigned char honaphosszak[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

unsigned char temp;                     // timer kompar�tor seg�dv�ltoz�ja

#ifdef USERCHARSETCHANGE
USERCHARSETARRAY userchars;
#endif

#define TMCOMP ((TimerSrcClock+TIMECLK/2)/TIMECLK)

// kompar�tor korrekci�s �rt�k
#define  TMCOMPCOR0     7 
#define  TMCOMPCOR1     5 

// timer0 kompar�tor hi�ny�nak p�tl�sa
#define TIMER0COMPLESS  _asm \
  movlb  temp \
  movlw  65536+TMCOMPCOR0-TMCOMP \
  addwf  TMR0L, 0, 0 \
  movwf  temp, 1 \
  movlw  (65536+TMCOMPCOR0-TMCOMP) >> 8 \
  addwfc TMR0H, 1, 0 \
  movff  temp, TMR0L \
_endasm

// timer1 kompar�tor hi�ny�nak p�tl�sa
#define TIMER1COMPLESS  _asm \
  movlb  temp \
  movlw  65536+TMCOMPCOR1-TMCOMP \
  addwf  TMR1L, 0, 0 \
  movwf  temp, 1 \
  movlw  (65536+TMCOMPCOR1-TMCOMP) >> 8 \
  addwfc TMR1H, 1, 0 \
  movff  temp, TMR1L \
_endasm

//******************************************************************************
void init(void)
{
  // id�z�t� inicializ�l�sa
  #if CLOCKTIMERNUM == 0
  T0CONbits.T08BIT = 0;                 // TMR0 16bites
  T0CONbits.T0CS = 0;                   // �rajel forr�s
  T0CONbits.PSA = 1;                    // el�oszt� tilt�s
  INTCON2bits.TMR0IP = 1;               // magas priorit�s
  INTCONbits.TMR0IE = 1;                // TMR0 megszak�t�s eng
  T0CONbits.TMR0ON = 1;                 // bekapcs
  TMR0L = 65536+TMCOMPCOR0-TMCOMP;      // timerL kezd��rt�k felt�lt�se bitid�re
  TMR0H = (65536+TMCOMPCOR0-TMCOMP) >> 8;// timerH kezd��rt�k felt�lt�se bitid�re
  #else
  T1CONbits.RD16 = 1;                   // 16bites �r�s/olv
  T1CONbits.TMR1CS = 0;                 // forr�s �rajel = systemclock
  T1CONbits.T1CKPS1 = 0; T1CONbits.T1CKPS0 = 0; // oszt� 1:1
  IPR1bits.TMR1IP = 1;                  // magas priorit�s
  PIE1bits.TMR1IE = 1;                  // megszak�t�s enge�lyez�s
  T1CONbits.TMR1ON = 1;                 // bekapcs
  TMR1L = 65536+TMCOMPCOR1-TMCOMP;      // timerL kezd��rt�k felt�lt�se bitid�re
  TMR1H = (65536+TMCOMPCOR1-TMCOMP) >> 8;// timerH kezd��rt�k felt�lt�se bitid�re
  #endif

  // megszak�t�s be�ll�t�sok
  #if LCDTIMERPR18 == -1
  RCONbits.IPEN = 0;                    // k�tszint� IRQ tilt�s
  #else
  RCONbits.IPEN = 1;                    // k�tszint� IRQ eng
  #endif
  INTCONbits.GIEL = 1;                  // glob�lis LO IRQ eng
  INTCONbits.GIEH = 1;                  // glob�lis IRQ eng

  // ledek be�ll�t�sa
  #ifdef  LEDS
  LED1 = 0; LED2 = 0; LEDIO1 = 0; LEDIO2 = 0;
  #endif
}
//******************************************************************************
void main(void)
{
  unsigned char lastsec = 255;
  #ifdef USERCHARSETCHANGE
  unsigned char i, j;
  // felhaszn�l�i karakterk�szlet m�sol�sa RAM-ba
  memcpypgm2ram((char *)userchars, userromcharset, sizeof(USERCHARSETARRAY)); 
  #endif

  // ha sz�ks�g van AD l�b digit�lis I/O l�bra t�rt�n� �tkapcsol�sra
  #ifdef InitAD
  InitAD();
  #endif

  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t
  //                             "01020304050607080910111213141516"
  memcpypgm2ram((char *)LcdText, "   eeee.hh.nn       oo:pp:mm    ", 32); // kezd�k�perny� felt�lt�se

  init();
  while(1)
  {
    // folyamatos friss�t�si �zemm�d megszak�t�s n�lk�l
    #ifdef LCDMODECONTBUSY
    LcdProcess();
    #endif

    if(lastsec != masodperc)
    {
      // 2 led felv�ltva villogjon
      #ifdef  LEDS
      if(LED1)
        {LED1 = 0; LED2 = 1;}
      else
        {LED1 = 1; LED2 = 0;}
      #endif

      // Ha nincs automata villogtat�s 1sec a villog�si id�z�t�se
      #if (defined LCDBLINKCHAR) && ((!defined LCDMODECONTIRQ) || (LCDBLINKSPEED == 0))
      BlinkPhase = !BlinkPhase;
      #endif

      // j�r az �ra j�r
      if(masodperc == 60)
      {
        masodperc = 0;
        if(++perc == 60)
        {
          perc = 0;
          if(++ora == 24)
          {
            ora = 0;
            if(++nap > honaphosszak[honap - 1])
            {
              nap = 1;
              if(++honap == 13)
              {
                honap = 1;
                ev++;
      } } } } }
      lastsec = masodperc;

      // sz�mjegyek ki�r�sa
      itoa(ev, (char *)LcdText + 3);
      btoa(honap, (char *)LcdText + 8);
      btoa(nap, (char *)LcdText + 11);
      btoa(ora, (char *)LcdText + 20);
      btoa(perc, (char *)LcdText + 23);
      btoa(masodperc, (char *)LcdText + 26);

      // egysz�mjegy� sz�mok k�tsz�mjegy�v� alak�t�sa
      if(!LcdText[9]) {LcdText[9]  = LcdText[8];  LcdText[8]  = '0';}
      if(!LcdText[12]){LcdText[12] = LcdText[11]; LcdText[11] = '0';}
      if(!LcdText[21]){LcdText[21] = LcdText[20]; LcdText[20] = '0';}
      if(!LcdText[24]){LcdText[24] = LcdText[23]; LcdText[23] = '0';}
      if(!LcdText[27]){LcdText[27] = LcdText[26]; LcdText[26] = '0';}

      // *toa �ltal fel�l�rt �r�sjelek p�tl�sa
      LcdText[7] = '.';
      LcdText[10] = '.';
      LcdText[22] = ':';
      LcdText[25] = ':';

      // sz�mok k�z�tti kett�spont villogtat�s
      #ifdef LCDBLINKCHAR
      LcdBlinkChar(22);
      LcdBlinkChar(25);
      #endif

      // felhaszn�l� �ltal defini�lt karakterek a sarkokban
      #ifdef USERCHARSET
      LcdText[0] = 8;
      LcdText[1] = 10;
      LcdText[16] = 9;
      LcdText[17] = 11;
      LcdText[14] = 12;
      LcdText[15] = 14;
      LcdText[30] = 13;
      LcdText[31] = 15;
      #endif

      // kurzor l�ptet�se �s kurzort�pus v�ltogat�sa
      #ifdef LCDCURSOR
      LcdCursorPos++;
      if(LcdCursorPos >= (1UL * LCDWIDTH * LCDLINES))
        LcdCursorPos = 0;
      if((LcdCursorPos & 0b11) == 0b00) LcdCursorOff();
      else if((LcdCursorPos & 0b11) == 0b01) {LcdCursorUnBlink(); LcdCursorOn();}
      else if((LcdCursorPos & 0b11) == 0b10) {LcdCursorBlink(); LcdCursorOn();}
      else if((LcdCursorPos & 0b11) == 0b11) {LcdCursorUnBlink(); LcdCursorOn();}
      #endif

      // karakterk�szlet shiftel�se �s felt�lt�se (csak ha karakterk�szlet v�ltoztat�s�nak lehet�s�ge engedve van)
      #ifdef USERCHARSETCHANGE
      j = userchars[63];
      i = 63;
      while(i-- > 0)
        userchars[i + 1] = userchars[i];
      userchars[0] = j;
      LcdChangeCharset(userchars);
      #endif // USERCHARSETCHANGE

      // egyszeri friss�t�si �zemm�dban megjelen�t�s
      #if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEWAIT) || (defined LCDMODEONCEIRQ)) 
      LcdRefreshAll();
      #endif
    }
  }
}

#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode()
{
  #if ((!defined LCDUSERTIMER) && (LCDTIMERPR18 == 0) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ)))
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode()
{
  static unsigned int msec = 0;

  #if CLOCKTIMERNUM == 0
  if(INTCONbits.TMR0IF)                 // timer0 interrupt ?
  {
    TIMER0COMPLESS;                     // timer0 kompar�tor hi�ny�nak p�tl�sa
    INTCONbits.TMR0IF = 0;              // nyugt�z�s
    msec++;
    if(msec >= 1000)
    {
      masodperc++;
      msec = 0; 
    }
    #if (defined LCDUSERTIMER)
    LcdIntProcess();                    // saj�t megszak�t�skezel�vel �s id�z�t�skezel�vel haszn�lva
    #endif
  }
  #else
  if(PIR1bits.TMR1IF)                   // timer1 interrupt ?
  {
    TIMER1COMPLESS;                     // timer1 kompar�tor hi�ny�nak p�tl�sa
    PIR1bits.TMR1IF = 0;                // nyugt�z�s
    msec++;
    if(msec >= 1000)
    {
      masodperc++;
      msec = 0; 
    }
    #if (defined LCDUSERTIMER)
    LcdIntProcess();                    // saj�t megszak�t�skezel�vel �s id�z�t�skezel�vel haszn�lva
    #endif
  }
  #endif

  #if ((LCDTIMERPR18 == -1) || (LCDTIMERPR18 == 1)) && (!defined LCDUSERTIMER)  && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
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

//******************************************************************************
// 8 bites 18Fxxx PIC �ra demo az charlcd tesztel�s�hez, htc18, xc8 fordit�hoz
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

#if defined(__XC__)
#include <xc.h>
#else
#include <htc.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <timers.h>
#include "charlcd.h"

// �rajel forr�s frekvencia
#define  TimerSrcClock (1UL*SystemClock/4)

// 1000 megszak�t�s/sec
#define  TIMECLK 1000

// led l�bak defini�l�sa (ha nincs, kommentelj�k ki)
#define  LED1       B, 0
#define  LED2       B, 1

// melyik id�z�t�n j�rjon az �ra ? (hogy ne �tk�zz�n az lcdchar �ltal felhaszn�ltal)
#if (LCDTIMERNUM == 0)
#define  CLOCKTIMERNUM   1
#else
#define  CLOCKTIMERNUM   0
#endif

unsigned short ev = 2013;
unsigned char honap = 12;
unsigned char nap = 24;
unsigned char ora = 23;
unsigned char perc = 58;
unsigned char masodperc = 0;
const unsigned char honaphosszak[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

unsigned char temp;                     // timer kompar�tor seg�dv�ltoz�ja

#ifdef USERCHARSETCHANGE
USERCHARSETARRAY userchars;
#endif

#define TMCOMP ((TimerSrcClock+TIMECLK/2)/TIMECLK)

// kompar�tor korrekci�s �rt�k
#define  TMCOMPCOR0     7 
#define  TMCOMPCOR1     5 

// timer0 kompar�tor hi�ny�nak p�tl�sa
#define TIMER0COMPLESS { \
  temp = (65535+TMCOMPCOR0-TMCOMP) + TMR0L; \
  WREG = ((65535+TMCOMPCOR0-TMCOMP) >> 8); \
  asm("addwfc (TMR0H), 1, 0"); \
  TMR0L = temp; \
}

// timer1 kompar�tor hi�ny�nak p�tl�sa
#define TIMER1COMPLESS { \
  temp = (65535+TMCOMPCOR1-TMCOMP) + TMR1L; \
  WREG = ((65535+TMCOMPCOR1-TMCOMP) >> 8); \
  asm("addwfc (TMR1H), 1, 0"); \
  TMR1L = temp; \
}

#define GLUE(a, b)   a##b
#define SET_(what, p, m)       GLUE(what, p) |= (1 << (m))
#define CLR_(what, p, m)       GLUE(what, p) &= ~(1 << (m))
#define GET_(/* PORT, */ p, m) GLUE(PORT, p) & (1 << (m))
#define IOIN(x)                SET_(TRIS, x)
#define IOOUT(x)               CLR_(TRIS, x)
#define SET(x)                 SET_(LAT, x)
#define CLR(x)                 CLR_(LAT, x)
#define GET(x)                 GET_(x)

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
  unsigned char lastsec = 255;

  #ifdef USERCHARSETCHANGE
  unsigned char i, j;
  #endif

  // ha sz�ks�g van AD l�b digit�lis I/O l�bra t�rt�n� �tkapcsol�sra
  #ifdef InitAD
  InitAD();
  #endif

  // felhaszn�l�i karakterk�szlet m�sol�sa RAM-ba (csak ha enged�lyezve van)
  #ifdef USERCHARSETCHANGE
  memcpy((char *)userchars, userromcharset, sizeof(USERCHARSETARRAY)); 
  #endif
  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t
  //                      "01020304050607080910111213141516"
  memcpy((char *)LcdText, "   eeee.hh.nn       oo:pp:mm    ", 32); // kezd�k�perny� felt�lt�se

  init();
  while(1)
  {
    // folyamatos friss�t�si �zemm�d megszak�t�s n�lk�l
    #ifdef LCDMODECONT
    LcdProcess();
    #endif

    if(lastsec != masodperc)
    {
      // 2 led felv�ltva villogjon
      #ifdef  LED1
      if(GET(LED1)) 
      {
        CLR(LED1);
        #ifdef  LED1
        SET(LED2);
        #endif
      }
      else 
      {
        SET(LED1);
        #ifdef  LED1
        CLR(LED2);
        #endif
      }
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
      utoa((char *)LcdText + 3, ev, 10);
      utoa((char *)LcdText + 8, honap, 10);
      utoa((char *)LcdText + 11, nap, 10);
      utoa((char *)LcdText + 20, ora, 10);
      utoa((char *)LcdText + 23, perc, 10);
      utoa((char *)LcdText + 26, masodperc, 10);

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

void interrupt low_priority irql(void)
{
  #if (!defined LCDUSERTIMER) && (LCDTIMERPR18 == 0) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

void interrupt irqh(void)
{
  static unsigned int msec = 0;

  #if (!defined LCDUSERTIMER) && ((LCDTIMERPR18 == -1) || (LCDTIMERPR18 == 1)) && (defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ)
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif

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
}

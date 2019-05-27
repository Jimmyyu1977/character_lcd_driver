//******************************************************************************
// 8 bites 16Fxxx PIC �ra demo a charlcd tesztel�s�hez, htc, xc8 fordit�khoz
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
#include "charlcd.h"

// �rajel forr�s frekvencia
#define TimerSrcClock (1UL*SystemClock/4)

// 100 megszak�t�s/sec
#define  TIMECLK  100

// led l�bak defini�l�sa (ha nincs, kommentelj�k ki)
#define  LED1       B, 0
#define  LED2       B, 1

// melyik id�z�t�n j�rjon az �ra ? (hogy ne �tk�zz�n az lcdchar �ltal felhaszn�ltal)
#if (LCDTIMERNUM == 1)
#define  CLOCKTIMERNUM   2
#else
#define  CLOCKTIMERNUM   1
#endif

unsigned int ev = 2013;
unsigned char honap = 12;
unsigned char nap = 24;
unsigned char ora = 23;
unsigned char perc = 58;
volatile unsigned char masodperc = 0;
unsigned char honaphosszak[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#ifdef USERCHARSETCHANGE
USERCHARSETARRAY userchars;
#endif

// oszt�sar�ny
#define  TIMEDIV ((TimerSrcClock+TIMECLK/2)/TIMECLK)

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
  // id�z�t� inicializ�l�sa
  #if CLOCKTIMERNUM == 1
  #if TIMEDIV < 65536
  #define TMCOMP (((TimerSrcClock/1)+TIMECLK/2)/TIMECLK)
  #define TMCOMPCOR     65540
  T1CONbits.T1CKPS = 0;
  #elif TIMEDIV < (65536*8)
  #define TMCOMP (((TimerSrcClock/8)+TIMECLK/2)/TIMECLK)
  #define TMCOMPCOR     65536
  T1CONbits.T1CKPS = 3;
  #else
  #error "PIC16: ez a frekvencia t�l alacsony"
  #endif // TIMEDIV

  T1CONbits.TMR1CS = 0;                 // forr�s �rajel = systemclock
  PIE1bits.TMR1IE = 1;                  // megszak�t�s enged�lyez�s
  TMR1 = TMCOMPCOR-TMCOMP;              // timer kezd��rt�k felt�lt�se bitid�re
  T1CONbits.TMR1ON = 1;                 // bekapcs
  #else  // CLOCKTIMERNUM == 1

  #if TIMEDIV < 256
  T2CONbits.T2CKPS = 0; T2CONbits.TOUTPS = 0;
  PR2 = (((TimerSrcClock/1)+TIMECLK/2)/TIMECLK-1);
  #elif TIMEDIV < (256*16)
  T2CONbits.T2CKPS = 2; T2CONbits.TOUTPS = 0;
  PR2 = (((TimerSrcClock/16)+TIMECLK/2)/TIMECLK-1);
  #elif TIMEDIV < (256*16*16)
  T2CONbits.T2CKPS = 2; T2CONbits.TOUTPS = 15;
  PR2 = (((TimerSrcClock/256)+TIMECLK/2)/TIMECLK-1);
  #else
  #error "PIC16: ez a frekvencia t�l alacsony"
  #endif
  PIE1bits.TMR2IE = 1;                  /* megszak�t�s enge�lyez�s */\
  T2CONbits.TMR2ON = 1;                 // Timer2 ON
  #endif // else CLOCKTIMERNUM == 1

  INTCONbits.PEIE = 1;                  // perif�ria IRQ eng
  INTCONbits.GIE = 1;                   // glob�lis IRQ eng
  
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
      // 2 led m�sodpercenk��nt felv�ltva villogjon
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

       // Ha nincs automata villogtat�sn 1sec a villog�si id�z�t�se
      #if (defined LCDBLINKCHAR) && (!defined LCDMODECONTIRQ) || (LCDBLINKSPEED == 0)
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

      // utoa �ltal #0-val fel�l�rt �r�sjelek p�tl�sa
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


//******************************************************************************
// PIC16 megszak�t�s kiszolg�l�s
static void interrupt isr(void)
{
  static unsigned char msec10 = 0;
  
  #if CLOCKTIMERNUM == 1
  if(PIR1bits.TMR1IF)                   // timer1 interrupt ?
  {
    TMR1 += (TMCOMPCOR-TMCOMP);         // timer1 kompar�tor hi�ny�nak p�tl�sa
    PIR1bits.TMR1IF = 0;                // nyugt�z�s
    msec10++;
    if(msec10 >= 100)
    {
      masodperc++;
      msec10 = 0; 
    }
  }
  #else
  if(PIR1bits.TMR2IF)                   // timer2 interrupt ?
  {
    PIR1bits.TMR2IF = 0;                // nyugt�z�s
    msec10++;
    if(msec10 >= 100)
    {
      masodperc++;
      msec10 = 0; 
    }

    #if (defined LCDUSERTIMER)
    LcdIntProcess();                    // saj�t megszak�t�skezel�vel �s id�z�t�skezel�vel haszn�lva
    #endif
  }
  #endif

  #if (!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
  LcdIntProcess();                      // interrupt m�d eset�n friss�tj�k a kijelz� tartalm�t
  #endif
}

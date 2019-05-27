//******************************************************************************
// 16 bites PIC �ra demo az charlcd tesztel�s�hez, c30, xc16 fordit�hoz
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

// include, �rajel forr�s frekvencia
#if defined(__dsPIC30F__)
#include <p30Fxxxx.h>
#define TimerSrcClock (1UL*SystemClock/4)
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#define TimerSrcClock (1UL*SystemClock/2)
#elif defined(__PIC24F__)
#include <p24Fxxxx.h>
#define TimerSrcClock (1UL*SystemClock/2)
#elif defined(__PIC24FK__)
#include <p24Fxxxx.h>
#define TimerSrcClock (1UL*SystemClock/2)
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#define TimerSrcClock (1UL*SystemClock/2)
#endif

#include <timer.h>
#include <string.h>
#include <stdlib.h>
#include "charlcd.h"

// 1000 megszak�t�s/sec
#define  TIMECLK  1000

// melyik id�zit�n j�rjon az �ra? (hogy ne �tk�zz�n az lcdchar �ltal felhaszn�ltal)
#if (LCDTIMERNUM == 1)
#define  CLOCKTIMERNUM   2
#else
#define  CLOCKTIMERNUM   1
#endif

// 1msec megszak�t�s
#define  TMCOMP (((TimerSrcClock)+1000/2)/1000-1)

unsigned int ev = 2013;
unsigned char honap = 12;
unsigned char nap = 27;
unsigned char ora = 23;
unsigned char perc = 58;
volatile unsigned char masodperc = 0;
unsigned char honaphosszak[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#ifdef USERCHARSETCHANGE
USERCHARSETARRAY userchars;
#endif

// oszt�sar�ny
#define  TIMEDIV ((TimerSrcClock+TIMECLK/2)/TIMECLK-1)

//******************************************************************************
void init(void)
{
  // PIC24 TIMER2 id�z�t� inicializ�l�sa
  #if CLOCKTIMERNUM == 1
  OpenTimer1(T1_ON & T1_GATE_OFF & T1_PS_1_1 & T1_SOURCE_INT & T1_INT_ON, TIMEDIV);
  SetPriorityIntT1(1);                  // legkisebb priorit�s
  EnableIntT1;
  #else
  OpenTimer2(T2_ON & T2_GATE_OFF & T2_PS_1_1 & T2_32BIT_MODE_OFF & T2_SOURCE_INT & T2_INT_ON, TIMEDIV);
  SetPriorityIntT2(1);                  // legkisebb priorit�s
  EnableIntT2;
  #endif
}
//******************************************************************************

int main(void)
{
  static unsigned char lastsec;
  #ifdef USERCHARSETCHANGE
  unsigned char i, j;
  // felhaszn�l�i karakterk�szlet m�sol�sa RAM-ba (csak ha enged�lyezve van)
  memcpy((char *)userchars, userromcharset, sizeof(USERCHARSETARRAY)); 
  #endif

  // ha sz�ks�g van AD l�b digit�lis I/O l�bra t�rt�n� �tkapcsol�sra
  #ifdef InitAD
  InitAD();
  #endif

  LcdInit();                            // m�k�d�sre b�rjuk az LCD-t
  init();

  //                     "01020304050607080910111213141516"
  memcpy((char*)LcdText, "   eeee.hh.nn       oo:pp:mm    ", 32);
  lastsec = 255;
  while(1)
  {
    // folyamatos friss�t�si �zemm�d megszak�t�s n�lk�l
    #ifdef LCDMODECONT
    LcdProcess();
    #endif

    if(lastsec != masodperc)
    {
      // Ha nincs automata villogtat�s 1sec a villog�si id�z�t�se
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

      itoa((char*)LcdText + 3, ev, 10);
      itoa((char*)LcdText + 8, honap, 10);
      itoa((char*)LcdText + 11, nap, 10);
      itoa((char*)LcdText + 20, ora, 10);
      itoa((char*)LcdText + 23, perc, 10);
      itoa((char*)LcdText + 26, masodperc, 10);

      // egysz�mjegy� sz�mok k�tsz�mjegy�v� alak�t�sa
      if(!LcdText[9]) {LcdText[9]  = LcdText[8];  LcdText[8]  = '0';}
      if(!LcdText[12]){LcdText[12] = LcdText[11]; LcdText[11] = '0';}
      if(!LcdText[21]){LcdText[21] = LcdText[20]; LcdText[20] = '0';}
      if(!LcdText[24]){LcdText[24] = LcdText[23]; LcdText[23] = '0';}
      if(!LcdText[27]){LcdText[27] = LcdText[26]; LcdText[26] = '0';}

      // itoa �ltal fel�l�rt �r�sjelek p�tl�sa
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

      // karakterk�szlet shiftel�se �s felt�lt�se (csak ha karakterk�szlet v�ltoztat�s�nak lehet?s�ge engedve van)
      #ifdef USERCHARSETCHANGE
      j = userchars[63];
      i = 63;
      while(i--)
        userchars[i + 1] = userchars[i];
      userchars[0] = j;
      LcdChangeCharset(userchars);
      #endif // USERCHARSETCHANGE

      // egyszeri friss�t�si �zemm�dban megjelen�t�s
      #if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ)) 
      LcdRefreshAll();
      #endif
    }
  }
}

//******************************************************************************
// PIC24 megszak�t�s kiszolg�l�s
#if CLOCKTIMERNUM == 1
void __attribute__ ((interrupt, auto_psv)) _T1Interrupt(void)
{
  volatile static unsigned int msec = 0;
  IFS0bits.T1IF = 0;
  msec++;
  if(msec == 1000)
  {
    masodperc++;
    msec = 0; 
  }
  #if (defined LCDUSERTIMER)
  LcdIntProcess();                      // saj�t megszak�t�skezel�vel �s id�z�t�skezel?vel haszn�lva
  #endif
}
#else
void __attribute__ ((interrupt, auto_psv)) _T2Interrupt(void)
{
  volatile static unsigned int msec = 0;
  IFS0bits.T2IF = 0;
  msec++;
  if(msec == 1000)
  {
    masodperc++;
    msec = 0; 
  }
  #if (defined LCDUSERTIMER)
  LcdIntProcess();                      // saj�t megszak�t�skezel�vel �s id�z�t�skezel�vel haszn�lva
  #endif
}
#endif

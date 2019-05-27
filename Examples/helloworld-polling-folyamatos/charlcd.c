//------------------------------------------------------------------------------
// K�sz�t�: Roberto Benjami (robertodebenjami(kukac)gmail(pont)com)
// Verzi�:  v0.65 (2017.11)

//------------------------------------------------------------------------------
// M�dos�t�sok:
// 0.4 els� k�zz�tett verzi�

// 0.41:
// - jav: Dupla vez�rl� felhaszn�l�i karakterk�szlet felt�lt�s jav�t�sa (SETDDRAMADDR -> SETCGRAMADDR).
// - opt: LcdWrite2()-ben felesleges RS-t �jra be�ll�tani, mikor LcdWrite()-ban m�r be�ll�t�sra ker�lt.
// - opt: LcdWrite2()-ben felesleges az adatir�nyl�bakat �jra be�ll�tani, mikor LcdWrite()-ban m�r be�ll�t�sra ker�lt.

// 0.42:
// - opt: Veremm�veletek cs�kkent�se miatt LcdWrite() param�ter n�lk�liv� alak�tva.
//        (ch �gy is ott van glob�lis v�ltoz�k�nt, RS l�b be�ll�t�sa, meg a h�v�s el�tt t�rt�nik)
// - jav: Egysoros kijelz� eset�n m�r egy sorosk�nt is inicializ�l (LCD_LINES -> LCDLINES).

// 0.43
// - jav: C18 ford�t� "LCDWIDTH * LCDLINES + 1" kifejez�st, ha > 127 rosszul �rt�keli ki -> 1UL szorz�val jav�tva
// - jav: C18 ford�t� "LCDWIDTH * LCDLINES + 7) / 8" kifejez�st rosszul �rt�kelheti ki -> 1UL szorz�val jav�tva

// 0.44
// - �j:  Megszak�t�sos m�dban lehet�s�g van a friss�t�st ki/bekapcsolni (pl. ha pontos id�z�t�sre van sz�ks�g)
//        a LcdRefreshStart(), LcdRefreshStop() f�ggv�nyh�v�ssal.
//        (ekkor az esetlegesen villog� karakterek is az aktu�lis �llapotban maradnak)

// 0.45
// - �j:  Lehet�s�g van a folyamatos mellett, egyszeri friss�t�si �zemm�dra is.
//        Ekkor LcdRefreshAll() h�v�s�val lehet a kijelz� tartalm�t �jra�rni.
//        Megszak�t�sos m�dban LcdRefreshed() visszat�r� �rt�k�b�l meg�llap�that� befejezte-e a friss�t�st.
//        (megszak�t�s n�lk�li m�dban a f�ggv�nyben marad a friss�t�s v�g�ig, �gy erre nincs sz�ks�g)
// - jav: AVR timer2: #ifdef TIMER0_COMP_vect -> #ifdef TIMER2_COMP_vect
// - �j:  ATMEGA8 compar�tor n�lk�li TIMER0-nak a haszn�lati lehet�s�ge
// - jav: FPS->CPS timer sz�m�t�s�hoz a DDRAM �ll�t�si ciklust is bekalkul�lja
// - �j:  LCDZEROCHANGETEXT defin�ci�val (ez a charlcd.c -ben van kiv�telesen) az LcdText-ben is sz�k�zre cser�li a #0 karaktereket.

// 0.46
// - �j:  LCDSTEREO defin�ci�t haszn�lva 2db kijelz�t is haszn�lhatunk.

// 0.47
// - �j:  LCDCURSOR defin�ci�t haszn�lva lehet�s�g van a kurzor haszn�lat�ra (csak egyszeri friss�t�si �zemm�dban).

// 0.48
// - �j:  USERCHARSETCHANGE defin�ci�t haszn�lva lehet�s�g van fut�s alatt felhaszn�l�i karakterk�szletet cser�lni.

// 0.49
// - �j:  HI-TECH C18 ford�t� hozz�ad�sa
// - �j:  �jabb �zemm�d (megszak�t�s n�lk�l egyszeri friss�t�si �zemm�dban BUSY flag figyel�s helyett be�ll�that� v�rakoz�ssal)
// - �j:  Az 5 �zemm�d k�z�tt az elnevez�s�vel lehet v�lasztani

// 0.50
// - �j:  �sszef�rhetetlen be�ll�t�sok kisz�r�se
// - �j:  PIC18: v�laszthat� priorit�s n�lki, alacsony, magas priorit�s haszn�lata megszak�t�sos m�dban
// - �j:  Megszak�t�s m�dban saj�t megszak�t�skezel�ssel megoldani az LCD friss�t�si elj�r�s�nak hiv�s�t
// - �j:  TOUTPS - T2OUTPS ford�t�si hiba�zenet eset�n k�nny� �t�ll�s a m�sik verzi�ra
// - jav: Karakterk�szlet nem 5x7, hanem 5x8 pixeles
// - jav: USERCHARSET �s USERCHARSETCHANGE f�ggetlen�t�se egym�st�l

// 0.51
// - jav: PIC18: TIME2DIV oszt� jav�t�sa 2-es  osz�si ar�ny eset�n
// - jav: T2OUTPS elnevez�si elt�r�sek miatti hiba�zenet megsz�ntet�se direkt regiszter �r�ssal

// 0.52
// - opt: AVR saj�t karakterk�szletet ne m�solja �t a ROM-b�l a RAM-ba.
// - jav: AVR Timer0 LcdRefreshStart - Stop dupla defini�l�s megsz�ntet�se
// - jav: PIC18: Timer1 inicializ�l�sn�l T1 sz�ml�l� helyett T3-ba t�lt�tte a kezd��rt�ket

// 0.60
// - �j:  L�bhozz�rendel�sek megad�s�nak megv�ltoztat�sa (egy sorban a PORT �s a l�b, pl: #define LCDE B, 3)

// 0.61
// - jav: A 0.60-asban elrontott inicial�si hiba jav�t�sa

// 0.62
// - opt: E l�b magasra �ll�t�sa el�tt nem sz�ks�ges v�rakoz�si id� (kiv�ve)
// - �j:  LCDDTINm LCDDTOUT, LCDDT0TO3, LCDDT4TO7, LCDDT0TO7 optimaliz�l�si lehet�s�ge charlcd.h-ban
// - �j:  #define LCDNOP ... a ford�t� optimaliz�l�s�nak megtart�sa miatt

// 0.63
// - jav: PIC18 TimecCompless ASM -> C, hogy a ford�t� ne kapcsolja le az optimaliz�ci�t
// - jav: LCDRSPIN makro haszn�lat javit�sa GPIOX_CLRBIT(LCDRS) illetve GPIOX_SETBIT(LCDRS)-re
// - �j:  #define LCDGLOBALINTAUTOSTART inicializ�l�skor bekapcsolja-e az INT-et
// - �j:  karakterk�szlet csere ROM ter�letr�l (#define USERCHARSETCHANGEROM)

// 0.64
// - �j:  XC8 fordito felv�tele
// - �j:  automatikus optimaliz�l�s, ha egym�st k�vet� l�bakhoz rendelj�k az adatl�bakat

// 0.65
// - mod: LCDE2 definicio megad�sa jelenti a k�t kijelz� haszn�lat�t, LCDSTEREO definicio sz�m�zve
//        (a 80 karakteresn�l nagyobb kijelz� is kett� kijelz�k�nt m�k�dik)
// - mod: LCDRW definicio megl�te hat�rozza meg, hogy az a l�b haszn�lva lesz-e (LCDRWUSED sz�m�zve)
// - mod: LCDDT0..LCDDT3 definicio megl�te eset�n automatikusan 8 bites modban fog m�k�dni (LCD4BITMODE definicio megl�te hat�rozza meg, hogy az a l�b haszn�lva lesz-e (LCDRWUSED sz�m�zve)
// - jav: LcdRefreshStart: a timer alap�llapotbol induljon, hogy az els� k�rben ne legyen r�videbb az id�zit�s

//------------------------------------------------------------------------------
// Ha ez defini�lva van, a friss�t�s sor�n az Lcdtext[] t�mbben is kicser�lgeti a #0 karaktereket sz�k�zre
// (�ltal�ban nincs erre sz�ks�g, csak ha m�s c�lra is fel szeretn�nk haszn�lni a sz�veget)
// #define LCDZEROCHANGETEXT
//------------------------------------------------------------------------------
// Szoftver szimul�torhoz: kihagyja az LcdBusy f�ggv�nyt (figyelem: enged�lyezve a kijelz� nem fog m�k�dni!)
// #define SOFTSIMBUSY
// Szoftver szimul�torhoz: inicializ�l�skor a v�rakoz�si ciklusok kihagy�sa (figyelem: enged�lyezve a kijelz� nem fog m�k�dni!)
// #define SOFTSIMFAST
//------------------------------------------------------------------------------

// processzorcsal�d include
#if defined(__AVR__)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/ina90.h>
#elif (defined(_PIC14) || defined(_PIC14E))
#include <pic.h>
#elif defined(__XC__)
#include <xc.h>
#elif defined(__PICC18__)
#include <htc.h>
#elif defined(__18CXX)
#include <p18cxxx.h>
#elif defined(__C30__)
#if defined(__dsPIC30F__)
#include <p30Fxxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#elif defined(__PIC24F__)
#include <p24Fxxxx.h>
#elif defined(__PIC24FK__)
#include <p24Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#else
#error "ismeretlen processzorcsal�d vagy forditoprogram"
#endif
#endif // defined(xxx proci)

#include "charlcd.h"

// ha LCDDT0..LCDDT3 l�tezik, akkor 8 bites mod
#if (defined(LCDDT0) && defined(LCDDT1) && defined(LCDDT2) && defined(LCDDT3))
#define LCD8BITMODE
#else
#define LCD4BITMODE
#endif

//------------------------------------------------------------------------------
// �sszef�rhetetlen be�ll�t�sok kisz�r�se
#if !((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ) || (defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ)) 
#error "Valamelyik �zemm�dot v�laszd ki!"
#endif

#ifdef LCDMODEONCEBUSY
#if ((defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ) || (defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ)) 
#error "Csak 1 �zemm�dot �ll�ts be!"
#endif
#ifndef LCDRW
#error "LCD MODEONCEBUSY �zemm�dban k�telez� haszn�lni az RW l�bat!"
#endif
#ifdef LCDUSERTIMER
#error "LCD USERTIMER csak megszak�t�sos m�dban haszn�lhat�"
#endif
#endif

#ifdef LCDMODEONCEDELAY
#if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEIRQ) || (defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
#error "Csak 1 �zemm�dot �ll�ts be!"
#endif
#ifndef LCDEXECUTIONTIME
#error "LCD MODEONCEDELAY �zemm�dban LCDEXECUTIONTIME �rt�k�t is meg kell adni!"
#endif
#ifdef LCDUSERTIMER
#error "LCD USERTIMER csak megszak�t�sos m�dban haszn�lhat�"
#endif
#endif

#ifdef LCDMODEONCEIRQ
#if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
#error "Csak 1 �zemm�dot �ll�ts be!"
#endif
#endif

#ifdef LCDMODECONTBUSY
#if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
#error "Csak 1 �zemm�dot �ll�ts be!"
#endif
#ifndef LCDRW
#error "LCD MODECONTBUSY �zemm�dban k�telez� haszn�lni az RW l�bat!"
#endif
#ifdef LCDCURSOR
#error "LCD MODECONTBUSY �zemm�dban LCD CURSOR nem haszn�lhat�!"
#endif
#ifdef LCDUSERTIMER
#error "LCD USERTIMER csak megszak�t�sos m�dban haszn�lhat�"
#endif
#endif

#ifdef LCDMODECONTIRQ
#if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ) || (defined LCDMODECONTBUSY))
#error "Csak 1 �zemm�dot �ll�ts be!"
#endif
#ifdef LCDCURSOR
#error "LCD MODECONTIRQ �zemm�dban LCD CURSOR nem haszn�lhat�!"
#endif
#endif

#if (LCDLINES != 1) && (LCDLINES != 2) && (LCDLINES != 4)
#error "A driver csak 1, 2, 4 soros kijelz�vel haszn�lhat�!"
#endif

#if (LCDWIDTH > 40)
#error "A driver maximum 40 karakter sz�les kijelz�vel haszn�lhat�!"
#endif

//------------------------------------------------------------------------------
// rendszer�rajel
#ifndef SystemClock
#error  "Nincs rendszer�rajel frekvencia defini�lva!"
#endif

// ha CPU �rajel frekvencia nincs megadva akkor a rendszer�rajellel megegyezik
#ifndef CpuClock
#define CpuClock SystemClock
#endif

// processzorcsal�d szerinti �rajel oszt�sok
#if defined(__AVR__)
#define Cpu8bit
#define CpuMips (1UL*CpuClock)
#define DELAYNUM (CpuMips/1000/14)
#define LCDPROCCYCLENULL   109
#define LCDPROCCYCLENULLSTEREO   192
#define TimerSrcClock (1UL*SystemClock)
#elif (defined(_PIC14) || defined(_PIC14E))
#define Cpu8bit
#define CpuMips (1UL*CpuClock/4)
#define DELAYNUM (CpuMips/1000/10)
#define LCDPROCCYCLENULL   61
#define LCDPROCCYCLENULLSTEREO   100
#define TimerSrcClock (1UL*SystemClock/4)
#elif defined(__18CXX)
#define Cpu8bit
#define CpuMips (1UL*CpuClock/4)
#define DELAYNUM (CpuMips/1000/8)
#define LCDPROCCYCLENULL   67
#define LCDPROCCYCLENULLSTEREO   107
#define TimerSrcClock (1UL*SystemClock/4)
#elif (defined(__XC__) || defined(__PICC18__))
#define Cpu8bit
#define CpuMips (1UL*CpuClock/4)
#define DELAYNUM (CpuMips/1000/8)
#define LCDPROCCYCLENULL   61
#define LCDPROCCYCLENULLSTEREO   100
#define TimerSrcClock (1UL*SystemClock/4)
#elif defined(__C30__)
#if defined(__dsPIC30F__)
#define Cpu16bit
#define CpuMips (1UL*CpuClock/4)
#define DELAYNUM (CpuMips/1000/7)
#define LCDPROCCYCLENULL   90
#define LCDPROCCYCLENULLSTEREO   100
#define TimerSrcClock (1UL*SystemClock/4)
#elif defined(__dsPIC33F__)
#define Cpu16bit
#define CpuMips (1UL*CpuClock/2)
#define DELAYNUM (CpuMips/1000/7)
#define LCDPROCCYCLENULL   90
#define LCDPROCCYCLENULLSTEREO   159
#define TimerSrcClock (1UL*SystemClock/2)
#elif defined(__PIC24F__)
#define Cpu16bit
#define CpuMips (1UL*CpuClock/2)
#define DELAYNUM (CpuMips/1000/7)
#define LCDPROCCYCLENULL   90
#define LCDPROCCYCLENULLSTEREO   159
#define TimerSrcClock (1UL*SystemClock/2)
#elif defined(__PIC24FK__)
#define Cpu16bit
#define CpuMips (1UL*CpuClock/2)
#define DELAYNUM (CpuMips/1000/7)
#define LCDPROCCYCLENULL   90
#define LCDPROCCYCLENULLSTEREO   159
#define TimerSrcClock (1UL*SystemClock/2)
#elif defined(__PIC24H__)
#define Cpu16bit
#define CpuMips (1UL*CpuClock/2)
#define DELAYNUM (CpuMips/1000/7)
#define LCDPROCCYCLENULL   90
#define LCDPROCCYCLENULLSTEREO   159
#define TimerSrcClock (1UL*SystemClock/2)
#else
#error "ismeretlen processzort�pus"
#endif
#endif // defined(xxx proci)

#define GPIOX_PORTTONUM_A     1
#define GPIOX_PORTTONUM_B     2
#define GPIOX_PORTTONUM_C     3
#define GPIOX_PORTTONUM_D     4
#define GPIOX_PORTTONUM_E     5
#define GPIOX_PORTTONUM_F     6
#define GPIOX_PORTTONUM_G     7
#define GPIOX_PORTTONUM_H     8
#define GPIOX_PORTTONUM_J     9
#define GPIOX_PORTTONUM_K     10
#define GPIOX_PORTTONUM_L     11
#define GPIOX_PORTTONUM_M     12
#define GPIOX_PORTNUM_(p, m)  GPIOX_PORTTONUM_ ## p
#define GPIOX_PORTNAME_(p, m) p
#define GPIOX_PIN_(p, m)      m

#if defined(__18CXX)
#define GPIOX_PORTNUM(x)      x(GPIOX_PORTNUM_)
#define GPIOX_PORTNAME(x)     x(GPIOX_PORTNAME_)
#define GPIOX_PIN(x)          x(GPIOX_PIN_)
#else
#define GPIOX_PORTNUM(x)      GPIOX_PORTNUM_(x)
#define GPIOX_PORTNAME(x)     GPIOX_PORTNAME_(x)
#define GPIOX_PIN(x)          GPIOX_PIN_(x)
#endif

#if defined(__AVR__)
#define GLUE(a, b)            a ## b
#define DDRBITSET(p, m)       GLUE(DDR, p) |= (1 << m)
#define DDRBITCLR(p, m)       GLUE(DDR, p) &= ~(1 << m)
#define PORTBITSET(p, m)      GLUE(PORT, p) |= (1 << m)
#define PORTBITCLR(p, m)      GLUE(PORT, p) &= ~(1 << m)
#define PINBIT(p, m)          GLUE(PIN, p) & (1 << m)
#define PORTX(p, m)           GLUE(PORT, p)
#define DDRX(p, m)            GLUE(DDR, p)

#define GPIOX_MODEPININ(x)    DDRBITCLR(x)
#define GPIOX_MODEPINOUT(x)   DDRBITSET(x)
#define GPIOX_SETBIT(x)       PORTBITSET(x)
#define GPIOX_CLRBIT(x)       PORTBITCLR(x)
#define GPIOX_GETBIT(x)       (PINBIT(x))
#define GPIOX_MODEPORT(x)     DDRX(x)
#define GPIOX_WRITEPORT(x)    PORTX(x)

#else // #if defined(__AVR__)

#define TRISBIT(p, m)         TRIS ## p ## bits.TRIS ## p ## m
#define LATBIT(p, m)          LAT ## p ## bits.LAT ## p ## m
#define PORTBIT(p, m)         PORT ## p ## bits.R ## p ## m
#define TRISX(p, m)           TRIS ## p
#define LATX(p, m)            LAT ## p
#define PORTX(p, m)           PORT ## p

// PIC18Fxxx (C18)
#if defined(__18CXX)
#define GPIOX_MODEPININ(x)    x(TRISBIT) = 1
#define GPIOX_MODEPINOUT(x)   x(TRISBIT) = 0
#define GPIOX_SETBIT(x)       x(LATBIT) = 1
#define GPIOX_CLRBIT(x)       x(LATBIT) = 0
#define GPIOX_GETBIT(x)       x(PORTBIT)
#define GPIOX_MODEPORT(x)     x(TRISX)
#define GPIOX_WRITEPORT(x)    x(LATX)
#ifdef  LCDNOP
#undef  NOP
#define NOP() LCDNOP
#endif
#endif  // #if defined(__18CXX)

// PIC16Fxx (HTC, XC)
#if (defined(_PIC14) || defined(_PIC14E))
#define GPIOX_MODEPININ(x)    TRISBIT(x) = 1
#define GPIOX_MODEPINOUT(x)   TRISBIT(x) = 0
#define GPIOX_SETBIT(x)       PORTBIT(x) = 1
#define GPIOX_CLRBIT(x)       PORTBIT(x) = 0
#define GPIOX_GETBIT(x)       PORTBIT(x)
#define GPIOX_MODEPORT(x)     TRISX(x)
#define GPIOX_WRITEPORT(x)    PORTX(x)
#endif  // #if (defined(_PIC14) || defined(_PIC14E))

// PIC16F1xxx, PIC18F, PIC24x, DSPIC (XC8, XC16, C30, HTC18)
#if (defined(__PICC18__) || defined(__C30__))
#define GPIOX_MODEPININ(x)    TRISBIT(x) = 1
#define GPIOX_MODEPINOUT(x)   TRISBIT(x) = 0
#define GPIOX_SETBIT(x)       LATBIT(x) = 1
#define GPIOX_CLRBIT(x)       LATBIT(x) = 0
#define GPIOX_GETBIT(x)       PORTBIT(x)
#define GPIOX_MODEPORT(x)     TRISX(x)
#define GPIOX_WRITEPORT(x)    LATX(x)
#endif  // #if (defined(__PICC18__) || defined(__C30__))

#endif // #else defined(__AVR__)

//------------------------------------------------------------------------------
// karakterenk�nti friss�t�si frekvencia (csak interrupt m�dban)
// (ha 80 karakteresn�l nagyobb, akkor a fele, mert egyszerre 2 karaktert friss�t)
#if (1UL * LCDLINES * LCDWIDTH > 160)
#error "Maximum 2x80 karakteres lehet a kijelz�"
#endif

#if (1UL * LCDLINES * LCDWIDTH > 80)

#ifndef  LCDE2
#error  "80 karakteresn�l nagyobb kijelz� eset�n E2 l�bat is meg kell adni!"
#endif

#if     LCDLINES != 4
#error  "80 karakteresn�l nagyobb kijelz�, csak 4 sorosban l�tezik!"
#endif

// a 80 karakteresn�l nagyobbat, 2db 2 sorosnak tekintj�k
#undef  LCDLINES
#define LCDLINES        2
#endif  // #if (1UL * LCDLINES * LCDWIDTH > 80)

#define LCDCHARPERMODUL (1UL * LCDLINES * LCDWIDTH)
#define LCDCHARPERSEC   (1UL * LCDFRAMEPERSEC * (LCDLINES * LCDWIDTH + LCDLINES))

// LCD mem�riac�mek (1..4 sorok kezdete)
#define SETDDRAMADDR1  0x80
#define SETDDRAMADDR2  0xC0
#define SETDDRAMADDR3  (0x80 + LCDWIDTH)
#define SETDDRAMADDR4  (0xC0 + LCDWIDTH)

#define SETCGRAMADDR   0x40

// megszak�t�s �zemm�dban a timer be�ll�t�sok
#if ((!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ)))

#include "charlcd-timers.h"

#endif // (!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))

//==============================================================================
// �ltal�nos v�ltoz�k

// az LcdWrite f�ggv�ny haszn�lja �tmeneti t�rol�sra (csak a felesleges veremm�veletek elker�l�se miatt)
volatile union
{
  struct
  {
    unsigned bit0: 1;
    unsigned bit1: 1;
    unsigned bit2: 1;
    unsigned bit3: 1;
    unsigned bit4: 1;
    unsigned bit5: 1;
    unsigned bit6: 1;
    unsigned bit7: 1;
  };
  struct
  {
    unsigned nibble03: 4;
    unsigned nibble47: 4;
  };
  unsigned char chr;
}ch;

// ebben a t�mbben t�roljuk a megjelen�tend� karaktereket
#ifdef LCDE2
volatile char LcdText[2UL * LCDWIDTH * LCDLINES + 1];
#else
volatile char LcdText[1UL * LCDWIDTH * LCDLINES + 1];
#endif
volatile unsigned char LcdPos;                   // Friss�t�sn�l az aktu�lis karakter poz�ci�ja

// LcdStatus lehets�ges �rt�kei:
// - HOME: DDRAM = 0 �ll�t�si f�zis
// - LCHAR: karakterek ki�r�sa f�zis
// - DDR: DDRAM �ll�t�s f�zis
// - CURTYPE: kurzor t�pus �ll�t�sa f�zis
// - CURPOS: kurzorpoz�ci� be�ll�t�si f�zis
// - CGR: CGRAM �ll�t�si f�zis RAM-b�l
// - CHARGEN: felhaszn�l�i karakterk�szlet felt�lt�se RAM-b�l f�zis
// - CGRROM: CGRAM �ll�t�si f�zis ROM-b�l
// - CHARGENROM: felhaszn�l�i karakterk�szlet felt�lt�se ROM-b�l f�zis
// - REFREND: friss�t�s befejezve
enum LS {HOME, LCHAR, DDR, CURTYPE, CURPOS, CGR, CHARGEN, CGRROM, CHARGENROM, REFREND} LcdStatus = LCHAR;

// A villog� karaktereket ebben a t�mbben 1 �rt�k� bit jelzi
#ifdef LCDBLINKCHAR
#ifdef LCDE2
volatile char LcdBlink[(2UL * LCDWIDTH * LCDLINES + 7) / 8];
#else  // LCDE2
volatile char LcdBlink[(1UL * LCDWIDTH * LCDLINES + 7) / 8];
#endif // LCDE2

// Villog�si f�zis t�rol�ja
volatile unsigned char BlinkPhase = 0;  // 0 = villog� karakterek l�tszanak, 1 = nem l�tszik
#endif // LCDBLINKCHAR

// Kurzor poz�ci� �s kurzor t�pus
#ifdef LCDCURSOR
volatile unsigned char LcdCursorPos = 0;
volatile unsigned char LcdCursorType;
#endif

// saj�t megszak�t�skezel�vel �s id�z�t�skezel�vel haszn�lva a friss�t�si �llapot ebben a v�ltoz�ban lesz
#if (defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
unsigned char LcdIrqStatus = 0;
#endif

// Felhaszn�l�i karakterk�szlet RAM-b�l
#ifdef USERCHARSETCHANGE
char*  uchp;
#endif

// Felhaszn�l�i karakterk�szlet ROM-b�l
#ifdef USERCHARSETCHANGEROM
rom char*  uchpr;
#endif

//==============================================================================
// C18 fordito a nem forditando ter�leten is hib�t jelez, ha nincs defini�lva LCDDT0..3
#if defined(LCD4BITMODE) && defined(__18CXX)
#undef LCDDT0
#undef LCDDT1
#undef LCDDT2
#undef LCDDT3
#define LCDDT0 LCDDT4
#define LCDDT1 LCDDT5
#define LCDDT2 LCDDT6
#define LCDDT3 LCDDT7
#endif

#if ((GPIOX_PORTNUM(LCDDT4) == GPIOX_PORTNUM(LCDDT5))\
  && (GPIOX_PORTNUM(LCDDT5) == GPIOX_PORTNUM(LCDDT6))\
  && (GPIOX_PORTNUM(LCDDT6) == GPIOX_PORTNUM(LCDDT7)))
#if ((GPIOX_PIN(LCDDT4) + 1 == GPIOX_PIN(LCDDT5))\
  && (GPIOX_PIN(LCDDT5) + 1 == GPIOX_PIN(LCDDT6))\
  && (GPIOX_PIN(LCDDT6) + 1 == GPIOX_PIN(LCDDT7)))
// LCD adatl�bai folyamotosan vannak be�llitva (pl. B2,B3,B4,B5)
#ifdef LCD8BITMODE
#if ((GPIOX_PORTNUM(LCDDT0) == GPIOX_PORTNUM(LCDDT1))\
  && (GPIOX_PORTNUM(LCDDT1) == GPIOX_PORTNUM(LCDDT2))\
  && (GPIOX_PORTNUM(LCDDT2) == GPIOX_PORTNUM(LCDDT3))\
  && (GPIOX_PORTNUM(LCDDT3) == GPIOX_PORTNUM(LCDDT4)))
#if ((GPIOX_PIN(LCDDT0) + 1 == GPIOX_PIN(LCDDT1))\
  && (GPIOX_PIN(LCDDT1) + 1 == GPIOX_PIN(LCDDT2))\
  && (GPIOX_PIN(LCDDT2) + 1 == GPIOX_PIN(LCDDT3))\
  && (GPIOX_PIN(LCDDT3) + 1 == GPIOX_PIN(LCDDT4)))
// LCD adatl�bai folyamotosan vannak be�llitva (pl. B0,B1,B2,B3,B4,B5,B6,B7)
#define LCDDTAUTOOPT
#endif // D0..D4 portl�b folytonoss�g ?
#endif // D0..D4 port azonoss�g ?
#else  // #ifdef LCD8BITMODE
// LCD adatl�bai folyamotosan vannak be�llitva (pl. B2,B3,B4,B5)
#define LCDDTAUTOOPT
#endif // #else LCD8BITMODE
#endif // D4..D7 portl�b folytonoss�g ?
#endif // D4..D7 port azonoss�g ?

//------------------------------------------------------------------ 4 bites mod

#ifdef LCD4BITMODE

// LCD adatl�bak LCD -> CPU (Lcd olvas�sa)
#ifndef LCDDTIN
#ifdef  LCDDTAUTOOPT
#if defined(__AVR__)
#define LCDDTIN GPIOX_MODEPORT(LCDDT4) &= ~(0x0F << GPIOX_PIN(LCDDT4))
#else
#define LCDDTIN GPIOX_MODEPORT(LCDDT4) |= (0x0F << GPIOX_PIN(LCDDT4))
#endif
#else
#define LCDDTIN GPIOX_MODEPININ(LCDDT4); GPIOX_MODEPININ(LCDDT5); GPIOX_MODEPININ(LCDDT6); GPIOX_MODEPININ(LCDDT7);
#endif
#endif

// LCD adatl�bak CPU -> LCD (Lcd ir�sa)
#ifndef LCDDTOUT
#ifdef  LCDDTAUTOOPT
#if defined(__AVR__)
#define LCDDTOUT GPIOX_MODEPORT(LCDDT4) |= (0x0F << GPIOX_PIN(LCDDT4))
#else
#define LCDDTOUT GPIOX_MODEPORT(LCDDT4) &= ~(0x0F << GPIOX_PIN(LCDDT4))
#endif
#else
#define LCDDTOUT GPIOX_MODEPINOUT(LCDDT4); GPIOX_MODEPINOUT(LCDDT5); GPIOX_MODEPINOUT(LCDDT6); GPIOX_MODEPINOUT(LCDDT7);
#endif
#endif

// LCD adatl�bakra 4..7 bit kirak�sa (4 bites m�dban)
#ifndef LCDDT4TO7
#ifdef  LCDDTAUTOOPT
#define LCDDT4TO7 GPIOX_WRITEPORT(LCDDT4) = (GPIOX_WRITEPORT(LCDDT4) & ~(0b00001111 << GPIOX_PIN(LCDDT4)))\
        | (((ch.chr >> 4) & 0b00001111) << GPIOX_PIN(LCDDT4))
#else  
#define LCDDT4TO7 \
  if(!ch.bit4) GPIOX_CLRBIT(LCDDT4); if(ch.bit4) GPIOX_SETBIT(LCDDT4);\
  if(!ch.bit5) GPIOX_CLRBIT(LCDDT5); if(ch.bit5) GPIOX_SETBIT(LCDDT5);\
  if(!ch.bit6) GPIOX_CLRBIT(LCDDT6); if(ch.bit6) GPIOX_SETBIT(LCDDT6);\
  if(!ch.bit7) GPIOX_CLRBIT(LCDDT7); if(ch.bit7) GPIOX_SETBIT(LCDDT7);
#endif  // else LCDDTAUTOOPT
#endif

// LCD adatl�bakra 0..3 bit kirak�sa (4 bites m�dban)
#ifndef LCDDT0TO3
#ifdef  LCDDTAUTOOPT
#define LCDDT0TO3 GPIOX_WRITEPORT(LCDDT4) = (GPIOX_WRITEPORT(LCDDT4) & ~(0b00001111 << GPIOX_PIN(LCDDT4)))\
        | ((ch.chr & 0b00001111) << GPIOX_PIN(LCDDT4))
#else  // LCDDTAUTOOPT 
#define LCDDT0TO3 \
  if(!ch.bit0) GPIOX_CLRBIT(LCDDT4); if(ch.bit0) GPIOX_SETBIT(LCDDT4);\
  if(!ch.bit1) GPIOX_CLRBIT(LCDDT5); if(ch.bit1) GPIOX_SETBIT(LCDDT5);\
  if(!ch.bit2) GPIOX_CLRBIT(LCDDT6); if(ch.bit2) GPIOX_SETBIT(LCDDT6);\
  if(!ch.bit3) GPIOX_CLRBIT(LCDDT7); if(ch.bit3) GPIOX_SETBIT(LCDDT7);
#endif // else LCDDTAUTOOPT
#endif // ifndef LCDDT0TO3
#endif // #ifdef LCD4BITMODE

//------------------------------------------------------------------ 8 bites mod
#ifdef  LCD8BITMODE
// LCD adatl�bak LCD -> CPU (Lcd olvas�sa)
#ifndef LCDDTIN
#ifdef  LCDDTAUTOOPT
#if defined(__AVR__)
#define LCDDTIN GPIOX_MODEPORT(LCDDT0) = 0b00000000
#else
#ifdef  Cpu8bit
#define LCDDTIN GPIOX_MODEPORT(LCDDT0) = 0b11111111
#else
#define LCDDTIN GPIOX_MODEPORT(LCDDT0) |= (0b11111111 << GPIOX_PIN(LCDDT0))
#endif
#endif
#else
#define LCDDTIN {GPIOX_MODEPININ(LCDDT0); GPIOX_MODEPININ(LCDDT1); GPIOX_MODEPININ(LCDDT2); GPIOX_MODEPININ(LCDDT3);\
                 GPIOX_MODEPININ(LCDDT4); GPIOX_MODEPININ(LCDDT5); GPIOX_MODEPININ(LCDDT6); GPIOX_MODEPININ(LCDDT7);}
#endif
#endif

// LCD adatl�bak CPU -> LCD (Lcd ir�sa)
#ifndef LCDDTOUT
#ifdef  LCDDTAUTOOPT
#if defined(__AVR__)
#define LCDDTOUT GPIOX_MODEPORT(LCDDT0) = 0b11111111
#else
#ifdef  Cpu8bit
#define LCDDTOUT GPIOX_MODEPORT(LCDDT0) = 0b00000000
#else
#define LCDDTOUT GPIOX_MODEPORT(LCDDT0) &= ~(0b11111111 << GPIOX_PIN(LCDDT0))
#endif
#endif
#else
#define LCDDTOUT {GPIOX_MODEPINOUT(LCDDT0); GPIOX_MODEPINOUT(LCDDT1); GPIOX_MODEPINOUT(LCDDT2); GPIOX_MODEPINOUT(LCDDT3);\
                  GPIOX_MODEPINOUT(LCDDT4); GPIOX_MODEPINOUT(LCDDT5); GPIOX_MODEPINOUT(LCDDT6); GPIOX_MODEPINOUT(LCDDT7);}
#endif
#endif

  // LCD adatl�bakra 0..3 bit kirak�sa (8 bites m�dban)
#ifndef LCDDT0TO7
#ifdef  LCDDTAUTOOPT
#ifdef  Cpu8bit
#define LCDDT0TO7 GPIOX_WRITEPORT(LCDDT0) = ch.chr
#else
#define LCDDT0TO7 GPIOX_WRITEPORT(LCDDT0) = (GPIOX_WRITEPORT(LCDDT0) & ~(0b11111111 << GPIOX_PIN(LCDDT0)))\
        | (ch.chr << GPIOX_PIN(LCDDT0))
#endif
#else
#define LCDDT0TO7 \
  if(!ch.bit0) GPIOX_CLRBIT(LCDDT0); if(ch.bit0) GPIOX_SETBIT(LCDDT0);\
  if(!ch.bit1) GPIOX_CLRBIT(LCDDT1); if(ch.bit1) GPIOX_SETBIT(LCDDT1);\
  if(!ch.bit2) GPIOX_CLRBIT(LCDDT2); if(ch.bit2) GPIOX_SETBIT(LCDDT2);\
  if(!ch.bit3) GPIOX_CLRBIT(LCDDT3); if(ch.bit3) GPIOX_SETBIT(LCDDT3);\
  if(!ch.bit4) GPIOX_CLRBIT(LCDDT4); if(ch.bit4) GPIOX_SETBIT(LCDDT4);\
  if(!ch.bit5) GPIOX_CLRBIT(LCDDT5); if(ch.bit5) GPIOX_SETBIT(LCDDT5);\
  if(!ch.bit6) GPIOX_CLRBIT(LCDDT6); if(ch.bit6) GPIOX_SETBIT(LCDDT6);\
  if(!ch.bit7) GPIOX_CLRBIT(LCDDT7); if(ch.bit7) GPIOX_SETBIT(LCDDT7);
#endif
#endif
#endif // else LCD4BITMODE

//==============================================================================
#ifdef SOFTSIMFAST
#define EDELAY   ;
#define EDELAYCYCLE 0
#else  // SOFTSIMFAST


// E l�b tart�si id�
// AVR
#if defined(__AVR__)
#if   CpuMips >= 30000000
#define EDELAY  _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP()
#define EDELAYCYCLE 12
#elif CpuMips >= 20000000
#define EDELAY  _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP()
#define EDELAYCYCLE 8
#elif CpuMips >= 10000000
#define EDELAY  _NOP(); _NOP(); _NOP(); _NOP(); _NOP()
#define EDELAYCYCLE 4
#elif CpuMips >= 7500000
#define EDELAY  _NOP(); _NOP(); _NOP()
#define EDELAYCYCLE 3
#elif CpuMips >= 5000000
#define EDELAY  _NOP(); _NOP()
#define EDELAYCYCLE 2
#else
#define EDELAY  _NOP()
#define EDELAYCYCLE 1
#endif // CpuMips
//------------------------------------------------------------------------------
// PIC16, PIC18
#elif (defined(_PIC14) || defined(_PIC14E) || defined(__18CXX) || defined(__PICC18__))
#if   CpuMips >= 10000000
#define EDELAY  NOP(); NOP(); NOP(); NOP()
#define EDELAYCYCLE 4
#elif CpuMips >= 7500000
#define EDELAY  NOP(); NOP(); NOP()
#define EDELAYCYCLE 3
#elif CpuMips >= 5000000
#define EDELAY  NOP(); NOP()
#define EDELAYCYCLE 2
#else
#define EDELAY  NOP()
#define EDELAYCYCLE 1
#endif // CpuMips
//------------------------------------------------------------------------------
// PIC24
#elif defined(__C30__)
#if   CpuMips >= 40000000
#define EDELAY  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop()
#define EDELAYCYCLE 16
#elif CpuMips >= 30000000
#define EDELAY  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop()
#define EDELAYCYCLE 12
#elif CpuMips >= 20000000
#define EDELAY  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop()
#define EDELAYCYCLE 8
#elif CpuMips >= 10000000
#define EDELAY  Nop(); Nop(); Nop(); Nop()
#define EDELAYCYCLE 4
#elif CpuMips >= 7500000
#define EDELAY  Nop(); Nop(); Nop()
#define EDELAYCYCLE 3
#elif CpuMips >= 5000000
#define EDELAYCYCLE 2
#define EDELAY  Nop(); Nop()
#else
#define EDELAY  Nop()
#define EDELAYCYCLE 1
#endif // CpuMips
#endif // else defined(__AVR__)

#endif // else SOFTSIMFAST

//==============================================================================
// Processzorf�ggetlen r�szek
//==============================================================================

//==============================================================================
// DelayMs
// - 1..255msec v�rakoz�s (nem teljesen pontos de az LCD inicializ�l�shoz elegend�)
//==============================================================================
void DelayMs(unsigned char ms)
{
  volatile unsigned int dl;

  #ifndef SOFTSIMFAST
  while(ms--)
  {
    dl = DELAYNUM;
    while(dl--);
  }
  #endif
}

#ifdef LCDMODEONCEDELAY
//==============================================================================
// DelayLcd
// - Lcd �r�s v�rakoz�s v�rakoz�s (nem teljesen pontos)
//==============================================================================
// LCD �r�sok k�z�tt ennyi utas�t�sciklusnak kell legal�bb lenni:
#define LCDEXECUTIONCYCLE ((CpuMips * LCDEXECUTIONTIME) / 1000000)

// ennyi a val�s v�grehajt�si ideje az LCD �r� f�ggv�nynek
#ifdef LCDE2
#ifdef LCD4BITMODE
#define LCDPROCCYCLE (LCDPROCCYCLENULLSTEREO + 8 * EDELAYCYCLE + 4)
#else  // LCD4BITMODE
#define LCDPROCCYCLE (LCDPROCCYCLENULLSTEREO + 4 * EDELAYCYCLE)
#endif // else LCD4BITMODE
#else  // LCDE2
#ifdef LCD4BITMODE
#define LCDPROCCYCLE (LCDPROCCYCLENULL + 4 * EDELAYCYCLE + 2)
#else  // LCD4BITMODE
#define LCDPROCCYCLE (LCDPROCCYCLENULL + 2 * EDELAYCYCLE)
#endif // else LCD4BITMODE
#endif // else LCDE2

// ha LcdProc v�grehajt�si ideje >= LCDEXECUTIONTIME -> nincs sz�ks�g k�sleltet� ciklusra
#if LCDPROCCYCLE >= LCDEXECUTIONCYCLE
#define DelayLcd() ;
#else  // LCDPROCCYCLE >= LCDEXECUTIONCYCLE

// ennyit kell m�g v�rakozni az LCDEXECUTIONTIME-hoz
#define LCDDELAYCYCLE (LCDEXECUTIONCYCLE - LCDPROCCYCLE)

void DelayLcd(void)
{
  #if defined(__AVR__)
  static volatile unsigned char cl;
  #if (LCDDELAYCYCLE / 10) > 255
  #error "LCD EXECUTIONTIME t�l nagy!"
  #else
  cl = LCDDELAYCYCLE / 10;
  #endif
  #elif defined(__C30__)
  static volatile unsigned int cl;
  #if (LCDDELAYCYCLE / 4) > 65535
  #error "LCD EXECUTIONTIME t�l nagy!"
  #else
  cl = LCDDELAYCYCLE / 4;
  #endif
  #else // defined(__C30__)
  volatile unsigned char cl;
  #if (LCDDELAYCYCLE / 3) > 255
  #error "LCD EXECUTIONTIME t�l nagy!"
  #else
  cl = LCDDELAYCYCLE / 3;
  #endif
  #endif // else defined(__C30__)
  while(--cl);
}

#endif // else LCDPROCCYCLE >= LCDEXECUTIONCYCLE

#endif // LCDMODEONCEDELAY

//==============================================================================
// LcdBusy (csak LCDMODEONCEBUSY m�dban)
// - LCD vez�rl� foglalts�g�nak ellen�rz�se
//   (nem v�rja meg m�g szabad lesz)
// visszat�r� �rt�k:
//   1 ha a Busy flag is be van �ll�tva (foglalt)
//   0 ha szabad (lehet adatot k�ldeni)
//==============================================================================
#if (defined LCDMODEONCEBUSY) || (defined LCDMODECONTBUSY)
char LcdBusy(void)
{
  LCDDTIN;                              // adatl�bak bemenetek
  GPIOX_SETBIT(LCDRW);                  // adatir�ny: LCD -> mikrovez�rl� (read)
  GPIOX_CLRBIT(LCDRS);                  // RS = 0
  EDELAY;
  GPIOX_SETBIT(LCDE);
  EDELAY;

  if(GPIOX_GETBIT(LCDDT7))              // busy flag
  {
    GPIOX_CLRBIT(LCDE);
    #ifdef LCD4BITMODE
    EDELAY;
    GPIOX_SETBIT(LCDE); EDELAY; GPIOX_CLRBIT(LCDE);
    EDELAY;
    #endif // LCD4BITMODE
    return 1;                           // foglalt
  }
  GPIOX_CLRBIT(LCDE);
  #ifdef LCD4BITMODE
  EDELAY;
  GPIOX_SETBIT(LCDE); EDELAY; GPIOX_CLRBIT(LCDE);
  #endif // LCD4BITMODE

  #ifdef LCDE2
  EDELAY;
  GPIOX_SETBIT(LCDE2);
  EDELAY;
  if(GPIOX_GETBIT(LCDDT7))              // busy flag
  {
    GPIOX_CLRBIT(LCDE2);
    #ifdef LCD4BITMODE
    EDELAY;
    GPIOX_SETBIT(LCDE2); EDELAY; GPIOX_CLRBIT(LCDE2);
    #endif // LCD4BITMODE
    return 1;                           // foglalt
  }
  LCDE2PIN0;
  #ifdef LCD4BITMODE
  EDELAY;
  GPIOX_SETBIT(LCDE2); EDELAY; GPIOX_CLRBIT(LCDE2);
  EDELAY;
  #endif // LCD4BITMODE
  #endif // LCDE2

  return 0;                             // szabad
}
#endif // (defined LCDMODEONCEBUSY) || (defined LCDMODECONTBUSY)

//==============================================================================
// LcdWrite (I/O l�bakon kereszt�l egy karaktert �r az LCD-re)
// - RS l�b = 0: config regiszter (parancs)
// - RS l�b = 1: RAM (karakter)
// - ch glob�lis b�ltoz� = adat
//==============================================================================
void LcdWrite(void)
{
  // ha nem haszn�lunk BUSY m�dot, mindig kimenet marad az �sszes adatl�b, ez�rt nem kell �ll�tgatni
  #if (defined LCDMODEONCEBUSY) || (defined LCDMODECONTBUSY)
  GPIOX_CLRBIT(LCDRW);                  // adatir�ny: mikrovez�rl� -> LCD
  LCDDTOUT;
  #endif // !((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))

  #ifdef LCD4BITMODE
  LCDDT4TO7;
  GPIOX_SETBIT(LCDE); EDELAY; GPIOX_CLRBIT(LCDE);
  LCDDT0TO3;
  #else
  LCDDT0TO7;
  #endif
  GPIOX_SETBIT(LCDE); EDELAY; GPIOX_CLRBIT(LCDE);
}

//==============================================================================
#ifdef LCDE2
void LcdWrite2(void)
{
  #ifdef LCD4BITMODE
  LCDDT4TO7;
  GPIOX_SETBIT(LCDE2); EDELAY; GPIOX_CLRBIT(LCDE2);
  LCDDT0TO3;
  #else
  LCDDT0TO7;
  #endif
  GPIOX_SETBIT(LCDE2); EDELAY; GPIOX_CLRBIT(LCDE2);
}
#else
#define LcdWrite2() ;
#endif // #else LCDE2

//------------------------------------------------------------------------------
#ifdef USERCHARSET

#if defined(__AVR__)
#define userromcharset(n) pgm_read_dword_near(&userromcharset[n])
const USERCHARSETARRAY userromcharset PROGMEM =
#elif defined(__18CXX)
rom USERCHARSETARRAY userromcharset = 
#else
const USERCHARSETARRAY userromcharset = 
#endif
{
  USER0CHAR0, USER0CHAR1, USER0CHAR2, USER0CHAR3, USER0CHAR4, USER0CHAR5, USER0CHAR6, USER0CHAR7,
  USER1CHAR0, USER1CHAR1, USER1CHAR2, USER1CHAR3, USER1CHAR4, USER1CHAR5, USER1CHAR6, USER1CHAR7,
  USER2CHAR0, USER2CHAR1, USER2CHAR2, USER2CHAR3, USER2CHAR4, USER2CHAR5, USER2CHAR6, USER2CHAR7,
  USER3CHAR0, USER3CHAR1, USER3CHAR2, USER3CHAR3, USER3CHAR4, USER3CHAR5, USER3CHAR6, USER3CHAR7,
  USER4CHAR0, USER4CHAR1, USER4CHAR2, USER4CHAR3, USER4CHAR4, USER4CHAR5, USER4CHAR6, USER4CHAR7,
  USER5CHAR0, USER5CHAR1, USER5CHAR2, USER5CHAR3, USER5CHAR4, USER5CHAR5, USER5CHAR6, USER5CHAR7,
  USER6CHAR0, USER6CHAR1, USER6CHAR2, USER6CHAR3, USER6CHAR4, USER6CHAR5, USER6CHAR6, USER6CHAR7,
  USER7CHAR0, USER7CHAR1, USER7CHAR2, USER7CHAR3, USER7CHAR4, USER7CHAR5, USER7CHAR6, USER7CHAR7
};
#endif // USERCHARSET

#ifdef LCDE2
#define  EIMPULSE  {GPIOX_SETBIT(LCDE); GPIOX_SETBIT(LCDE2); EDELAY; GPIOX_CLRBIT(LCDE); GPIOX_CLRBIT(LCDE2); }
#else
#define  EIMPULSE  {GPIOX_SETBIT(LCDE); EDELAY; GPIOX_CLRBIT(LCDE);}
#endif

//==============================================================================
// LcdInit
// - I/O l�bak, TRIS regiszterek be�ll�t�sa 
// - LCD kijelz� bekapcsol�sa �s alap�llapotba �ll�t�sa
//==============================================================================
void LcdInit(void)
{
  unsigned char i;
  // I/O l�bak be�ll�t�sa
  GPIOX_CLRBIT(LCDE);         // E = 0
  GPIOX_MODEPINOUT(LCDE);     // E = kimenet

  #ifdef LCDE2
  GPIOX_CLRBIT(LCDE2);        // E2 = 0
  GPIOX_MODEPINOUT(LCDE2);    // E2 = kimenet
  #endif

  GPIOX_CLRBIT(LCDRS);        // RS = 0
  GPIOX_MODEPINOUT(LCDRS);    // RS = kimenet

  // ha R/W l�bat is haszn�ljuk
  #ifdef LCDRW
  GPIOX_CLRBIT(LCDRW);        // RW = 0
  GPIOX_MODEPINOUT(LCDRW);    // RW = kimenet
  #endif // LCDRW

  #ifdef LCD4BITMODE
  ch.chr = 0b00000000;
  LCDDT4TO7;
  LCDDTOUT;
  DelayMs(100);
  ch.chr = 0b00110000;
  LCDDT4TO7;
  EIMPULSE;                             // 0011
  DelayMs(5);
  EIMPULSE;                             // 0011
  DelayMs(5);
  EIMPULSE;                             // 0011
  DelayMs(5);
  ch.chr = 0b00100000;
  LCDDT4TO7;
  EIMPULSE;                             // 0010
  DelayMs(5);
  #if  LCDLINES > 1
  ch.chr = 0b00101000;                  // 2 soros eset�n 00101000
  #endif
  LcdWrite();
  LcdWrite2();

  #else  // #ifdef LCD4BITMODE
  ch.chr = 0b00000000;
  LCDDT0TO7;
  LCDDTOUT;
  DelayMs(100);
  ch.chr = 0b00110000;
  LCDDT0TO7;
  EIMPULSE;
  DelayMs(5);
  EIMPULSE;
  DelayMs(5);
  EIMPULSE;
  DelayMs(5);
  #if  LCDLINES > 1
  ch.chr = 0b00111000;                  // 2 soros eset�n 00111000
  #endif
  LcdWrite();
  LcdWrite2();
  #endif // #else LCD4BITMODE

  DelayMs(2);
  ch.chr = 0b00001100;                  // kijelz� bekapcs, kurzor ki, kurzor villog�s ki
  LcdWrite();
  LcdWrite2();
  
  DelayMs(2);
  ch.chr = 0b00000001;                  // display off
  LcdWrite();
  LcdWrite2();

  DelayMs(2);
  ch.chr = 0b00000110;                  // 
  LcdWrite();
  LcdWrite2();
  DelayMs(2);

  #ifdef USERCHARSET
  DelayMs(2);
  ch.chr = SETCGRAMADDR;                // CGRAM = 0
  LcdWrite();
  LcdWrite2();

  GPIOX_SETBIT(LCDRS);
  for(i = 0; i < 64; i++)
  {
    DelayMs(2);
    #if defined(__AVR__)
    ch.chr = userromcharset(i);
    #else
    ch.chr = userromcharset[i];
    #endif
    LcdWrite();
    LcdWrite2();
  }

  DelayMs(2);
  GPIOX_CLRBIT(LCDRS);
  ch.chr = SETDDRAMADDR1;               // DDRAM = els� sor eleje
  LcdWrite();
  LcdWrite2();
  #endif // USERCHARSET
  GPIOX_SETBIT(LCDRS);

  // sz�veg t�rl�se
  #ifdef LCDE2
  for(i = 0; i < (2UL * LCDWIDTH * LCDLINES); i++) LcdText[i] = ' ';
  #else
  for(i = 0; i < (1UL * LCDWIDTH * LCDLINES); i++) LcdText[i] = ' ';
  #endif

  // villog�s t�mb t�rl�se
  #ifdef LCDBLINKCHAR
  #ifdef LCDE2
  for(i = 0; i < ((2UL * LCDWIDTH * LCDLINES + 7) / 8); i++) LcdBlink[i] = 0;
  #else  // LCDE2
  for(i = 0; i < ((1UL * LCDWIDTH * LCDLINES + 7) / 8); i++) LcdBlink[i] = 0;
  #endif // LCDE2
  #endif // LCDBLINKCHAR

  LcdPos = 0;

  #if (!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
  TIMERINIT;
  #ifdef LCDMODECONTIRQ
  LcdRefreshStart();
  #endif // LCDMODECONTIRQ
  #endif // (!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
}


//==============================================================================
// Blinker (villogtat�, csak megszak�t�sos folyamatos friss�t�si �zemm�dban)
#if (defined LCDMODECONTIRQ) && (defined LCDBLINKCHAR) && (LCDBLINKSPEED > 0)
#define BLINKER() {             \
  if(!BlinkTimer--)             \
  {                             \
    BlinkPhase = !BlinkPhase;   \
    BlinkTimer = LCDBLINKSPEED; \
  }                             \
}
#define AUTOBLINKER
#else  // (defined LCDMODECONTIRQ) && (defined LCDBLINKCHAR) && (LCDBLINKSPEED > 0)
#define BLINKER() ;
#endif // else (defined LCDMODECONTIRQ) && (defined LCDBLINKCHAR) && (LCDBLINKSPEED > 0)

//==============================================================================
// LcdProcess (egy karaktert friss�t a kijelz�n)
// El�felt�tel: LcdInit() inicializ�lni kell
// Input:           LcdText[], villog� m�dban LcdBlink[]
// �ttekint�s:      Ha szabad az LCD, egy karaktert �tm�sol az LcdText[] t�mbb�l 
//                  az LCD kijelz�re 
// Megjegyz�s:      Nincs v�rokoz�si id�, ez�rt USB-s alkalmaz�sn�l ezzel friss�thet� 
//                  a kijelz�.
//==============================================================================
#ifdef TIMERINTPROCESS
TIMERINTPROCESS
#else  // TIMERINTPROCESS
void LcdProcess(void)
#endif // else TIMERINTPROCESS
{
  // Automatikus blinker frame sz�ml�l�ja a villog�s �temez�s�hez
  #ifdef AUTOBLINKER
  static char BlinkTimer = LCDBLINKSPEED;
  #endif

//------------------------------------------------------------------------------
  // megszak�t�s m�d specialit�sai
  #if (!defined LCDUSERTIMER) && ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))
  // kompar�tor n�lk�li id�z�t� �t�ll�t�sa
  TIMERCOMPLESS;
  // megszak�t�s nyugt�z�sa
  TIMERIRQACK;
  #endif

//------------------------------------------------------------------------------
  // LCD foglalts�gfigyel�ses �zemm�d
  #if (((defined LCDMODEONCEBUSY) || (defined LCDMODECONTBUSY)) && (!defined SOFTSIMBUSY))
  if(LcdBusy()) return;                 // LCD foglalt ?
  #endif

  //----------------------------------------------------------------------------
  if(LcdStatus == LCHAR)
  {
    GPIOX_SETBIT(LCDRS);                // karakter megy majd a kijelz�re
    // ----------------------------------- egy karakter fiss�t�s
    ch.chr = LcdText[LcdPos];
    
    #ifdef LCDZEROCHANGE
    if (ch.chr == 0)
    {
      ch.chr = ' ';                     // #0 k�d� karakter -> sz�k�z karakter
      #ifdef LCDZEROCHANGETEXT
      LcdText[LcdPos] = ' ';            // LcdText-ben is #0 -> 'SPACE' csere
      #endif
    }
    #endif // LCDZEROCHANGE

    #ifdef LCDBLINKCHAR
    if((BlinkPhase) && (LcdBlink[LcdPos >> 3] & (1 << (LcdPos & 7))))
      ch.chr = ' ';                     // ha villog�si f�zis == 1, �s az adott karakter is villogtatva van -> sz�k�z
    #endif // LCDBLINKCHAR

    LcdWrite();

    // ----------------------------------- stereo kijelz� eset�n, a m�sik is r�gt�n �rhat�
    #ifdef LCDE2

    ch.chr = LcdText[LcdPos + LCDLINES * LCDWIDTH];
    #ifdef LCDZEROCHANGE
    if (ch.chr == 0)
    {
      ch.chr = ' ';
      #ifdef LCDZEROCHANGETEXT
      LcdText[LcdPos + LCDLINES * LCDWIDTH] = ' '; // LcdText-ben is #0 -> 'SPACE' csere
      #endif
    }
    #endif // LCDZEROCHANGE

    #ifdef LCDBLINKCHAR
    if((BlinkPhase) && (LcdBlink[(LcdPos + LCDLINES * LCDWIDTH) >> 3] & (1 << ((LcdPos + LCDLINES * LCDWIDTH) & 7))))
      ch.chr = ' ';
    #endif // LCDBLINKCHAR

    LcdWrite2();
    #endif // LCDE2
    //------------------------------------

    LcdPos++;

    //------------------------------------ 1 soros
    #if LCDLINES == 1
    if(LcdPos == LCDWIDTH)              // 1 soros 1.sor v�ge?
    {
      #ifdef LCDCURSOR
      LcdStatus = CURPOS;               // kurzor poz�ci� be�ll�t�s
      #else // LCDCURSOR
      #ifdef LCDMODEONCEIRQ
      LcdRefreshStop();                 // k�sz az LCD friss�t�se, le�ll�that�
      #else  // LCDMODEONCEIRQ
      LcdStatus = REFREND;              // k�sz az LCD friss�t�se, LcdRefresh while ciklusa befejez�dhet
      #endif // else LCDMODEONCEIRQ
      #endif // LCDCURSOR

      #if ((defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
      LcdStatus = DDR;                  // folyamatos friss�t�si �zemm�dban DDRAM �ll�t�s lesz majd
      #endif
    }
    //------------------------------------ 2 soros
    #elif LCDLINES == 2
    if(LcdPos == LCDWIDTH)              // 2 soros, 1.sor v�ge?
    {
      LcdStatus = DDR;                  // DDRAM �ll�t�s lesz majd
    }
    else if(LcdPos == LCDWIDTH * 2)     // 2 soros, 2.sor v�ge?
    {
      #ifdef LCDCURSOR
      LcdStatus = CURPOS;               // kurzor poz�ci� be�ll�t�s
      #else // LCDCURSOR
      #ifdef LCDMODEONCEIRQ
      LcdRefreshStop();                 // k�sz az LCD friss�t�se, le�ll�that�
      #else  // LCDMODEONCEIRQ
      LcdStatus = REFREND;              // k�sz az LCD friss�t�se, LcdRefresh while ciklusa befejez�dhet
      #endif // else LCDMODEONCEIRQ
      #endif // LCDCURSOR

      #if ((defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
      LcdStatus = DDR;                  // folyamatos friss�t�si �zemm�dban DDRAM �ll�t�s lesz majd
      #endif
    }
    //------------------------------------ 4 soros
    #elif LCDLINES == 4
 
    //------------------------------------ 4 soros szimpla vez�rl�s
    if(LcdPos == LCDWIDTH)              // 4 soros, 1.sor v�ge?
    {
      LcdStatus = DDR;                  // DDRAM �ll�t�s lesz majd
    }
    else if(LcdPos == LCDWIDTH * 2)     // 4 soros, 2.sor v�ge?
    {
      LcdStatus = DDR;                  // DDRAM �ll�t�s lesz majd
    }
    else if(LcdPos == LCDWIDTH * 3)     // 4 soros, 3.sor v�ge?
    {
      LcdStatus = DDR;                  // DDRAM �ll�t�s lesz majd
    }
    else if(LcdPos == LCDWIDTH * 4)     // 4 soros, 4.sor v�ge?
    {
      #ifdef LCDCURSOR
      LcdStatus = CURPOS;               // kurzor poz�ci� be�ll�t�s
      #else // LCDCURSOR
      #ifdef LCDMODEONCEIRQ
      LcdRefreshStop();                 // k�sz az LCD friss�t�se, le�ll�that�
      #else  // LCDMODEONCEIRQ
      LcdStatus = REFREND;              // k�sz az LCD friss�t�se, LcdRefresh while ciklusa befejez�dhet
      #endif // else LCDMODEONCEIRQ
      #endif // LCDCURSOR

      #if ((defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
      LcdStatus = DDR;                  // folyamatos friss�t�si �zemm�dban DDRAM �ll�t�s lesz majd
      #endif
    }
    #endif // LCDLINES

  } // LcdStatus

  //----------------------------------------------------------------------------
  else if(LcdStatus == DDR)             // DDRAM �ll�tgat�s
  {
    // ----------------------------------- DDRAM �ll�tgat�s (sorok v�gein), villogtat�si �temez�s
    GPIOX_CLRBIT(LCDRS);                // parancs megy majd a kijelz�re (RS = 0)

    // ----------------------------------- 1 soros kijelz�
    #if LCDLINES == 1
    ch.chr = SETDDRAMADDR1;             // 1 soros, 1.sor v�ge -> DDRAM = 1.sor eleje
    LcdWrite();
    LcdWrite2();
    LcdPos = 0;
    BLINKER();

    // ----------------------------------- 2 soros kijelz�
    #elif LCDLINES == 2
    if(LcdPos == LCDWIDTH)
    {                                   // 2 soros, 1.sor v�ge
      ch.chr = SETDDRAMADDR2;           // DDRAM = 2.sor eleje
      LcdWrite();
      LcdWrite2();
    }
    else
    {                                   // 2 soros, 2.sor v�ge
    ch.chr = SETDDRAMADDR1;             // DDRAM = 1.sor eleje
      LcdWrite();
      LcdWrite2();
      LcdPos = 0;
      BLINKER();
    }

    // ----------------------------------- 4 soros kijelz�
    #elif LCDLINES == 4

    // 4 soros szimpla vez�rl�s (max 80 karakteres)
    if(LcdPos == LCDWIDTH)              // 1. sor v�ge ?
    { 
      ch.chr = SETDDRAMADDR2;           // DDRAM = 2.sor eleje
      LcdWrite();
      LcdWrite2();
    }
    else if(LcdPos == (LCDWIDTH * 2))   // 2. sor v�ge ?
    {
      ch.chr = SETDDRAMADDR3;           // DDRAM = 3.sor eleje
      LcdWrite();
      LcdWrite2();
    }
    else if(LcdPos == (LCDWIDTH * 3))   // 3. sor v�ge ?
    {
      ch.chr = SETDDRAMADDR4;           // DDRAM = 4.sor eleje
      LcdWrite();
      LcdWrite2();
    }
    else if(LcdPos == (LCDWIDTH * 4))   // 4. sor v�ge ?
    {
      ch.chr = SETDDRAMADDR1;           // DDRAM = 1.sor eleje
      LcdWrite(); 
      LcdWrite2();
      LcdPos = 0;
      BLINKER();
    }
    #endif // LCDLINES
    LcdStatus = LCHAR;                  // a k�vetkez� h�v�sn�l m�r karakter megy a kijelz�re
  }

  //----------------------------------------------------------------------------
  else if(LcdStatus == HOME)            // DDRAM = 0
  {
    GPIOX_CLRBIT(LCDRS);                // parancs megy majd a kijelz�re (RS = 0)
    ch.chr = SETDDRAMADDR1;             // DDRAM = 1.sor eleje
    LcdWrite();
    LcdWrite2();
    LcdPos = 0;
    LcdStatus = LCHAR;                  // a k�vetkez� h�v�sn�l m�r karakter megy a kijelz�re
  }

  #ifdef LCDCURSOR
  //----------------------------------------------------------------------------
  else if(LcdStatus == CURPOS)          // kurzor poz�ci� �ll�t�si f�zis
  {
    GPIOX_CLRBIT(LCDRS);                // parancs megy majd a kijelz�re (RS = 0)

    // ----------------------------------- 1 soros kijelz�
    #if LCDLINES == 1
    ch.chr = SETDDRAMADDR1 + LcdCursorPos;// DDRAM = 1.sor + kurzor poz�ci�
    LcdWrite();

    #elif LCDLINES == 2
    // ----------------------------------- 2db 2 soros kijelz�
    #ifdef LCDE2
    if(LcdCursorPos < LCDCHARPERMODUL)
    {                                   // a kurzor az 1.modulban van
      if (LcdCursorPos < LCDWIDTH)
        ch.chr = SETDDRAMADDR1 + LcdCursorPos;// DDRAM = 1.sor + kurzor poz�ci�
      else
        ch.chr = SETDDRAMADDR2 - LCDWIDTH + LcdCursorPos;// DDRAM = 2.sor - 1*sorhossz + kurzor poz�ci�
    }
    else
    {                                   // a kurzor az 2.modulban van
      if (LcdCursorPos < LCDCHARPERMODUL + LCDWIDTH)
        ch.chr = SETDDRAMADDR1 - LCDCHARPERMODUL + LcdCursorPos;// DDRAM = 1.sor - modulkaraktersz�m + kurzor poz�ci�
      else
        ch.chr = SETDDRAMADDR2 - LCDCHARPERMODUL - LCDWIDTH + LcdCursorPos;// DDRAM = 2.sor - modulkaraktersz�m - 1*sorhossz + kurzor poz�ci�
    }
    LcdWrite();
    LcdWrite2();
    #else  // LCDE2

    // ----------------------------------- 2 soros kijelz�
    if (LcdCursorPos < LCDWIDTH)
      ch.chr = SETDDRAMADDR1 + LcdCursorPos;// DDRAM = 1.sor + kurzor poz�ci�
    else
      ch.chr = SETDDRAMADDR2 - LCDWIDTH + LcdCursorPos;// DDRAM = 2.sor - 1 * sorhossz + kurzor poz�ci�
    LcdWrite();
    #endif // else LCDE2

    // ----------------------------------- 2db 4 soros kijelz�
    #elif LCDLINES == 4
    #ifdef LCDE2

    if(LcdCursorPos < LCDCHARPERMODUL)
    {                                   // a kurzor az 1.modulban van
      if(LcdCursorPos < LCDWIDTH)
      {
        ch.chr = SETDDRAMADDR1 + LcdCursorPos;// DDRAM = 1.sor + kurzor poz�ci�
      }
      else if(LcdCursorPos < 2 * LCDWIDTH)
      {
        ch.chr = SETDDRAMADDR2 - LCDWIDTH + LcdCursorPos;// DDRAM = 2.sor - 1*sorhossz + kurzor poz�ci�
      }
      else if(LcdCursorPos < 3 * LCDWIDTH)
      {
        ch.chr = SETDDRAMADDR3 - 2*LCDWIDTH + LcdCursorPos;// DDRAM = 3.sor - 2*sorhossz + kurzor poz�ci�
      }
      else
      {
        ch.chr = SETDDRAMADDR4 - 3*LCDWIDTH + LcdCursorPos;// DDRAM = 4.sor - 3*sorhossz + kurzor poz�ci�
      }
    }
    else
    {                                   // a kurzor az 2.modulban van
      if(LcdCursorPos < LCDWIDTH + LCDCHARPERMODUL)
      {
        ch.chr = SETDDRAMADDR1 - LCDCHARPERMODUL + LcdCursorPos;// DDRAM = 1.sor + kurzor poz�ci�
      }
      else if(LcdCursorPos < 2 * LCDWIDTH + LCDCHARPERMODUL)
      {
        ch.chr = SETDDRAMADDR2 - LCDCHARPERMODUL - LCDWIDTH + LcdCursorPos;// DDRAM = 2.sor - 1*sorhossz + kurzor poz�ci�
      }
      else if(LcdCursorPos < 3 * LCDWIDTH + LCDCHARPERMODUL)
      {
        ch.chr = SETDDRAMADDR3 - LCDCHARPERMODUL - 2*LCDWIDTH + LcdCursorPos;// DDRAM = 3.sor - 2*sorhossz + kurzor poz�ci�
      }
      else
      {
        ch.chr = SETDDRAMADDR4 - LCDCHARPERMODUL - 3*LCDWIDTH + LcdCursorPos;// DDRAM = 4.sor - 3*sorhossz + kurzor poz�ci�
      }
    }
    LcdWrite();
    LcdWrite2();
  
    // ----------------------------------- 4 soros dupla vez�rl�s kijelz� (2db 2soros)
    #else  // LCDE2
    if(LcdCursorPos < LCDWIDTH)
    {
      ch.chr = SETDDRAMADDR1 + LcdCursorPos;// DDRAM = 1.sor + kurzor poz�ci�
    }
    else if(LcdCursorPos < 2 * LCDWIDTH)
    {
      ch.chr = SETDDRAMADDR2 - LCDWIDTH + LcdCursorPos;// DDRAM = 2.sor - 1*sorhossz + kurzor poz�ci�
    }
    else if(LcdCursorPos < 3 * LCDWIDTH)
    {
      ch.chr = SETDDRAMADDR3 - 2*LCDWIDTH + LcdCursorPos;// DDRAM = 3.sor - 2*sorhossz + kurzor poz�ci�
    }
    else
    {
      ch.chr = SETDDRAMADDR4 - 3*LCDWIDTH + LcdCursorPos;// DDRAM = 4.sor - 3*sorhossz + kurzor poz�ci�
    }
    LcdWrite();

    #endif // else LCDE2
    #endif // LCDLINES 

    LcdStatus = CURTYPE;                // a k�vetkez� h�v�sn�l kurzort�pus be�ll�t�s
  }

  //----------------------------------------------------------------------------
  else if(LcdStatus == CURTYPE)         // kurzor t�pus �ll�t�si f�zis
  {
    GPIOX_CLRBIT(LCDRS);                // parancs megy majd a kijelz�re (RS = 0)

    #ifdef LCDE2
    if(LcdCursorPos < LCDCHARPERMODUL)
    { // els� modulban van
      ch.chr = LcdCursorType | 0b00001100;
      LcdWrite();                       // kurzor a be�ll�t�s
      ch.chr = 0b00001100;
      LcdWrite2();                      // kurzor kikapcs
    }
    else
    { // m�sodik modulban van a kurzor
      ch.chr = 0b00001100;
      LcdWrite();                       // kurzor kikapcs
      ch.chr = LcdCursorType | 0b00001100;
      LcdWrite2();                      // kurzor a be�ll�t�s
    }

    #else  // LCDE2
    ch.chr = LcdCursorType | 0b00001100;
    LcdWrite();
    #endif // LCDE2

    #ifdef LCDMODEONCEIRQ
    LcdRefreshStop();                   // k�sz az LCD friss�t�se, le�ll�that�
    #else  // LCDMODEONCEIRQ
    LcdStatus = REFREND;                // k�sz az LCD friss�t�se, LcdRefresh while ciklusa befejez�dhet
    #endif // else LCDMODEONCEIRQ
  }
  #endif // LCDCURSOR

  //----------------------------------------------------------------------------
  // Karakterk�szlet friss�t�s RAM ter�letr�l
  #ifdef USERCHARSETCHANGE
  else if(LcdStatus == CGR)             // CGRAM c�m be�ll�t�si f�zis
  {
    GPIOX_CLRBIT(LCDRS);                // parancs megy a kijelz�re (RS = 0)
    ch.chr = SETCGRAMADDR;              // CGRAM = 0
    LcdWrite();
    LcdWrite2();
    LcdPos = 0;                         // karakter gener�tor karakter sz�ml�l�
    LcdStatus = CHARGEN;                // karakter gener�tor felt�lt�se k�vetkezik
  }

  //----------------------------------------------------------------------------
  else if(LcdStatus == CHARGEN)         // karakter gener�tor felt�lt�se
  {
    GPIOX_SETBIT(LCDRS);                // karakter megy a kijelz�re (RS = 1)
    ch.chr = *uchp++;                   // karakter le�r� t�mb
    LcdWrite();
    LcdWrite2();
    LcdPos++;
    if(LcdPos >= 64)
    {                                   // karaktergener�tor felt�lt�s befejez�d�tt
      #if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY))
      #ifdef LCDCURSOR
      LcdStatus = CURPOS;               // egyszeri friss�t�si m�d, megszak�t�s n�lk�l kurzorral: kurzor poz�ci� be�ll�t�s
      #else  // LCDCURSOR
      LcdStatus = REFREND;              // egyszeri friss�t�si m�d, megszak�t�s n�lk�l: k�sz az LCD karaktergener�tor friss�t�se -> LcdRefresh while ciklusa befejez�dhet
      #endif // else LCDCURSOR
      #endif // ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY))

      #ifdef LCDMODEONCEIRQ
      LcdStatus = HOME;                 // egyszeri friss�t�si m�d megszak�t�ssal: DDRAM = 0 lesz a k�vetkez� k�rben
      #endif // LCDMODEONCEIRQ

      #if ((defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
      LcdStatus = HOME;                 // folyamatos friss�t�si m�d: DDRAM = 0 lesz a k�vetkez� k�rben
      #ifdef AUTOBLINKER
      #if LCDCHARPERMODUL <= 24
      BLINKER();                        // hogy a villog�si �temez�s ne maradjon ki
      BLINKER();                        // 1x16, 1x20-as kijelz�kn�l a 64 byte felt�lt�se kb annyi ideig tart mint n�gy frame
      BLINKER();
      BLINKER();
      #elif LCDCHARPERMODUL <= 48
      BLINKER();                        // hogy a villog�si �temez�s ne maradjon ki
      BLINKER();                        // 2x16, 2x20-as kijelz�kn�l a 64 byte felt�lt�se kb annyi ideig tart mint k�t frame
      #else  // LCDCHARPERMODUL
      BLINKER();                        // nagyobb kijelz�kn�l a 64 byte felt�lt�se kb annyi ideig tart mint egy frame
      #endif // LCDCHARPERMODUL
      #endif // AUTOBLINKER
      #endif // (defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ)
    }
  }
  #endif // USERCHARSETCHANGE

  //----------------------------------------------------------------------------
  // Karakterk�szlet friss�t�s ROM ter�letr�l
  #ifdef USERCHARSETCHANGEROM
  else if(LcdStatus == CGRROM)          // CGRAM c�m be�ll�t�si f�zis
  {
    GPIOX_CLRBIT(LCDRS);                // parancs megy a kijelz�re (RS = 0)
    ch.chr = SETCGRAMADDR;              // CGRAM = 0
    LcdWrite();
    LcdWrite2();
    LcdPos = 0;                         // karakter gener�tor karakter sz�ml�l�
    LcdStatus = CHARGENROM;             // karakter gener�tor felt�lt�se k�vetkezik
  }

  //----------------------------------------------------------------------------
  else if(LcdStatus == CHARGENROM)      // karakter gener�tor felt�lt�se
  {
    GPIOX_SETBIT(LCDRS);                // karakter megy a kijelz�re (RS = 1)
    ch.chr = *uchpr++;                  // karakter le�r� t�mb
    LcdWrite();
    LcdWrite2();
    LcdPos++;
    if(LcdPos >= 64)
    {                                   // karaktergener�tor felt�lt�s befejez�d�tt
      #if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY))
      #ifdef LCDCURSOR
      LcdStatus = CURPOS;               // egyszeri friss�t�si m�d, megszak�t�s n�lk�l kurzorral: kurzor poz�ci� be�ll�t�s
      #else  // LCDCURSOR
      LcdStatus = REFREND;              // egyszeri friss�t�si m�d, megszak�t�s n�lk�l: k�sz az LCD karaktergener�tor friss�t�se -> LcdRefresh while ciklusa befejez�dhet
      #endif // else LCDCURSOR
      #endif // ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY))

      #ifdef LCDMODEONCEIRQ
      LcdStatus = HOME;                 // egyszeri friss�t�si m�d megszak�t�ssal: DDRAM = 0 lesz a k�vetkez� k�rben
      #endif // LCDMODEONCEIRQ

      #if ((defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ))
      LcdStatus = HOME;                 // folyamatos friss�t�si m�d: DDRAM = 0 lesz a k�vetkez� k�rben
      #ifdef AUTOBLINKER
      #if LCDCHARPERMODUL <= 24
      BLINKER();                        // hogy a villog�si �temez�s ne maradjon ki
      BLINKER();                        // 1x16, 1x20-as kijelz�kn�l a 64 byte felt�lt�se kb annyi ideig tart mint n�gy frame
      BLINKER();
      BLINKER();
      #elif LCDCHARPERMODUL <= 48
      BLINKER();                        // hogy a villog�si �temez�s ne maradjon ki
      BLINKER();                        // 2x16, 2x20-as kijelz�kn�l a 64 byte felt�lt�se kb annyi ideig tart mint k�t frame
      #else  // LCDCHARPERMODUL
      BLINKER();                        // nagyobb kijelz�kn�l a 64 byte felt�lt�se kb annyi ideig tart mint egy frame
      #endif // LCDCHARPERMODUL
      #endif // AUTOBLINKER
      #endif // (defined LCDMODECONTBUSY) || (defined LCDMODECONTIRQ)
    }
  }
  #endif // USERCHARSETCHANGEROM
}

#if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ))
//==============================================================================
// LcdRefreshAll (csak egyszeri friss�t�si �zemm�dban: a teljes kijelz�tartalom friss�t�se)
// El�felt�tel: LcdInit() inicializ�lni kell
// Input:           LcdText[], villog� m�dban LcdBlink[]
// �ttekint�s:      LcdText[] teljes tartalm�t �tm�solja az LCD kijerlz�re 
//                  megszak�t�s n�lk�li m�d:
//                    addig tart a f�ggv�m lefut�sa, am�g az �tm�sol�s tart (blokkol� f�ggv�ny!)
//                  megszak�t�sos m�d:
//                    csak elind�tja a m�sol�st, a karakterek ki�r�sa megszak�t�sb�l fog megt�rt�nni,
//                    a teljes tartalom ki�r�sa ut�n le�ll�tja a friss�t�st
// Megjegyz�s:      
//==============================================================================
void LcdRefreshAll(void)
{
  #ifdef LCDMODEONCEIRQ
  #ifdef USERCHARSETCHANGE
  if(!LcdRefreshed())
  { // friss�t�s folyamatban van m�g
    if((LcdStatus == CGR) || (LcdStatus == CHARGEN))
    { // karaktergener�tor friss�t�s van folyamatban, semmit nem kell csin�lni
    } // mert a karaktergener�tor v�gezt�vel a kijelz� tartalmat is �jra�rja
    else
    { // sz�veg friss�t�s van m�g folyamatban -> el�lr�l kezdeni
      LcdStatus = HOME;
      LcdRefreshStart();                // mehet (ha pont most �llt volna le)
    }
  }
  else
  { // nincs friss�t�s alatt
    LcdStatus = HOME;                   // megszak�t�s, kijelz� friss�t�st el�lr�l kezdeni
    LcdRefreshStart();                  // mehet
  }
  #else  // USERCHARSETCHANGE
  LcdStatus = HOME;                     // megszak�t�s, karakterk�szlet m�dos�t�s n�lk�l
  LcdRefreshStart();
  #endif // USERCHARSETCHANGE
  #endif // LCDMODEONCEIRQ

  #ifdef LCDMODEONCEBUSY
  LcdStatus = HOME;                     // megszak�t�s n�lk�l
  while(LcdStatus != REFREND)
    LcdProcess();                       // megv�rjuk am�g ki�r�dik
  #endif

  #ifdef LCDMODEONCEDELAY
  LcdStatus = HOME;                     // megszak�t�s n�lk�l
  while(LcdStatus != REFREND)
  {
    DelayLcd();                         // Lcd �r�s v�rakoz�s
    LcdProcess();                       // megv�rjuk am�g ki�r�dik
  }
  #endif
}
#endif //  ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ))

#ifdef USERCHARSETCHANGE
//==============================================================================
// LcdChangeCharset (a 8db felhaszn�l� �ltal defini�lt karakterk�szlet �tvitele CGRAM-ba )
// El�felt�tel: LcdInit(), USERCHARSETCHANGE
// Input:           pch*: a 64 b�jtos karaktert�bla c�me a RAM-ban
// �ttekint�s:      Egyszeri friss�t�si �zemm�dban:
//                  megszak�t�s n�lk�li m�d:
//                    addig tart a f�ggv�m lefut�sa, am�g az �tm�sol�s tart (blokkol� f�ggv�ny, 
//                    csak a karakterk�szletet m�solja �t, a kijelz� tartalmat nem!)
//                  megszak�t�sos m�d:
//                    csak elind�tja a m�sol�st, a karakterk�szlet �s a kijelz� tartalm�nak ki�r�sa 
//                    (a karakterk�szlet ut�n a sz�veget is �jra�rja) megszak�t�sb�l fog megt�rt�nni,
//                    a teljes tartalom ki�r�sa ut�n le�ll�tja a friss�t�st
//
//                  Folyamatos friss�t�si �zemm�dban:
//                  megszak�t�s n�lk�li m�d: 
//                    A kijelz�re t�rt�n� ki�r�s a f�programhurokban az LcdProcess() f�ggv�nyh�v�ssal fog megt�rt�nni (ha a kijelz� szabad).
//                    Ha a teljes karakterk�szletet ki�rta, akkor automatikusan a tartalom ki�r�s�ra t�r �t.
//                  megszak�t�sos m�d:
//                    csak elind�tja a m�sol�st, a karakterk�szlet �s a kijelz� tartalm�nak ki�r�sa megszak�t�sb�l fog megt�rt�nni,
//                    a teljes tartalom ki�r�sa ut�n automatikusan a tartalom ki�r�s�ra t�r �t.
// Megjegyz�s:
//==============================================================================
void LcdChangeCharset(char* pch)
{
  LcdStatus = CGR;
  uchp = pch;

  #ifdef LCDMODEONCEIRQ
  LcdRefreshStart();                    // egyszeri friss�t�si �zemm�d, megszak�t�sban
  #endif

  #ifdef LCDMODEONCEBUSY
  while(LcdStatus != REFREND)           // egyszeri friss�t�si �zemm�d, megszak�t�s n�lk�l
    LcdProcess();                       // megv�rjuk am�g ki�r�dik
  #endif

  #ifdef LCDMODEONCEDELAY
  while(LcdStatus != REFREND)           // egyszeri friss�t�si �zemm�d v�rakoz�sos m�d
  {
    DelayLcd();                         // Lcd �r�s v�rakoz�s
    LcdProcess();                       // megv�rjuk am�g ki�r�dik
  }
  #endif
}
#endif // USERCHARSETCHANGE

#ifdef USERCHARSETCHANGEROM
//==============================================================================
// LcdChangeCharset (a 8db felhaszn�l� �ltal defini�lt karakterk�szlet �tvitele CGRAM-ba )
// El�felt�tel: LcdInit(), USERCHARSETCHANGE
// Input:           pch*: a 64 b�jtos karaktert�bla c�me a ROM-ban
// �ttekint�s:      Egyszeri friss�t�si �zemm�dban:
//                  megszak�t�s n�lk�li m�d:
//                    addig tart a f�ggv�m lefut�sa, am�g az �tm�sol�s tart (blokkol� f�ggv�ny,
//                    csak a karakterk�szletet m�solja �t, a kijelz� tartalmat nem!)
//                  megszak�t�sos m�d:
//                    csak elind�tja a m�sol�st, a karakterk�szlet �s a kijelz� tartalm�nak ki�r�sa
//                    (a karakterk�szlet ut�n a sz�veget is �jra�rja) megszak�t�sb�l fog megt�rt�nni,
//                    a teljes tartalom ki�r�sa ut�n le�ll�tja a friss�t�st
//
//                  Folyamatos friss�t�si �zemm�dban:
//                  megszak�t�s n�lk�li m�d:
//                    A kijelz�re t�rt�n� ki�r�s a f�programhurokban az LcdProcess() f�ggv�nyh�v�ssal fog megt�rt�nni (ha a kijelz� szabad).
//                    Ha a teljes karakterk�szletet ki�rta, akkor automatikusan a tartalom ki�r�s�ra t�r �t.
//                  megszak�t�sos m�d:
//                    csak elind�tja a m�sol�st, a karakterk�szlet �s a kijelz� tartalm�nak ki�r�sa megszak�t�sb�l fog megt�rt�nni,
//                    a teljes tartalom ki�r�sa ut�n automatikusan a tartalom ki�r�s�ra t�r �t.
// Megjegyz�s:
//==============================================================================
void LcdChangeCharsetRom(rom char* pch)
{
  LcdStatus = CGRROM;
  uchpr = pch;

  #ifdef LCDMODEONCEIRQ
  LcdRefreshStart();                    // egyszeri friss�t�si �zemm�d, megszak�t�sban
  #endif

  #ifdef LCDMODEONCEBUSY
  while(LcdStatus != REFREND)           // egyszeri friss�t�si �zemm�d, megszak�t�s n�lk�l
    LcdProcess();                       // megv�rjuk am�g ki�r�dik
  #endif

  #ifdef LCDMODEONCEDELAY
  while(LcdStatus != REFREND)           // egyszeri friss�t�si �zemm�d v�rakoz�sos m�d
  {
    DelayLcd();                         // Lcd �r�s v�rakoz�s
    LcdProcess();                       // megv�rjuk am�g ki�r�dik
  }
  #endif
}
#endif // USERCHARSETCHANGE

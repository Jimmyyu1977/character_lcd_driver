// k�sz�t�: Roberto Benjami
// verzi�:  v0.65 2017.11

#ifndef __CHARLCD_H
#define __CHARLCD_H

//------------------------------------------------------------------------------
// Karakteres LCD kiszolg�l� elj�r�sok
// - Inicializ�l�s LcdInit() f�ggv�nnyel (ez p�rsz�z msec id�t ig�nyel) 
// - A megjelen�tend� sz�veget LcdText[] t�mbbe tegy�k be. 
//   Ez tartalmazza amit a kijelz�nek meg kell jelen�tenie. 
//   A #0 karakterek helyett sz�k�zt �rhat a kijelz�re (be�ll�t�st�l f�gg�en).
//
// �zemm�dok (az 5 �zemm�db�l csak egyet v�lassz ki!):
//-------------------------------------
// #define LCDMODEONCEBUSY
// - Egyszeri friss�t�si �zemm�d megszak�t�s n�lk�l, BUSY flag figyel�ssel:
//   haszn�lata:
//     1. Az LcdText[]-be be�rjuk a megjelen�tend� sz�veget.
//     2. LcdRefreshAll() f�ggv�ny megh�v�s�val �tm�soljuk a kijelz�be.
//   korl�toz�sok:
//     R/W l�bat a foglalts�gi �llapot visszaolvas�s miatt mindenk�pp haszn�lni kell (LCDRWUSED).
//     Automata villogtat�s nem m�k�dik, csak �gy lehet a villog�st megval�s�tani,
//     ha a villog�si �temnek megfelel� id�ben BlinkPhase v�ltoz�ba betessz�k 
//     az aktu�lis villog�si f�zist, majd LcdRefreshAll() f�ggv�nyt ism�t megh�vjuk.
//   megjegyz�s: 
//     Az LcdRefreshAll() BLOKKOL� t�pus� f�ggv�ny, addig nem t�r vissza, 
//     am�g a kijelz� friss�t�se folyamatban van.
//     Figyelem! Kijelz� n�lk�l, hib�s vagy rosszul bek�t�tt kijelz�vel v�gtelen ciklusba ker�lhet a program.
//
//-------------------------------------
// #define LCDMODEONCEDELAY
// - Egyszeri friss�t�si �zemm�d megszak�t�s n�lk�l, LCD �r�si m�veletek k�z�tt v�rakoz�si ciklussal:
//   haszn�lata:
//     1. Az LcdText[]-be be�rjuk a megjelen�tend� sz�veget.
//     2. LcdRefreshAll() f�ggv�ny megh�v�s�val �tm�soljuk a kijelz�be.
//   korl�toz�sok:
//     Automata villogtat�s nem m�k�dik, csak �gy lehet a villog�st megval�s�tani,
//     ha a villog�si �temnek megfelel� id�ben BlinkPhase v�ltoz�ba betessz�k 
//     az aktu�lis villog�si f�zist, majd LcdRefreshAll() f�ggv�nyt ism�t megh�vjuk.
//   megjegyz�s: 
//     Lehet�s�g van a kijelz� R/W l�b�t GND-re k�tve egy I/O l�bat megsp�rolni.
//     Az LcdRefreshAll() BLOKKOL� t�pus� f�ggv�ny, addig nem t�r vissza,
//     am�g a kijelz� friss�t�se folyamatban van.
//
//-------------------------------------
// #define LCDMODEONCEIRQ
// - Egyszeri friss�t�si �zemm�d megszak�t�ssal:
//   haszn�lata:
//     1. Az LcdText[]-be be�rjuk a megjelen�tend� sz�veget.
//     2. LcdRefreshAll() f�ggv�ny megh�v�s�val �tm�soljuk a kijelz�be.
//   korl�toz�sok:
//     Automata villogtat�s nem m�k�dik, csak �gy lehet a villog�st megval�s�tani,
//     ha a villog�si �temnek megfelel� id�ben BlinkPhase v�ltoz�ba betessz�k 
//     az aktu�lis villog�si f�zist, majd LcdRefreshAll() f�ggv�nyt ism�t megh�vjuk.
//   megjegyz�s:
//     Lehet�s�g van a kijelz� R/W l�b�t GND-re k�tve egy I/O l�bat megsp�rolni.
//     Az LcdRefreshAll() NEM BLOKKOL� t�pus� f�ggv�ny, csak elind�tja a kijelz� friss�t�s�t,
//     a friss�t�s maga megszak�t�sb�l fog megt�rt�nni.
//     A friss�t�s befejezt�t az LcdRefreshed() f�ggv�nnyel (val�j�ban ez egy v�ltoz�) k�rdezhetj�k le:
//     pl. a while(!LcdRefreshed()); addig v�rakozik, am�g a friss�t�s folyamatban van.
//     �nmag�ban nem okoz gondot, ha a friss�t�s befejez�se el�tt �jb�l megh�vjuk az LcdRefreshAll() f�ggv�nyt.
//     Ilyenkor f�lbehagyja az el�z� friss�t�si ciklust, �s el�lr�l kezdi.
//     Ha viszont folyamatosan f�lbeszak�tjuk, akkor lehet hogy a sz�veg v�ge sosem fog megjelenni mert nem jut el a v�g�ig.
//     8 bites PIC eset�n a k�z�s megszak�t�skiszolg�l� f�ggv�nybe be kell illeszteni egy LcdIntProcess() f�ggv�nyh�v�st.
//
//-------------------------------------
// #define  LCDMODECONTBUSY
// - Folyamatos friss�t�si �zemm�d megszak�t�s n�lk�l:
//   haszn�lata:
//     1. Az LcdText[]-be be�rjuk a megjelen�tend� sz�veget
//     2. LcdUpdateChar() f�ggv�nyt folyamatosan h�vogatva id�vel �t fog ker�lni a kijelz�re
//        (ezt a f�ggv�nyt a f�programhurokba �rdemes berakni, hogy rendszeresen megh�v�djon)
//   korl�toz�sok:
//     R/W l�bat a foglalts�gi �llapot visszaolvas�s miatt mindenk�pp haszn�lni kell (LCDRWUSED).
//     Automata villogtat�s nem m�k�dik, csak �gy lehet a villog�st megval�s�tani,
//     ha a villog�si �temnek megfelel� id�ben BlinkPhase v�ltoz�ba betessz�k az aktu�lis villog�si f�zist.
//     Kurzor nem haszn�lhat�.
//   megjegyz�s: 
//     Az LcdUpdateChar() NEM BLOKKOL� t�pus� f�ggv�ny, ha a kijelz� foglalt -> azonnal visszat�r,
//     ha szabad -> egy (dupla kijelz�s m�dban kett�) karaktert �r ki a kijelz�(k)re.
//     
//-------------------------------------
#define  LCDMODECONTIRQ
// - Folyamatos friss�t�si �zemm�d megszak�t�ssal:
//   haszn�lata:
//     1. Az LcdText[]-be be�rjuk a megjelen�tend� sz�veget
//     2. PIC16, PIC18 eset�n a k�z�s megszak�t�skiszolg�l� programba beillesztj�k
//        az LcdIntProcess() f�ggv�nyh�v�st.
//   korl�toz�sok:
//     Kurzor nem haszn�lhat�.
//   megjegyz�s: 
//     Lehet�s�g van a kijelz� R/W l�b�t GND-re k�tve egy I/O l�bat megsp�rolni.
//     Automata villogtat�s csak ebben az �zemm�dban haszn�lhat�.

//==============================================================================
#include "hardwareprofile.h"
// Ha k�ls� hardverprofilt haszn�lunk akkor azt itt adhatjuk meg
// - bels� hardverprofil eset�n kommentelj�k ki
// - k�ls� hardverprofil eset�n az ott megadott param�tereket ebben a file-ban kommentelj�k ki
//------------------------------------------------------------------------------
// LCD kijelz� I/O l�bak:
// - LCDE: E l�b hozz�rendel�se
// - LCDE2: E2 l�b hozz�rendel�se (csak k�t kijelz�, vagy dupla kijelz� eset�n asjuk meg)
// - LCDRW: RW l�b hozz�rendel�se (csak akkor adjuk meg, ha haszn�ljuk is, nem hasznlva a GND-re kell k�tni)
// - LCDDT0..LCDDT3: D0..D3 l�b hozz�rendel�se (csak 8 bites �zemmodban adjuk meg)
// - LCDDT4..LCDDT7: D4..D7 l�b hozz�rendel�se

#ifndef LCDE         // ha hardwareprofile.h -ban E l�b hozz�rendel�s�t megadjuk, akkor a t�bbit is ott adjuk meg
#if defined(__18CXX)
// E, E2, R/W (interrupt m�dban lehet n�lk�l�zni), RS, D0..D7 (4bites m�dban csak D4..D7 lesz haszn�lva)
#define LCDE(f)      f(A, 4)
#define LCDE2(f)     f(B, 3)
#define LCDRW(f)     f(B, 5)
#define LCDRS(f)     f(B, 4)
#define LCDDT0(f)    f(C, 0)
#define LCDDT1(f)    f(C, 1)
#define LCDDT2(f)    f(D, 2)
#define LCDDT3(f)    f(D, 3)
#define LCDDT4(f)    f(B, 0)
#define LCDDT5(f)    f(B, 1)
#define LCDDT6(f)    f(B, 2)
#define LCDDT7(f)    f(B, 3)
#else   // defined(__18CXX)
// E, E2, R/W (interrupt m�dban lehet n�lk�l�zni), RS, D0..D7 (4bites m�dban csak D4..D7 lesz haszn�lva)
#define LCDE         A, 4
#define LCDE2        B, 3
#define LCDRW        B, 5
#define LCDRS        B, 4
#define LCDDT0       C, 0
#define LCDDT1       C, 1
#define LCDDT2       D, 2
#define LCDDT3       D, 3
#define LCDDT4       B, 0
#define LCDDT5       B, 1
#define LCDDT6       B, 2
#define LCDDT7       B, 3

#endif  // else defined(__18CXX)
#endif  // LCDE

//------------------------------------------------------------------------------
// #define SystemClock 4000000
// #define CpuClock    4000000
// Rendszer �rajel frekvencia (sz�ks�ges az id�z�t�sekhez �s a timerek be�ll�t�s�hoz)
// (ha a CpuClock megegyezik SystemClock-al, akkor CpuClock-ot nem k�telez� megadni)

//==============================================================================
#define LCDLINES  2
// Sorok sz�ma (1, 2, 4 lehet)
//------------------------------------------------------------------------------
#define LCDWIDTH  16
// Soronk�nti karakterek sz�ma (16, 20, 24, 40 lehet)
//------------------------------------------------------------------------------
#define LCDTIMERNUM  2
// Timer interrrupt m�d: a felhaszn�lt timer kiv�laszt�sa
// (lehet�leg olyat, amelyiket nincs m�sra haszn�lva :-)
// - AVR:   0, 1, 2
// - PIC16: 0, 1, 2
// - PIC18: 0, 1, 2, 3
// - PIC24: 1, 2, 3, 4, 5
//------------------------------------------------------------------------------
#define LCDTIMERPR18  -1
// PIC18 csak megszak�t�s m�dban: melyik priorit�s� megszak�t�st haszn�lja az id�z�t�h�z
// lehets�ges �rt�kei:
// - -1 : nincs a t�bbszint� priorit�s haszn�lva (ilyenkor minden csak a HIGH interruptban megy)
// -  0 : alacsony priorit�s
// -  1 : magas priorit�s
//------------------------------------------------------------------------------
// #define LCDUSERTIMER
// Csak megszak�t�s m�dban, lehet�s�g van saj�t id�z�t�s szerint hivogatni az LcdIntProcess() friss�t� rutint.
// Ilyenkor nem t�rt�nik meg a TIMER inicializ�l�sa, azt nek�nk kell megoldanunk.
// Akkor �rdemes ezt haszn�lni, ha m�s c�lra is haszn�lunk id�z�t�t, ami a kijelz� friss�t�si
// �temez�se szerinti temp�ban j�r.
//------------------------------------------------------------------------------
// #define LCDGLOBALINTAUTOSTART
// Csak megszak�t�s m�dban, ha ez enged�lyezve van, az inicializ�l�skor bekapcsolja
// a glob�lis megszak�t�st is. Ha az inicaializ�l�skor m�g ezt nem szeretn�nk megtenni
// kommentelj�k ezt ki!
//------------------------------------------------------------------------------
#define LCDFRAMEPERSEC 20
// Csak megszak�t�s m�dban: a kijelz� friss�t�si sebess�ge (FPS)
// aj�nlott �rt�k: 20
//------------------------------------------------------------------------------
#define LCDEXECUTIONTIME 150
// Csak LCDMODEONCEDELAY m�dban felhaszn�lva: a kijelz� parancsv�grehajt�si ideje (usec)
// (A kijelz� adatlapj�n EXECUTION TIME tal�ljuk. 40usec a tipikus ideje, legal�bb ekkora id�t v�lasszunk!)
// aj�nlott �rt�k 50
//------------------------------------------------------------------------------
#define LCDBLINKCHAR
// Villogtathat�ak a karakterek
// - villog�s bekapcsol�sa: LcdBlinkChar(n); (n = karakter sz�ma)
// - villog�s kikapcsol�sa: LcdUnBlinkChar(n); (n = karakter sz�ma)
//------------------------------------------------------------------------------
#define LCDBLINKSPEED 5
// A villog� karakterek ennyi ideig frame-ig lesznek s�t�tek �s vil�gosak
// - csak LCDMODECONTIRQ �zemm�dban!
// - a t�bbi �zemmm�dban saj�t magunknak kell el��ll�tani
//   a villog�si �temet �s BlinkPhase v�ltoz�ba rakni !
// - ha LCDMODECONTIRQ �zemm�dban automatikus helyett
//   saj�t villog�si �temez�st akarsz -> LCDBLINKSPEED 0 �rt�ket adj!
//------------------------------------------------------------------------------
// #define LCDCURSOR
// Kurzorhaszn�lat enged�lyez�se (csak egyszeri friss�t�si �zemm�dban)
// - kurzor poz�ci� be�ll�t�sa: LcdCursorPos = n (0 < n < kijelz�(k) karaktereinek sz�ma - 1)
// - kurzor bekapcsol�sa: LcdCursorOn()
// - kurzor kikapcsol�sa: LcdCursorOff()
// - villog� kurzor bekapcsol�sa: LcdCursorBlink()
// - villog� kurzor kikapcsol�sa: LcdCursorUnBlink()
//------------------------------------------------------------------------------
#define LCDZEROCHANGE
// Ha defini�lva van, az LcdText-ben lev� #0 k�d� karakterek helyett #32 (SPACE) karaktert
// �r ki a kijelz�re (a sz�m�talak�t� f�ggv�nyek #0 karaktert tesznek a sz�m v�g�re).
// Kommenteld ki, ha a #0 k�d� karaktert a felhaszn�l� �ltal defini�lt karakterhez akarod haszn�lni.
// (a #0..#7 k�d� karakter a #8..#15 k�dokon is el�rhet�, ez�rt a kikommentel�st nem javaslom)
//==============================================================================
#define USERCHARSET
// Saj�t karakterk�szlet felt�lt�se inicializ�l�skor.
// Enged�lyezve az USERxCHARy fogja defini�lni a karakterek form�j�t.
// Ezt �t lehet szerkeszteni saj�t ig�nyeink szerint.
//------------------------------------------------------------------------------
#define USERCHARSETCHANGE
// Karakterk�szlet fut�s alatti v�ltoztat�s�nak lehet�s�ge RAM ter�letr�l.
// - LcdChangeCharset(char* pch) f�ggv�nnyel lehet felt�lteni az aktu�lis karakterk�szletet.
//   a karakterk�szletet USERCHARSETARRAY t�pus� v�ltoz�val lehet l�trehozni a RAM-ban.
//------------------------------------------------------------------------------
// #define USERCHARSETCHANGEROM
// Karakterk�szlet fut�s alatti v�ltoztat�s�nak lehet�s�ge ROM ter�letr�l.
// - LcdChangeCharsetRom(rom char* pch) f�ggv�nnyel lehet felt�lteni az aktu�lis karakterk�szletet.
//   a karakterk�szletet USERCHARSETARRAY t�pus� v�ltoz�val lehet l�trehozni a ROM-ban.
//------------------------------------------------------------------------------
// Saj�t karakterk�szlet haszn�lata eset�n CGRAM-ba fel lehet t�lteni 8db 5x8-as karaktert.
// Az al�bbi minta '0'..'7' sz�mot defini�l inverz kijelz�ssel.
// Ha az 5x8-es karakterm�trix bitje = 0 -> vil�gos, 1 -> fekete (vil�gos h�tter� kijelz� eset�n).
// Ezeket el�rni a 0..7, ill 8..15 karakterk�dokon kereszt�l lehet.
// Javaslom a 8..15  k�dokat haszn�lni, mert a program a 0 k�dot -> 32 (SPACE) k�dd� alak�thatja.
// Ezt az�rt teszi hogy a k�nyvt�ri �talak�t�f�ggv�nyek �ltal berakott #0 karaktererek
// ne l�tsz�djanak.
// Figyelem: ha a legals� sor fekete, akkor nem l�tszik az als� vonalas kurzor!

#define USER0CHAR0   0b11111
#define USER0CHAR1   0b11011
#define USER0CHAR2   0b10101
#define USER0CHAR3   0b10101
#define USER0CHAR4   0b10101
#define USER0CHAR5   0b10101
#define USER0CHAR6   0b11011
#define USER0CHAR7   0b11111

#define USER1CHAR0   0b11111
#define USER1CHAR1   0b11101
#define USER1CHAR2   0b11001
#define USER1CHAR3   0b10101
#define USER1CHAR4   0b11101
#define USER1CHAR5   0b11101
#define USER1CHAR6   0b11101
#define USER1CHAR7   0b11111

#define USER2CHAR0   0b11111
#define USER2CHAR1   0b10011
#define USER2CHAR2   0b10101
#define USER2CHAR3   0b11101
#define USER2CHAR4   0b11011
#define USER2CHAR5   0b10111
#define USER2CHAR6   0b10001
#define USER2CHAR7   0b11111

#define USER3CHAR0   0b11111
#define USER3CHAR1   0b10011
#define USER3CHAR2   0b11101
#define USER3CHAR3   0b10011
#define USER3CHAR4   0b11101
#define USER3CHAR5   0b11101
#define USER3CHAR6   0b10011
#define USER3CHAR7   0b11111

#define USER4CHAR0   0b11111
#define USER4CHAR1   0b11101
#define USER4CHAR2   0b11001
#define USER4CHAR3   0b10101
#define USER4CHAR4   0b10001
#define USER4CHAR5   0b11101
#define USER4CHAR6   0b11101
#define USER4CHAR7   0b11111

#define USER5CHAR0   0b11111
#define USER5CHAR1   0b10001
#define USER5CHAR2   0b10111
#define USER5CHAR3   0b10011
#define USER5CHAR4   0b11101
#define USER5CHAR5   0b11101
#define USER5CHAR6   0b10011
#define USER5CHAR7   0b11111

#define USER6CHAR0   0b11111
#define USER6CHAR1   0b11001
#define USER6CHAR2   0b10111
#define USER6CHAR3   0b10011
#define USER6CHAR4   0b10101
#define USER6CHAR5   0b10101
#define USER6CHAR6   0b11011
#define USER6CHAR7   0b11111

#define USER7CHAR0   0b11111
#define USER7CHAR1   0b10001
#define USER7CHAR2   0b11101
#define USER7CHAR3   0b11101
#define USER7CHAR4   0b11011
#define USER7CHAR5   0b10111
#define USER7CHAR6   0b10111
#define USER7CHAR7   0b11111

//==============================================================================
// CharLcd "driver" optimaliz�l�si lehet�s�gek
// (ha nem akarunk �lni vele kommentelj�k ki mindet)

//------------------------------------------------------------------------------
// LCD adatl�bak optimaliz�l�si lehet�s�ge (hogy gyors�tani lehessen az eredeti bitenk�nti �r�st)
// - LCDDTIN csak LCDMODEONCEBUSY �s LCDMODECONTBUSY �zemm�dban van haszn�lva
// - LCDDT0TO3, LCDDT4TO7 csak 4 bites m�dban van haszn�lva
// - LCDDT0TO7 csak 8 bites m�dban van haszn�lva
// - a kiirand� adat ch.chr-ben illetve ch.bit0...ch.bit7-ben �ll rendelkez�sre
// - (ha egym�st k�vet� l�bakra ker�lnek az adat l�bak, v0.64-tol kezdve a ford�to is tudja optimaliz�lni)

// LCD adatl�bakra csatlakoz� I/O bemenetre �ll�t�sa (LCD -> CPU)
// #define LCDDTIN   TRISB |=  (0b00001111)
// LCD adatl�bakra csatlakoz� I/O kimenetre �ll�t�sa (CPU -> LCD)
// #define LCDDTOUT  TRISB &= ~(0b00001111)
// 4bites m�dban az also 4bit ki�r�sa (az adat a ch.chr als� fel�ben van jelen)
// #define LCDDT0TO3 LATB = (LATB & ~(0b00001111)) | (ch.chr & 0b00001111)
// 4bites m�dban a fels� 4bit ki�r�sa (az adat a ch.chr fels� fel�ben van jelen)
// #define LCDDT4TO7 LATB = (LATB & ~(0b00001111)) | ((ch.chr >> 4) & 0b00001111)
// 8bites m�dban mind a 8bit ki�r�sa (az adat ch.chr-ben van jelen)
// #define LCDDT0TO7 LATB = ch.chr

//------------------------------------------------------------------------------
// Mivel a ford�t�programok nem optimaliz�lnak ki egy f�ggv�nyt, ha ASM bet�t is van benne (a NOP defin�ci�ja ASM bet�t)
// �rdemes olyan utas�t�sra cser�lni, ami nem fog semmit csin�lni, de egy utas�t�sciklusnyi ideig tart.
// (pl. <64kByte mem�ri�ju PIC18-n�l a PCLATU egy�bk�nt is mindig nulla, semmi nem t�rt�nik ha lenull�zzuk)
// #define LCDNOP  PCLATU = 0

//==============================================================================
// Innent�l kezdve nem kell beleny�lni !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//==============================================================================
// Haszn�lhat� adatok:
//
// - char LcdText[] : ebbe a karaktereket tartalmaz� t�mbbe kell elhelyezni a megjelen�tend� sz�veget
//
// - unsigned char BlinkPhase : villog�si f�zis (ha 0 -> villog� karakterek l�tszanak, 1 -> nem l�tszanak)
//     Megszak�t�sos folyamatos friss�t�si �zemm�dban, ha LCDBLINKSPEED > 0) nem kell haszn�lni,
//     ilyenkor automata m�dban megy a villogtat�s
//     A t�bbi �zemm�dban nek�nk kell a villog�si �temez�st biztos�tani)
//
// - unsigned char LcdCursorPos : kurzor poz�ci� (egyszeri friss�t�si �zemm�dban, ha enged�lyezve van)
//
// - USERCHARSETARRAY : a fut�s k�zbeni felhaszn�l�i karakterk�szletet ilyen t�pus� v�ltoz�ba lehet elt�rolni
//                      ez egy 64karaktert t�rol� t�mb 
//==============================================================================
// Haszn�lhat�/haszn�land� f�ggv�nyek:
//
// - LcdInit() : Az LCD inicializ�s�t, �s a felhaszn�l�i karakterek felt�lt�s�t v�gzi (fut�si ideje kb. 130msec).
//
// - LcdProcess() : LCDMODECONTBUSY �zemm�dban ezt kell rendszeresen megh�vni, hogy friss�lj�n a kijelz� tartalma.
//
// - LcdIntProcess() : PIC16xxx, PIC18xxx megszak�t�sos �zemm�dban a k�z�s megszak�t�skiszolg�l�b�l kell ezt a f�ggv�nyt megh�vni.
//                     Lehet�s�g van saj�t id�z�t�sb�l is hivogatni a friss�t� rutint, akkor is ezt kell haszn�lni (AVR �s PIC24-en is).
//
// - LcdBlinkChar(n) : 'n'-edik karakter (0..karakterek sz�ma-1) villogni fog
//
// - LcdUnBlinkChar(n) : 'n'-edik karakter (0..karakterek sz�ma-1) nem fog villogni
//
// - LcdRefreshStart() : Megszak�t�sos �zemm�dban ezzel lehet az ideiglenesen le�ll�tott friss�t�st �jra elind�tani.
//
// - LcdRefreshStop() : Megszak�t�sos �zemm�dban ezzel lehet a friss�t�st ideiglenesen le�ll�tani.
//                      Pl. ha pontos id�z�t�s miatt nem akarjuk, hogy megszak�t�s keletkezzen az LCD miatt.
//
// - LcdRefreshAll() : Egyszeri friss�t�si �zemm�dban ha �jra szeretn�nk �rni az LCD-t ezzel tehetj�k meg.
//                     Akkor c�lszer� ezt megtenni, amikor az LcdText t�mbben teljesen �ssze�llt a megjelen�tend� sz�veg
//                     Megszak�t�s n�lk�l addig a f�ggv�nyben marad, am�g az LCD �r�sa tart,
//                     megszak�t�sos m�dban r�gt�n visszat�r. Ekkor az �r�s befejzt�t az LcdRefreshed() f�ggv�nnyel k�rdezhetj�k le.
//
// - LcdRefreshed() : Megszak�t�sos egyszeri friss�t�si �zemm�dban:
//                    ha a visszat�r� �rt�k == 0 -> LcdRefreshAll() f�ggv�nyh�v�s ut�n m�g tart a friss�t�s
//                    pl. a while(!LcdRefreshed()); addig v�rakozik, am�g a friss�t�s folyamatban van
//
// - LcdCursorOn()      : Kurzor bekapcsol�sa.
// - LcdCursorOff()     : Kurzor kikapcsol�sa.
// - LcdCursorBlink()   : Kurzor villog�s bekapcsol�sa.
// - LcdCursorUnBlink() : Kurzor villog�s kikapcsol�sa (egy als� vonal marad a kurzor).
//
// - LcdChangeCharset(char* pch) : Fut�s alatt a felhaszn�l�i karakterk�szlet cser�je.
//                                 Param�terk�nt a karaktereket t�rol� 64b�jtos t�mb c�m�t (USERCHARSETARRAY) kell megadni.
//
//------------------------------------------------------------------------------

// Sz�veg t�mb
extern volatile char LcdText[];

// Villog�s t�mb, villog�s ki/be
#ifdef LCDBLINKCHAR
extern volatile char LcdBlink[];
extern volatile unsigned char BlinkPhase;// 0 = villog� karakterek l�tszanak, egy�bk�nt nem l�tszik
#define LcdBlinkChar(s) LcdBlink[ s >> 3 ] |= (1 << ( s & 7))
#define LcdUnBlinkChar(s) LcdBlink[ s >> 3 ] &= ~(1 << ( s & 7))
#endif // LCDBLINKCHAR

// Felhaszn�l�i karakterk�szlet
#ifdef USERCHARSET
typedef unsigned char USERCHARSETARRAY[64];
#if defined(__18CXX)
extern rom USERCHARSETARRAY userromcharset;
#else  // defined(__18CXX)
extern const USERCHARSETARRAY userromcharset; 
#endif // else defined(__18CXX)
#ifdef USERCHARSETCHANGE
void LcdChangeCharset(char* pch);
#endif // USERCHARSETCHANGE
#endif // USERCHARSET

// Kurzor
#ifdef LCDCURSOR
#define LcdCursorOn()         LcdCursorType |= 2
#define LcdCursorOff()        LcdCursorType &= ~2
#define LcdCursorBlink()      LcdCursorType |= 1
#define LcdCursorUnBlink()    LcdCursorType &= ~1
extern volatile unsigned char LcdCursorType;
extern volatile unsigned char LcdCursorPos;
#endif  // LCDCURSOR

// LCD inicializ�l�sa
void LcdInit(void);

// Egy karakter friss�t�se (csak folyamatos friss�t�si �zemm�dban, megszak�t�s n�lk�l)
#ifdef LCDMODECONTBUSY
void LcdProcess(void);
#endif

// Teljes kijelz�tartalom friss�t�se (csak egyszeri friss�t�si �zemm�dban)
#if ((defined LCDMODEONCEBUSY) || (defined LCDMODEONCEDELAY) || (defined LCDMODEONCEIRQ))
void LcdRefreshAll(void);
#endif

//------------------------------------------------------------------------------
#if ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))

#ifdef LCDUSERTIMER

// saj�t megszak�t�skezel�vel �s id�z�t�skezel�vel haszn�lva
extern unsigned char LcdIrqStatus;
void LcdProcess(void);
#define LcdIntProcess()    if(!LcdIrqStatus)LcdProcess()
#define LcdRefreshStart()  LcdIrqStatus = 0
#define LcdRefreshStop()   LcdIrqStatus = 1
#ifdef  LCDMODEONCEIRQ
#define LcdRefreshed()     LcdIrqStatus
#endif // LCDMODEONCEIRQ

#else  // LCDUSERTIMER

// AVR: LcdRefreshStart(), LcdRefreshStop(), LcdRefreshed()
#if (defined(__AVR__))

#if LCDTIMERNUM == 0
#if defined OCR0 || defined OCR0A
#ifdef TIMSK0
#define LcdRefreshStart()  {TCNT0 = 0; TIMSK0 = (1<<OCIE0A);}
#define LcdRefreshStop()   TIMSK0 = 0
#define LcdRefreshed()     (!TIMSK0)
#else  // TIMSK0
#define LcdRefreshStart()  {TCNT0 = 0; TIMSK |= (1<<OCIE0);}
#define LcdRefreshStop()   TIMSK &= ~(1<<OCIE0)
#define LcdRefreshed()     (!(TIMSK & (1<<OCIE0)))
#endif // TIMSK0
#else  // defined OCR0 || defined OCR0A
#define LcdRefreshStart()  {TCNT0 = 0; TIMSK = (1<<TOIE0);}
#define LcdRefreshStop()   TIMSK = 0
#define LcdRefreshed()     (!TIMSK)
#endif // defined OCR0 || defined OCR0A

#elif LCDTIMERNUM == 1
// timer bekapcsol�s
#ifdef TIMSK1
#define LcdRefreshStart()  {TCNT1 = 0; TIMSK1 = (1<<OCIE1A);}
#define LcdRefreshStop()   TIMSK1 = 0
#define LcdRefreshed()     (!TIMSK1)
#else  // TIMSK1
#define LcdRefreshStart()  {TCNT1 = 0; TIMSK |= (1<<OCIE1A);}
#define LcdRefreshStop()   TIMSK &= ~(1<<OCIE1A)
#define LcdRefreshed()     (!(TIMSK & (1<<OCIE1A)))
#endif // TIMSK1

#elif LCDTIMERNUM == 2
#ifdef TIMSK2
#define LcdRefreshStart()  {TCNT2 = 0; TIMSK2 = (1<<OCIE2A);}
#define LcdRefreshStop()   TIMSK2 = 0
#define LcdRefreshed()     (!TIMSK2)
#else  // TIMSK2
// #define LcdRefreshStart()  {IFR0 &= ~(1<<OCIF2); TIMSK |= (1<<OCIE2);}
#define LcdRefreshStart()  {TCNT2 = 0; TIMSK |= (1<<OCIE2);}
#define LcdRefreshStop()   TIMSK &= ~(1<<OCIE2)
#define LcdRefreshed()     (!(TIMSK & (1<<OCIE2)))
#endif // TIMSK2
#endif // LCDTIMERNUM
#endif // defined(__AVR__)

//------------------------------------------------------------------------------
// PIC16: LcdIntProcess(), LcdRefreshStart(), LcdRefreshStop(), LcdRefreshed()
// interruptba LcdIntProcess() f�ggv�nyt be kell tenni
#if (defined(_PIC14) || defined(_PIC14E))
void LcdProcess(void);

#if LCDTIMERNUM == 0
#define LcdIntProcess()    if(INTCONbits.T0IE && INTCONbits.T0IF)LcdProcess()
#define LcdRefreshStart()  {TMR0 = 0; INTCONbits.T0IE = 1;}
#define LcdRefreshStop()   INTCONbits.T0IE = 0
#define LcdRefreshed()     (!INTCONbits.T0IE)
#elif LCDTIMERNUM == 1
#define LcdIntProcess()    if(PIR1bits.TMR1IF)LcdProcess()
#define LcdRefreshStart()  T1CONbits.TMR1ON = 1
#define LcdRefreshStop()   T1CONbits.TMR1ON = 0
#define LcdRefreshed()     (!T1CONbits.TMR1ON)
#elif LCDTIMERNUM == 2
#define LcdIntProcess()    if(PIR1bits.TMR2IF)LcdProcess()
#define LcdRefreshStart()  T2CONbits.TMR2ON = 1
#define LcdRefreshStop()   T2CONbits.TMR2ON = 0
#define LcdRefreshed()     (!T2CONbits.TMR2ON)
#endif // LCDTIMERNUM
#endif // defined(_PIC14)

//------------------------------------------------------------------------------
// PIC18: LcdIntProcess(), LcdRefreshStart(), LcdRefreshStop(), LcdRefreshed()
// LOW interruptba LcdIntProcess() f�ggv�nyt be kell tenni
#if ((defined(__18CXX)) || (defined(__PICC18__)))
void LcdProcess(void);

#if LCDTIMERNUM == 0
#define LcdIntProcess()    if(INTCONbits.TMR0IF)LcdProcess()
#define LcdRefreshStart()  T0CONbits.TMR0ON = 1
#define LcdRefreshStop()   T0CONbits.TMR0ON = 0
#define LcdRefreshed()     (!T0CONbits.TMR0ON)
#elif LCDTIMERNUM == 1
#define LcdIntProcess()    if(PIR1bits.TMR1IF)LcdProcess()
#define LcdRefreshStart()  T1CONbits.TMR1ON = 1
#define LcdRefreshStop()   T1CONbits.TMR1ON = 0
#define LcdRefreshed()     (!T1CONbits.TMR1ON)
#elif LCDTIMERNUM == 2
#define LcdIntProcess()    if(PIR1bits.TMR2IF)LcdProcess()
#define LcdRefreshStart()  T2CONbits.TMR2ON = 1
#define LcdRefreshStop()   T2CONbits.TMR2ON = 0
#define LcdRefreshed()     (!T2CONbits.TMR2ON)
#elif LCDTIMERNUM == 3
#define LcdIntProcess()    if(PIR2bits.TMR3IF)LcdProcess()
#define LcdRefreshStart()  T3CONbits.TMR3ON = 1
#define LcdRefreshStop()   T3CONbits.TMR3ON = 0
#define LcdRefreshed()     (!T3CONbits.TMR3ON)
#endif // LCDTIMERNUM
#endif // defined(__18CXX)

//------------------------------------------------------------------------------
// PIC30: LcdRefreshStart(), LcdRefreshStop(), LcdRefreshed()
#if (defined(__C30__))

#if LCDTIMERNUM == 1
#define LcdRefreshStart()  T1CONbits.TON = 1
#define LcdRefreshStop()   T1CONbits.TON = 0
#define LcdRefreshed()     (!T1CONbits.TON)
#elif LCDTIMERNUM == 2
#define LcdRefreshStart()  T2CONbits.TON = 1
#define LcdRefreshStop()   T2CONbits.TON = 0
#define LcdRefreshed()     (!T2CONbits.TON)
#elif LCDTIMERNUM == 3
#define LcdRefreshStart()  T3CONbits.TON = 1
#define LcdRefreshStop()   T3CONbits.TON = 0
#define LcdRefreshed()     (!T3CONbits.TON)
#elif LCDTIMERNUM == 4
#define LcdRefreshStart()  T4CONbits.TON = 1
#define LcdRefreshStop()   T4CONbits.TON = 0
#define LcdRefreshed()     (!T4CONbits.TON)
#elif LCDTIMERNUM == 5
#define LcdRefreshStart()  T5CONbits.TON = 1
#define LcdRefreshStop()   T5CONbits.TON = 0
#define LcdRefreshed()     (!T5CONbits.TON)
#endif // LCDTIMERNUM
#endif // defined(__C30__)

#endif // else LCDUSERTIMER

#endif // ((defined LCDMODEONCEIRQ) || (defined LCDMODECONTIRQ))

#endif // __CHARLCD_H

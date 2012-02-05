/************************************************************************
Title:			eDIP240-Header-File
Version:		1.0
Created:		12/10/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284 @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 


#ifndef EDIP240_H_
#define EDIP240_H_

// dip240i.h
// ---------     Version 13.04.2007
//               bearbeitet: www.GoBlack.de, D.Schwarzer
// ----------------------------------------------------------------

#ifndef _dip240i_H
#define _dip240i_H
#include "i2c.h"

// Diese Headerdatei fasst die Funktionen zusammen, welche zum
// Betrieb des EAeDIP240 TouchDisplay an einem I2C-Bus des IPC@CHIP
// benötigt werden.

// vorzunehmende Voreinstellungen an der Anzeige zum Betrieb des
// I2C-Busses:
// 1. I2CMODE -Jumper auf GND setzen
// 2. Wie lautet die an der EA eDIP240-Anzeige gejumperte Adresse
//    für den I2C-Bus? Vorgabe, alle Jumper offen => 0xDE

   #define  i2c_eaDIP  0xDE        // Ba2|Ba1|Ba0|Sa2|Sa1|Sa0|R/W
                                   // alle Pins offen, also 1


// ----------------------------------------------------------------
// Definition von globalen Variablen für Informationen von und über
// die eDIP-Anzeige. Diese können von externen Programmteilen be-
// nutzt werden.

   #define  ibuflen  64       // Buffergrösse des dipin -Buffers
   char  dipin [ibuflen];     // Empfangsbuffer der Touch-Eingaben
                              // von dip240in()

   char  rbuf= 0;             // verbleibend freie Bytes im
                              // Empfangsbuffer der Anzeige
   char  sbuf= 0;             // vorhandene Bytes im Sende-
                              // buffer der Anzeige
                              // von dip240getbuf()

   char  mps = 0;             // maximal Paketgrösse (64 Bytes)
   char  aps = 0;             // aktuell eingest. Paketgrösse
   char  ato = 0;             // aktuell eingest. Timeout
                              // von dip240getprot()

   char  version[64]="";      // Versionsstring d. Anzeige
                              // von dip240vers()


// Kerntreiber der jeweiligen Schnittstelle. Sie müssen
// ersetzt werden bzw. entfallen, wenn eine andere Schnittstelle
// verwendet werden soll.
   char dip240init(void);     // Schnittstelle initialisieren
   void dip240release(void);  // Schnittstelle entlassen

   char dip240rec(void);      // füllt den Buffer dipin[]
   char dip240cmd(char typ, char* string, unsigned len);

// Handshakezeichen der EA eDIP240-Schnittstelle
   #define DC1 0x11
   #define DC2 0x12
   #define ACK 0x06
   #define NAK 0x15

// ASCII Codes im Terminalmodus der Anzeige
   #define ESC 0x1B     // Ersatzzeichen  #
   #define CR  0x0D     // Ersatzzeichen \r
   #define LF  0x0A     // Ersatzzeichen \n
   #define FF  0x0C     // Ersatzzeichen \f
   #define BS  0x08     // Ersatzzeichen <bs>



// ----------------------------------------------------------------
// Funktionen, welche die Protokolle bedienen
// Diese Funktionen sind unabhängig von der gewählten Schnittstelle
// Die zentralen Sende- und Empfangsfunktionen sind dip240out() und
// dip240in()
// ----------------------------------------------------------------

// dip240out()
// Sende-Protokoll: <DC1> len data bcc
// Sendet DC1 Protokollrahmen mit Kommandos oder Daten der Befehls-
// liste des Datenblatts in 'string' zur Anzeige. Der 'string' ist
// nicht nullterminiert. Die Anzahl der Daten in 'string' wird in
// 'len' angegeben. So kann auch die Null als Datum gesendet
// werden.
// Rückgabe: 1 -Daten wurden gesendet
//           0 -Timeoutfehler

char dip240out(char* string, unsigned len)
{
 return(dip240cmd(DC1, string, len));
}

// dip240in()
// Anforderungs-Protokoll: <DC2> 1 S bcc
// Empfängt DC1/DC2 Rückgaberahmen vom Display, im Besonderen
// von der Druckfolien-Eingabe.
// Rückgabe: 1 -Daten liegen in dipin[] vor, wobei dipin[0]
//              die Anzahl der Daten enthält.
//           0 -Timeoutfehler

char dip240in(void)
{
 if(!dip240cmd(DC2,"S",1)) return(0);
 return(dip240rec());
}

// ----------------------------------------------------------------

// dip240getbuf()
// Anforderungs-Protokoll: <DC2> 1 I bcc
// Bufferinformationen vom Display holen. Zurückgegeben und in
// globalen Variablen gespeichert, werden ..
// sbuf -die Anzahl der Daten im Sendebuffer
// rbuf -die Anzahl freier Speicher im Empfangsbuffer

char dip240getbuf(void)
{
 if(!dip240cmd(DC2,"I",1)) return(0);    // Anforderung senden
 if(!dip240rec()) return(0);             // Daten holen
 if(dipin[0]!=2)  return(0);             // Länge muss 2 sein
 sbuf=dipin[1];                          // Bytes im Sendebuffer
 rbuf=dipin[2];                          // Frei im Empfangsbuffer
 return(1);
}

// dip240setprot()
// Sende-Protokoll: <DC2> 3 D psfsb timo bcc
// nicht bei Version 1.2 möglich
// Protokoll im Display setzen. Übergeben werden ..
// 1. max Bytes der Sendebytes von der Anzeige  Var: psfsb
// 2. Timeoutzeit bei unvollst. Empfang d.Anz   Var: timo

char dip240setprot(char psfsb, char timo)
{
 char cmd[]={'D',64, 200};
 if(psfsb!=0 && timo!=0){
    cmd[1]=psfsb; cmd[2]= timo;
 }
 if(!dip240cmd(DC2,cmd,3)) return(0);
 return(1);
}

// dip240getprot()
// Anforderungs-Protokoll: <DC2> 1 P bcc
// nicht bei Version 1.2 möglich
// Protokollinformationen vom Display holen. Diese werden in den
// folgenden globalen Variablen abgelegt ..
// mps -maximale möglich Paketgrösse (64 Bytes)
// aps -aktuelle Paketgrösse     (siehe auch setprot())
// ato -die aktuelle Timeoutzeit (siehe auch setprot())
char dip240getprot(void)
{
 if(!dip240cmd(DC2,"P",1)) return(0);
 if(!dip240rec()) return(0);
 if(dipin[0] !=3) return(0);
 mps=dipin[1];
 aps=dipin[2];
 ato=dipin[3];
 return(1);
}

// dip240sendlastPack()
// Anforderungs-Protokoll: <DC2> 1 R bcc
// wird in der Funktion dip240rec(), verwendet und ist hier nicht
// als Funktion hinterlegt.

 

// ----------------------------------------------------------------
// dip240vers()
// allgemeine ESC-Sequenz: ESC S V
// benutzt dip240out() und dip240in() (siehe oben)
// fordert die Anzeige auf, ihre Versionsnummer im Sendebuffer zu
// hinterlegen. Diese wird mit dip240in() gelesen und im Buffer 
// dipin[], in der Form dipin[0]=Gesamtlänge.. ESC, V, anz, String
// abgelegt.
// Der empfangene String wird isoliert, nullterminiert und in die
// globale Variablen 'version' kopiert.
// Rückgabe: 1 -String wurde empfangen
//           0 -Timeout oder Fehler in Stringkennung

int dip240vers(void)
{
  char n=0;
  if(!dip240out("\x1BSV",3)) return(0);
  if(!dip240in()) return(0);
  if(dipin[1]!=0x1B || dipin[2]!='V') return(0);
  for(n=0; n<dipin[3]; n++) version[n]= dipin[n+4];
  version[n+1]=0;
  return(1);
}


// ----------------------------------------------------------------
// Basistreiber für den I2C-Bus
// Diese Programme bedingen die Includedatei ..      'i2c.h'
// ----------------------------------------------------------------

// dip240cmd()
// sendet DC1 oder DC2 Protokollrahmen und deren Daten über den
// I2C-Bus zur Anzeige.

// Es werden Portionen (Rahmen) von max. 64 Datenbytes gesendet.
// Wie viele Zeichen in 'string' zum Senden bereitstehen ist in
// 'len' anzugeben. Das Maximum für 'len' sind 65535 Bytes.

// Die Funktion wartet auf die Empfangsbestätigung der Anzeige.
// Bei ..
// ACK=0x06  Der Sendevorgang war erfolgreich,          Rückgabe 1
// NAK=0x15  Prüfsumme Display und gesendete Prüfsumme
//           differieren. Der Rahmen wird erneut
//           gesendet
// !=ACK     keine Antwort der Anzeige, weil Display
//           nicht vorhanden, oder Zeichen verloren-
//           gegangen                                   Rückgabe 0

// Rahmen:
// DC1 len data Prüfs.     -Daten senden
// DC2 1 S      Prüfs.     -Sendebuffer anfordern
// DC2 1 l      Prüfs.     -Bufferinfos anfordern
// DC2 3 D x x  Prüfs.     -Protokolleinst senden
// DC2 1 P      Prüfs.     -Protokollinfos anfordern
// DC2 1 R      Prüfs.     -Datenpack erneut anfordern
// DC2 3 A x x  Prüfs.     -RS232 eDIP selektieren

char dip240cmd(char typ, char* string, unsigned len)
{
 #define  datalen  64             // Anzahl Daten im
 char dipout[datalen+4];          // Sendebuffer
 unsigned datacnt=0;              // Zähler gesendeter Daten
 unsigned block;                  // Anzahl zu sendender Daten
 unsigned m=0;
 char bcc=0, n=0;

 // Blöcke zu 'datalen' Bytes senden, bis der string, der
 // 'len' Bytes lang ist gesendet wurde.
 while(datacnt<len){
   if(datalen<(len-datacnt)) block=datalen;
   else block=len-datacnt;

   neuSenden:
   bcc=dipout[0]=typ;
   for(n=0, m=datacnt; m<datacnt+block; n++,m++){
     dipout[n+2]=string[m];
     bcc+=string[m];
   }
   bcc+=n; dipout[1]=n; dipout[n+2]=bcc;
   //sendSerBlock(com, dipout, n+3);
   i2c_transblock (i2c_eaDIP, dipout, n+3);
   i2c_release();
   i2c_receive (i2c_eaDIP, &n, 0);
   i2c_release();
   if(n==NAK) goto neuSenden;
   if(n!=ACK) return(0);
   datacnt +=block;
 }
 return(1);
}


// dip240rec()
// liest die nach einem der obigen Anfordungsprotokolle, von der
// Anzeige hereinkommenden Daten. Diese können 64 Bytes
// (Vorgabewert) nicht überschreiten. Der Protokollrahmen wird
// entfernt,die Prüfsumme kontrolliert. Die Daten werden in dem
// globalen Buffer dipin[] abgelegt.
// dipin[0] enthält dabei die Anzahl der im Buffer folgenden
// Datenbytes. Beim Rückgabewert 0 der Funktion trat ein
// Empfangsproblem auf, bei 1 konnten Daten empfangen werden.

char dip240rec(void)
{ 
  char n=0, pb=0, c=0;

  // Buffer leeren bis DC1 oder DC2 erscheint.
  flush:
  i2c_receive (i2c_eaDIP, &c, 1);
  if(c!=0x11 && c!=0x12) goto flush;

  // DC1 oder DC2 gefunden
  // Wenn keine Zeichen im Display vorliegen sendet die Anzeige ..
  // 11 00 11.
  i2c_receive(i2c_eaDIP, &dipin[0],1);

  pb=c+dipin[0];
  for(n=0;n<dipin[0];n++){
    i2c_receive(i2c_eaDIP, &dipin[n+1],1);
    pb+=dipin[n+1];
  }

  // Prüfsumme holen und vergleichen
  i2c_receive(i2c_eaDIP,&c,0);
  i2c_release();
  if(pb != c){
    if(!dip240cmd(DC2, "R", 1)) return(0);
    goto flush;
  }
  return(1);
}

// dip240init()
// I2C-Bus zum Display initialisieren
// Rückgabe 0: Fehler Anzeige nicht gefunden
//          1: Anzeige wurde gefunden
char dip240init(void)
{
 // I2C-Bus einrichten und nach der Anzeige suchen
 i2c_init();
 if(i2c_scan(i2c_eaDIP,i2c_eaDIP)!=i2c_eaDIP) return(0);
 return(1);
}

// dip240release()
// I2C-Bus durch STOP-Bedingung freigeben
void dip240release(void)
{
 i2c_release();       // I2C-Bus STOP-Bedingung senden
}

#endif /* EDIP240_H_ */
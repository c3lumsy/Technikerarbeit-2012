/************************************************************************
Title:			gcode-Header-File
Version:		1.0
Created:		12/29/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.5.1
Hardware:		AVR ATmega1284P @ 18,432 MHz 

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de

				Open Source
************************************************************************/ 

#ifndef GOCNC_H_
#define GOCNC_H_

extern void gcode_g_check(void);
extern void gcode_m_check(void);
extern void gcode_t_check(void);



#endif /* GCODE_H_ */
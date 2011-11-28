/************************************************************************
Title:			goCNC-Header-File
Version:		1.0
Created:		11/22/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284 @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 


#ifndef GOCNC_H_
#define GOCNC_H_

extern void go_cnc(void);
extern void sqare(void);
extern void axis_ref(void);
extern int32_t fmm_to_ium(float f);

extern void z_move(int32_t way_um, uint16_t trip_speed);
extern void xy_move (uint16_t x_way_um, uint16_t y_way_um, uint16_t trip_speed);



#endif /* GOCNC_H_ */
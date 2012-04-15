/************************************************************************
Title:			MAIN-File
Version:		1.0
Created:		11/22/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.5.1
Hardware:		AVR ATmega1284P @ 18,432 MHz 
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
				
				Open Source
************************************************************************/ 
#include "globdef.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "i2cmaster.h"
#include "eDIP240.h"
#include "gocnc.h"
#include "vnc1l.h"

void maschine_init()
{
// CNC-Outputs / PA6 Ref-Switch
	DDRA |= _BV(7)|_BV(5)|_BV(4)|_BV(3)|_BV(2)|_BV(1)|_BV(0);   
	DDRD |= _BV(7)|_BV(6)|_BV(5);
	
// Delay for startup Display and VDRIVE2
	_delay_ms(500);
	
// Init UARTS
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU) ); 

	uart_puts("PWR_ON");
	uart_putc(CMD_CR);
// Init I2C
	i2c_init();
	
	uart_puts("I2C_OK");
	uart_putc(CMD_CR);
	
// IRQ defines

// IRQ for eDIP Touch PIN20
 	EICRA |= (1<<ISC21);				// INT2 on rising edge
 	EIMSK |= (1<<INT2);

	uart_puts("IRQ_OK");
	uart_putc(CMD_CR);


// IRQ for CTC-Mode	

// 	TCCR1A = 0x00;						// no Port operation, kein PWM
// 	TCCR1B = (1<<WGM12);				// CTC1
// 	TIMSK0 |= (1<<OCIE1A);				// Compare Match A Interrupt Enable
// 	 
	 

// Set Maschineflags
	M_FLAGS->ERROR_GLOB = 0;			// clear all errors	

	M_FLAGS->EDIP_ACTION = 0;			// no action from eDIP
	M_FLAGS->EDIP_CNC_HAND = 0;			// eDIP in Hand menu
	
	M_FLAGS->GCODE_FILESET = 0;
	M_FLAGS->GCODE_TOP = 0;
	M_FLAGS->GCODE_BOT = 0;
	
	M_FLAGS->USB_CON = 0;				// USB-Stick not connected till check
	M_FLAGS->USB_RDY = 0;
	M_FLAGS->USB_FILE_OPEN = 0;			// If Stick connected, first File is closed
	M_FLAGS->USB_FILE_EOF = 0;			// EndOfFile Flag cleared
	
	M_FLAGS->TOOL_Change = 0;		
	M_FLAGS->TOOL_OK = 0;
	
	M_FLAGS->AXIS_v1 = 2000;			// Set Maschinespeed to default
	
	// EDIP_POS_define
	X.AxisEdipPos = 105;
	Y.AxisEdipPos = 169;
	Z.AxisEdipPos = 233;
	
// Set default Axisspeed
	axis_set_speed(75);					// Speed in %

// Display RDY MSG
	_delay_ms(5000);
	edip_put_CMD(DC1,"#MT2");
	_delay_ms(1000);
	edip_msg(100);						// "Controller BEREIT!"

	uart_puts("INIT_OK");
	uart_putc(CMD_CR);
	 
// IRQ enable	 
    sei();								// Interrupts enable
	
	uart_puts("IRQ_ENABLE");
	uart_putc(CMD_CR);
}

void action_abort()
	{
		uart_puts("ABBRUCH");
		action_end();	
	}

void action_auto()
	{		
		uart_puts("AUTO");
		if (
			M_FLAGS->USB_CON 
			&& !M_FLAGS->USB_FILE_EOF
			&& !M_FLAGS->ERROR_GLOB
			&& M_FLAGS->TOOL_OK
			){
				while (usb_get_command() != 1)
					{
					}
			}
	
	}
	
void action_end()
{
	usb_close_file(FILENAME);
	edip_put_CMD(DC1,"#MT2");
	
	while (!(xyz_REF_SW))
		{
			axis_move(zAxis,1,v_ref);
		}
	axis_move(zAxis,-96,v_ref);

	axis_rel_move_interpol(0,0,0,0,M_FLAGS->AXIS_v1);
	edip_msg(108);
}


void error_wait()
	{
		M_FLAGS->ERROR_GLOB = 1;
		uart_puts("ERROR!");
	}

int main(void)
{
	maschine_init();					// first CMD in main-loop!
	
while (1)
{
	
	if (M_FLAGS->GCODE_FILESET == 1)
	{
		uart_putc(M_FLAGS->GCODE_TOP);
		uart_putc(M_FLAGS->GCODE_BOT);
		uart_puts(FILENAME);
		
		if (usb_open_file(FILENAME))
			{
				uart_puts("FILE OPEN");
			}
		else
			{
				uart_puts("FILE ERROR!");
			}
		
		M_FLAGS->GCODE_FILESET = 0;
	}
	
// Check if Tool Change
	if (M_FLAGS->TOOL_Change)
	{
		axis_tool_change();
		M_FLAGS->BREAK_Action = 0;
	}
	
// Check if new action from eDIP avaliable ?
	if (M_FLAGS->EDIP_ACTION == 1)
		{
			edip_check_input(eDIP_BUFFER);
		}

// Auto-Action
	if (
		(M_FLAGS->AUTO_Action)
		&& !(M_FLAGS->BREAK_Action)
		&& (M_FLAGS->USB_FILE_OPEN)
		&& !(M_FLAGS->USB_FILE_EOF)
		&& !(M_FLAGS->ABORT_Action)
		)
		{
			action_auto();
		}

	if (M_FLAGS->ABORT_Action)
		{
			action_abort();		
			M_FLAGS->ABORT_Action = 0;
		}

// Toggle LED´s on POLLIN-Board
	if (M_FLAGS->LIFEBIT == 1)
		{
			PORTD &= ~_BV(5);
			M_FLAGS->LIFEBIT = 0;
		} 
	else
		{
			PORTD |= _BV(5);
			M_FLAGS->LIFEBIT = 1;
			}
			
// Put new Pos to Display		
	if (M_FLAGS->EDIP_NEWPOS == 1)
		{
			int32_t temp = steps_to_um(X.AxisRelPos);
			edip_put_pos(X.AxisEdipPos,22,'R',temp);
		
			temp = steps_to_um(Y.AxisRelPos);
			edip_put_pos(Y.AxisEdipPos,22,'R',temp);
		
			temp = steps_to_um(Z.AxisRelPos);
			edip_put_pos(Z.AxisEdipPos,22,'R',temp);
			
			char* tempstring;
			ltoa(X.AxisAbsPos,tempstring,10);
			uart_puts(tempstring);
			ltoa(Y.AxisAbsPos,tempstring,10);
			uart_puts(tempstring);
			ltoa(Z.AxisAbsPos,tempstring,10);
			uart_puts(tempstring);
		
			M_FLAGS->EDIP_NEWPOS = 0;
		}
	
		
	}
	
	
}
	
// Interrupts
void UART1_RX_INT()
{
// 		char uart1_trans;
// 	
// 		uart1_trans = uart1_getc();
// 		if (uart1_trans != NULL)
// 		{
// 			uart_putc(uart1_trans);
// 		}
}

void UART0_RX_INT()
{
	char uart0_trans;
	
		uart0_trans = uart_getc();
		if (uart0_trans != NULL)
		{
			uart1_putc(uart0_trans);
		}
}

SIGNAL(INT2_vect)
{
// Interrupt from eDIP-Pin20
			cli();									// disable interrupts
			edip_get_buffer(eDIP_BUFFER);			// save I2C-BUFFER
			M_FLAGS->EDIP_ACTION = 1;				// set Flag for main-loop
			sei();									// enable interrupts
}

SIGNAL(TIMER0_COMPA_vect)
{
}

SIGNAL(TWI_vect)
{
}	
/* 
 * File:   digital_keypad.h
 * Author: Adil
 *
 * Created on 15 April, 2020, 8:22 PM
 */

#ifndef DIGITAL_KEYPAD_H
#define	DIGITAL_KEYPAD_H

#define LEVEL_DETECTION         0
#define LEVEL                   0

#define STATE_DETECTION         1
#define STATE                   1

#define KEYPAD_PORT             PORTB
#define KEYPAD_PORT_DDR         TRISB

#define INPUT_LINES             0x3F

#define SW1                     0x3E
#define SW2                     0x3D
#define SW3                     0x3B
#define SW4                     0x37
#define SW5                     0x2F
#define SW6                     0x1F


#define LONG_SW4              0xBB    //SW4
#define LONG_SW5              0xcc    //SW5

#define ALL_RELEASED            INPUT_LINES


//unsigned char read_digital_keypad(unsigned char mode);
unsigned char detect_key_press(void);
void init_digital_keypad(void);

#endif	/* DIGITAL_KEYPAD_H */


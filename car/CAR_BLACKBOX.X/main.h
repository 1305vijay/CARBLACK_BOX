/**************
 file : main.h
 ***************/

#ifndef MAIN_H
#define MAIN_H

#include <xc.h>
#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "eeprom.h"
#include "i2c.h"
#include "car_black_box_def.h"
#include "timers.h"
#include "uart.h"
#include <string.h>

#define DASHBOARD_SCREEN    0x01
#define LOGIN_SCREEN        0x02
#define MAIN_MENU_SCREEN    0x03

#define RESET_PASSWORD      0x11
#define RESET_NONE          0x22
#define RETURN_BACK         0x33
#define LOGIN_SUCCESS       0x44 
#define RESET_MENU          0x5A    //init 0x55

#define VIEW_LOG_SCREEN             0x66
#define CLEAR_LOG_SCREEN            0X77
#define DOWNLOAD_LOG_SCREEN         0x88
#define SET_TIME_LOG_SCREEN         0X99
#define CHANGE_PASSWD_LOG_SCREEN    0XAA

//defined in dkp.h file
//#define LONG_SW4            0xBB 
//#define LONG_SW5            0xcc
//#define LONG_PRESS_FLAG     0x80

#define OP_SUCCESS                 0xC1  
#define OP_FAILURE                 0xD2  
#define UPDATE_FIRMWARE_SCREEN     0xE3  


#endif /*MAIN_H*/


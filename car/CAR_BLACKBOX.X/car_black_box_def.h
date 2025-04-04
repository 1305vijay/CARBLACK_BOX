/***************************************
 file car black box defintions .h file
 ***************************************/

#ifndef CAR_BLACK_BOX_DEF_H
#define	CAR_BLACK_BOX_DEF_H

//
//unsigned char detect_key_press(unsigned char key);

//function to display data on dashboard in the first place
void display_dashboard(unsigned char event[], unsigned char speed);

//function to log events into eeprom
void log_event(unsigned char event[], unsigned char speed);

//function to clr the clcd display
void clear_display(void);

//function to check the user pass with pass
unsigned char login(unsigned char key, unsigned char reset_flag);

//function to display main menu screen
unsigned char menu_screen(unsigned char key, unsigned char reset_flag);

//##_____________## functions to do tasks based on main_menu ##_________________##
unsigned char view_log(unsigned char key, unsigned char start_flag);
unsigned char clear_log(unsigned char start_flag);
//void download_log(void);
unsigned char download_log(unsigned char start_flag);
unsigned char set_time_log(unsigned char key, unsigned char start_flag);
unsigned char change_passwd(unsigned char key, unsigned char start_flag);
void clear_logs_from_eeprom();


#endif	/* CAR_BLACK_BOX_DEF_H */


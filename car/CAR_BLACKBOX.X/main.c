/******************
 * NAME: M vijay
 * Date: 03-02-25
 * Desrp: Car Black Box project
 * File : main.c
 ******************/


#include"main.h"
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled) 

#define LONG_PRESS_THRESHOLD  50  // value for long press


void init_config(){
    //initialise the i2c
    init_i2c(200000);
    
    //initialise the RTC
    init_ds1307();
    
    //initialise adc
    init_adc();
    
    //initialise dkp
    init_digital_keypad();
    
    //initialse clcd
    init_clcd(); 
    
    //initialise timer2
    init_timer2();
    
    //initialise uart
    init_uart(9600); // COMMON BAUD RATE
    

    
    GIE = 1;    //global interrupt enable
    
    PEIE = 1;   //internal peripheral as interrupt
    
}

void main()
{
    unsigned char screen_mode = DASHBOARD_SCREEN; //control flag
    unsigned char reset_flag;   //flag to check password resets only after 3 attempts
    unsigned char start_flag;   //flag to start any menu operation from initial stage
    unsigned char event[3] = "ON";
    static unsigned char speed ;
    unsigned char key;
    static unsigned char prev_key;
    static unsigned int delay = 0, once = 1;
    unsigned char *gear[] ={"GN", "GR", "G1", "G2", "G3", "G4"};    //arr of ptr for gears 
    unsigned char gr = 0; //index for gear[]
    unsigned char menu_pos;
    static unsigned char flag1 = 1;// inst = 0;
    
    init_config();
    
    //calling function to save event as ON in log
    log_event(event, speed);
    
    //saving password into eeprom at addrs 0x00
    ext_eeprom_24C02_str_write(0x00, "0000");
    
    
    while(1){
        
        speed = read_adc()/10.3; //converting range of 0 - 1023 to 0 - 99
        
        key = detect_key_press();
        
        //debouncing
        __delay_ms(10);
        
        
        if(key == SW1)
        {
            strcpy(event, "Co");        //Co --> collision
            log_event(event, speed);    //log the changed data
        }
        else if(key == SW2 && gr < 6)   //gear should be less than 6
        {
            strcpy(event, gear[gr]);    //increment the gear
            log_event(event, speed);    //log the changed data
            gr++;
        }
        else if(key == SW3 && gr > 0)   //gear should be > 0
        {
            gr--;                       //decrement the gear
            strcpy(event, gear[gr]);
            log_event(event, speed);    //log the changed data
        }
        else if((key == SW4 || key == SW5 ) && screen_mode == DASHBOARD_SCREEN)
        {
                
                //static unsigned char flag = 1;
                flag1 = 1;
                
                //clear the display before entering into login page
                clear_display();
                
                //display msg to enter password]
                clcd_print("Enter Password", LINE1(1));
                
                //move the cursor to 4th position in line2
                clcd_write(LINE2(4), INST_MODE);
                //blink the cursor
                clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                __delay_us(100);
                
                //change flag to login page
                screen_mode = LOGIN_SCREEN;
                
                reset_flag = RESET_PASSWORD;
                
                //turn on the timer2
                TMR2ON = 1;            
        }
        else if(screen_mode == VIEW_LOG_SCREEN)
        {
            switch(key)
            {
                    case LONG_SW4:
                        clear_display();
                        screen_mode = MAIN_MENU_SCREEN;
                        break;
                    case LONG_SW5:
                        clear_display();
                        screen_mode = DASHBOARD_SCREEN;
                        break;
            }
                            
        }
        else if(screen_mode == MAIN_MENU_SCREEN && key == LONG_SW4) //long press SW4
        {
                    
            switch(menu_pos)
            {
                case 0:
                    
                    clear_display();
                    log_event("VL", speed);
                    start_flag = 'A';
                    screen_mode = VIEW_LOG_SCREEN;
                    break;  
                    
                case 1:
                    
                    clear_display();
                    start_flag = 'B';
                    log_event("CL", speed);
                    screen_mode = CLEAR_LOG_SCREEN;
                    break; 
                    
                case 2:
                    
                    clear_display();
                    start_flag = 'D';
                    log_event("DL", speed);
                    screen_mode = DOWNLOAD_LOG_SCREEN;
                    break;
                    
                case 3:
                    
                    clear_display();
                    start_flag = 'S';
                    log_event("ST", speed);
                    screen_mode = SET_TIME_LOG_SCREEN;
                    break;
                    
                case 4:
                    
                    clear_display();
                    start_flag = 'C';
                    log_event("CP", speed);
                    screen_mode = CHANGE_PASSWD_LOG_SCREEN;
                    break;      
                    
            }
        }
        //the display_dashboard should be shown during mode1
        //so we use switch case method to display data on various data
        switch(screen_mode)
        {
            //static unsigned char flag = 1;
            //if mode is dashboard screen
            case DASHBOARD_SCREEN:
                display_dashboard(event , speed);
                break;
            
            case LOGIN_SCREEN:
                //calling funct to check the login password
                switch(login(key, reset_flag))
                {
                    case RETURN_BACK:       //if return back is option go to dashboard_screen
                        screen_mode = DASHBOARD_SCREEN;
                        
                        //clear the display before entering into dashboard page
                        clear_display();

                        //turning off cursor
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;
                        break;
                        
                    case LOGIN_SUCCESS:
                        //clear the display before entering into main menu page
                        clear_display();
                        screen_mode = MAIN_MENU_SCREEN;
                        
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_ms(100);
                        
                        TMR2ON = 0;
                        reset_flag = RESET_MENU;    //changing the reset flag before entering into main menu
                        continue;                   //skip the nxt step of reset_flag = RESET_NONE
                }
                break;
            case MAIN_MENU_SCREEN: 
                
                menu_pos =  menu_screen(key, reset_flag);  //call the main menu screen function
                
                break;
                
            case VIEW_LOG_SCREEN:
                
                clcd_print("#  TIME    E  SP ",LINE1(0));
                
                //if no logs present return to main menu
                if(view_log(key, start_flag) == OP_FAILURE)
                    screen_mode = MAIN_MENU_SCREEN;
                           
                break;
                            
            case CLEAR_LOG_SCREEN:
                
                clear_display();
                if(clear_log(start_flag) == OP_SUCCESS)
                { 
                    __delay_ms(3000);
                    clear_display();
                    screen_mode = MAIN_MENU_SCREEN; 
                }               
                break;
               
            case DOWNLOAD_LOG_SCREEN:    
                
                if(download_log(start_flag) == OP_SUCCESS)
                {
                    __delay_ms(2000);
                    clear_display();
                    screen_mode = DASHBOARD_SCREEN;
                }
                break;
               
            case SET_TIME_LOG_SCREEN:
                
                if (set_time_log(key, start_flag) == OP_SUCCESS)
                {
                    __delay_ms(3000);
                    clear_display();
                    screen_mode = DASHBOARD_SCREEN;
                }
                break;
                
            case CHANGE_PASSWD_LOG_SCREEN:
                
                if (change_passwd(key, start_flag) == OP_SUCCESS)
                {
                    clear_display();
                    TMR2ON = 0;
                    screen_mode = DASHBOARD_SCREEN;
                }
                break;
                           
        }
        reset_flag = RESET_NONE;
    }
    
}

/*
 * File:   car_black_box.c
 * function definitions
 */

#include "main.h"

char time[7];   //HHMMSS
char clock_reg[3];
char log[11];
char log_pos = 0, sec;
unsigned char return_time, latest_log = 0;
static unsigned char once = 1;
int event_count = -1;


char *menu[] = {"view log", "clear log", "download log", "set time", "change passwd"};
char menu_pos;  //index to track the arr menu

//flag to allow the menu log operations 
//(flag == 1 -> allow or dont allow any operations)
static unsigned char log_flag = 1; 


// display_dashboard mode functions 
// function to store time from RTC to str time[7]
static void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

// function to display time
static void display_time()
{
    get_time(); //calling funct to print time on CLCD
    clcd_putch(time[0], LINE2(2));
    clcd_putch(time[1], LINE2(3));
    clcd_putch(':', LINE2(4));
    clcd_putch(time[2], LINE2(5));
    clcd_putch(time[3], LINE2(6));
    clcd_putch(':', LINE2(7));
    clcd_putch(time[4], LINE2(8));
    clcd_putch(time[5], LINE2(9));
    
}

void display_dashboard(unsigned char event[], unsigned char speed)
{
    //display title msg
    clcd_print(" TIME     E  SP", LINE1(1));
    
    //display time
    display_time();
    
    //display event
    clcd_print(event, LINE2(11));
    
    //display speed
    clcd_putch((speed / 10) + '0', LINE2(14));   //print 10s digit of speed(range 0-99)
    clcd_putch((speed % 10) + '0', LINE2(15));   //print 1s digit of speed
    
}

//**************** funct call to store data into eeprom  *****************
void store_event()
{
    char addrs = 0x05;
    //if all 10 bytes are filled with events store nxt event at start
    if(log_pos == 10)   
        log_pos = 0;
    
    //increment addrs to 10, after 10 bytes of data used
    addrs = 0x05 + log_pos*10;
    
    //write logged data to eeprom
    ext_eeprom_24C02_str_write(addrs, log);
    log_pos++;
    latest_log++;
    
    if(event_count < 10)  //increment the count event
        event_count++;
    
    
}

//******************** funct call to log the events ************************
void log_event(unsigned char event[], unsigned char speed)
{
    get_time(); //get current time before storing time into logs

    //log time 
    //time is of  6 bytes of data 
    strncpy(log, time, 6);
    
    //log event
    //event is of 2bytes of data
    strncpy(&log[6], event, 2);
    
    //log the speed
    //log is of 2bytes
    log[8] = (speed/10)+'0';    //get 10's place
    log[9] = (speed%10)+'0';    //get 1s place
    log[10]='\0';
    
    store_event();  //call function to store data into eeprom
    
}


//************************ functions for login screen ********************************

unsigned char login(unsigned char key, unsigned char reset_flag)
{
    static char user_pass[5];
    static unsigned char i;            //index
    static unsigned char attempts_left = 3;
    
    if(reset_flag == RESET_PASSWORD)
    {
        i = 0;
        attempts_left = 3;
        
        user_pass[0] = '\0';
        user_pass[1] = '\0';
        user_pass[2] = '\0';
        user_pass[3] = '\0';
        key = ALL_RELEASED;
        return_time = 5;    //assigning or resetting value for every event
    }
    
    //after 5 sec change the screen
    if(return_time == 0)
    {
        return RETURN_BACK;     //if no response is given for 5sec return to dashboard
    }
    
    //logic to store user typed pass based on 2 switches
    //SW4 0 SW5 1
    if(key == SW4 && i < 4)
    {
        clcd_putch('*', LINE2(4+i));    //print * at place of typed password
        user_pass[i] = '1';
        i++;
        return_time = 5;    //assigning or resetting value for every event
    }
    else if(key == SW5 && i < 4)
    {
        clcd_putch('*', LINE2(4+i));    //print * at place of typed password
        user_pass[i] = '0';
        i++;
        return_time = 5;    //assigning or resetting value for every event 
    }
    
    if(i == 4)
    {
        char save_pass[4]; //str to save pass from eeprom
        
        //read saved pass from eeprom 
        for(int k = 0; k < 4; k++)
        {
            save_pass[k] = ext_eeprom_24C02_read(k);
        }
        
        //compare user_pass only 4 bytes from save_pass
        if(!strncmp(user_pass, save_pass,4))  
        {
            //clr screen
            clear_display();
            //blink the cursor OFF
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("Login Success", LINE1(1));
            __delay_ms(3000);
            return LOGIN_SUCCESS;   //if login success go to main menu list
        }
        else                        //if password is not matching
        {
            attempts_left--;        //decrement attempts
            
            if(attempts_left == 0)  //if attempts are over
            {
                clear_display();    //clr screen
                //cursor OFF
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                //msg and timer to block for 60 sec
                clcd_print("YOU ARE BLOCKED", LINE1(0));
                clcd_print("wait for", LINE2(0));
                clcd_print("sec", LINE2(13));
                sec = 60;
                while(sec)
                {
                    clcd_putch((sec / 10) + '0', LINE2(10));
                    clcd_putch((sec % 10) + '0', LINE2(11));
                }
                //block screen with timer
                __delay_ms(3000);
                attempts_left = 3;
            }
            else
            {
                clear_display();
                clcd_print("Wrong Password", LINE1(0));
                clcd_print("Attempts left", LINE2(0));
                clcd_putch(attempts_left + '0', LINE2(14));
                __delay_ms(3000);
            }
            clear_display();
            //display msg to enter password]
                clcd_print("Enter Password", LINE1(1));
                
                //move the cursor to 4th position in line2
                clcd_write(LINE2(4), INST_MODE);
                //blink the cursor
                clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                __delay_ms(100);
                i = 0;
                return_time = 5;    //assigning or resetting value for every event
            
        }
    }
    return OP_SUCCESS;
}


//****************** function call to display main_menu ***************************
unsigned char menu_screen(unsigned char key, unsigned char reset_flag)
{
    if(reset_flag == RESET_MENU)
    {
        return_time = 5;
        menu_pos = 0;
    }
    if(return_time == 0)
        return OP_FAILURE;
    
    if(key == SW4 && menu_pos > 0)
    {
        //decrement menu pos
        clear_display();
        menu_pos--;
    }
    else if(key == SW5 && menu_pos < 4)
    {
        //inecrement menu pos
        clear_display();
        menu_pos++;
    }
    
    //logic for, if menu pos reaches last log, move '*' to last 
    if(menu_pos == 4)
    {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos - 1], LINE1(2));
        clcd_print(menu[menu_pos], LINE2(2));
    }
    else
    {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos + 1], LINE2(2));
    }
    
    return menu_pos;
}

//************************ function to clear the clcd display ************************
void clear_display()
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(100);
}

//**************  functions for menu list operations  ********


unsigned char view_log(unsigned char key, unsigned char start_flag)
{
    unsigned char logs[11];  // Array to store log info
    static unsigned char pos, logging;
    unsigned char addrs = 0x05;  // Start EEPROM address
    
    if (!log_flag)  // If no data is available
    {
        clear_display();
        clcd_print("NO LOGS PRESENT", LINE2(0));
        __delay_ms(2000);
        clear_display();
        return OP_FAILURE;  // Return to main menu
    }
    
    if (start_flag == 'A' && once)
    {
        key = 0;
        once = 0;
        logging = latest_log;
        pos = 0;
    }
    
    // Handle scrolling through logs
    if (key == SW4 && pos > 0)
    {       
        pos--;
        clear_display();
    }
    else if (key == SW5 && pos < event_count - 1 )
    {
        pos++;
        clear_display();
    }

    // Set address for EEPROM reading based on circular buffer
    if (latest_log < 10)
    {
        addrs = 0x05 + pos * 10;  // Start from first log
    }
    else
    {
        addrs = 0x05 + ((latest_log + pos) % 10) * 10;  // Circular indexing
    }
    

    // Read logs from EEPROM
    for (int i = 0; i < 10; i++)
    {
        logs[i] = ext_eeprom_24C02_read(addrs + i);
    }
    logs[10] = '\0';
    

    // Display event number (1 - 10)
    //logic to view event num
    if((pos+1) < 10)
        clcd_putch((pos+1)+'0', LINE2(0));
    else
    {
        clcd_putch('1', LINE2(0));
        clcd_putch('0', LINE2(1));
    }
    //view the logs data from eeprom
        clcd_putch(logs[0], LINE2(2));
        clcd_putch(logs[1], LINE2(3));
        clcd_putch(':', LINE2(4));
        clcd_putch(logs[2], LINE2(5));
        clcd_putch(logs[3], LINE2(6));
        clcd_putch(':', LINE2(7));
        clcd_putch(logs[4], LINE2(8));
        clcd_putch(logs[5], LINE2(9));
        //shows event log
        clcd_putch(logs[6], LINE2(11));
        clcd_putch(logs[7], LINE2(12));
        //shows speed log
        clcd_putch(logs[8], LINE2(14));
        clcd_putch(logs[9], LINE2(15));
       

    return OP_SUCCESS;
}


unsigned char clear_log(unsigned char start_flag)
{    
    
    //clear the log 
    //if flag = 1 --> logs available (or) flag = 0 --> not available
    if(start_flag == 'B' && log_flag) 
    {
        log_flag = 0 ;  //set down log operation flag 
        event_count = -1;
        //clear_display();
        clcd_print("LOGS  ARE",LINE1(4));
        clcd_print("CLEARED",LINE2(5));
    }
    else if(!log_flag)
    {
        clcd_print("NO LOGS ", LINE1(4));
        clcd_print("PRESENT", LINE2(4));
    }

    return OP_SUCCESS;    //return success flag
   
}
 

unsigned char download_log(unsigned char start_flag)
{       
    char buffer[11];
    //var to store the no of events logged
    int event = 0;
    unsigned char addrs = 0x05;
    //char pos = 0;    //var to track position of reading log
    
    if(!log_flag)      //if no data is present in eeprom to download
    {
        clcd_print("FOUND NO LOGS",LINE2(1));
        puts("FOUND NO LOGS");
    }
    
    //view the log events on uart
    else if(start_flag == 'D' && log_flag)
    {
        event = 0;
        //pos = 0;
        clcd_print("OPEN YOUR",LINE1(3));
        clcd_print("CUTECOM/TERATERM",LINE2(0));
        puts("LOGGED DATA");
        putchar('\n');putchar('\r');
        puts("No    TIME   EVENT  SPEED");
        putchar('\n');putchar('\r');
        
      while(event < event_count)  //download upto 10 events from eeprom
      {
          __delay_ms(500);

        addrs = 0x05 + event*10;
        
        //clear_display();
        for(int i=0; i<10; i++)
        {
            buffer[i] = ext_eeprom_24C02_read(addrs+i);
        }
        buffer[10] = '\0';

        putchar(event+'1');
        puts("   ");
        // showing time
        putchar(buffer[0]);
        putchar(buffer[1]);
        putchar(':');
        putchar(buffer[2]);
        putchar(buffer[3]);
        putchar(':');
        putchar(buffer[4]);
        putchar(buffer[5]);
        puts("  ");
        //showing event
        putchar(buffer[6]);
        putchar(buffer[7]);
        puts("    ");
        //showing speed
        putchar(buffer[8]);
        putchar(buffer[9]);
        putchar('\n');
        putchar('\r');
        event++;
      }
    }
    return OP_SUCCESS;
    
}

unsigned char set_time_log(unsigned char key, unsigned char start_flag)
{
    unsigned char positions[] = {3, 6, 9};
    static unsigned char index = 0;
    static unsigned char cur_pos;                       //cursor pos
    static unsigned int new_time[3];                    //arr to store new updated time
    static unsigned char blink = 1, once = 1;           // to blink the field
    unsigned char buffer;
    static unsigned char timer = 0;                     //var to update blink time 
    

    if(start_flag == 'S' && once)
    { 
        get_time();

        //set the timer on timer sensor
        //extracting HH : MM : SS nibble by nibble using bit operation
        new_time[0] = ((time[0] & 0x0F)*10 + (time[1] & 0x0F));
        new_time[1] = ((time[2] & 0x0F)*10 + (time[3] & 0x0F));
        new_time[2] = ((time[4] & 0x0F)*10 + (time[5] & 0x0F));
        
        //starting pos is HH field by default
        cur_pos = 3;    
        once = 0;
        key = 0;index = 0;
    
    }

    switch(key)
    {
        case SW4:                                       //if SW4 , increment the time val
            new_time[index]++;    
            break;
        case SW5:                                      //if short SW5, move cursor position
            index = (index + 1) % 3;
            cur_pos = positions[index] ;
            break;
            
        case LONG_SW5:                                  //save the time to ds1307
            
            get_time();
            
            //fetch and store HH value into buffer 
            buffer = ((new_time[0] / 10) << 4) | new_time[0]%10;
            
            //storing buff val into clock reg
            clock_reg[0] = (clock_reg[0] & 0xC0) | buffer;
            
            //writing new value to ds1307
            write_ds1307(HOUR_ADDR, clock_reg[0]);
            
            //fetch and store MM value into buffer 
            buffer = ((new_time[1] / 10) << 4) | new_time[1]%10;
            
            //storing buff val into clock reg
            clock_reg[1] = (clock_reg[1] & 0x80) | buffer;
            
            //writing new value to ds1307
            write_ds1307(MIN_ADDR, clock_reg[1]);
            
            //fetch and store SS value into buffer 
            buffer = ((new_time[2] / 10) << 4) | new_time[2]%10;
            
            //storing buff val into clock reg
            clock_reg[2] = (clock_reg[2] & 0x80) | buffer;
            
            //writing new value to ds1307
            write_ds1307(SEC_ADDR, clock_reg[2]);

            clear_display();
            
            clcd_print("Time changed", LINE1(2));
            clcd_print("and Saved", LINE2(3));
            
            __delay_ms(1000);
            return OP_SUCCESS;
            
    }
    
    //blinking 
    if(timer++ == 2)
    {
        timer=0;
        
        blink = !blink;
        
        if(blink)
        {
            if(cur_pos == positions[0])
            {
                clcd_print("  ",LINE2(3));__delay_ms(150);   
            }
            else if(cur_pos == positions[1])
            {
                clcd_print("  ", LINE2(6));__delay_ms(150);
            }
            else if(cur_pos == positions[2])
            {
                clcd_print("  ", LINE2(9));__delay_ms(150);
            }
        }
    }
    //logic to rollover the time val
    if(new_time[0] > 23)    //HH field reaches 23 set to 0
        new_time[0]= 0;
    if(new_time[1] > 59)    //MM field reaches 59 set to 0
        new_time[1] = 0;
    if(new_time[2] > 59)    //SS field reaches 59 set to 0
        new_time[2] = 0;
    
    clcd_print("HH MM SS", LINE1(3));

    
    clcd_putch(new_time[0]/10 + '0', LINE2(3));
    clcd_putch(new_time[0]%10 + '0', LINE2(4));
    clcd_putch(':', LINE2(5));
    clcd_putch(new_time[1]/10 + '0', LINE2(6));
    clcd_putch(new_time[1]%10 + '0', LINE2(7));
    clcd_putch(':', LINE2(8));
    clcd_putch(new_time[2]/10 + '0', LINE2(9));
    clcd_putch(new_time[2]%10 + '0', LINE2(10));
    
    
    return OP_FAILURE;
}

unsigned char change_passwd(unsigned char key, unsigned char start_flag)
{
    static char new_pass[5];
    static char re_new_pass[5];
    static char index, re_enter, once = 1, blink_flag = 0, delay = 0, toggle = 0; 
    char save_pass[4], compare_flag = 0;  //compare_flag
    
    if (start_flag == 'C' && once)
    {
        index = 0;
        once = 0;
        return_time = 5;
        re_enter = 1;
        compare_flag = 0;
        TMR2ON = 1;
        key = 0;

        for (int i = 0; i < 4; i++)
        {
            new_pass[i] = '\0';
            re_new_pass[i] = '\0';
        }
    }

    if (!return_time)  // Timeout
        return OP_SUCCESS;

    if(delay++ == 5)
    {
        toggle = !toggle;
        delay = 0;
    }
    
    if(index < 4)
    {
        if(toggle)  //toggle = 1
            clcd_putch('_',LINE2(4+index));
        else if(!toggle)
            clcd_putch(' ',LINE2(4+index));
    }
    
    if (re_enter)
    {
        clcd_print("ENTER NEW PASSWD", LINE1(0));

        if (key == SW4 && index < 4)     
        {
            clcd_putch('*', LINE2(4 + index));
            new_pass[index] = '1';
            index++;
            return_time = 5;
        }
        else if (key == SW5 && index < 4)
        {
            clcd_putch('*', LINE2(4 + index));
            new_pass[index] = '0';
            index++;
            return_time = 5;
        }

        //toggling cursor
        if(toggle)  //toggle = 1
            clcd_putch('_',LINE2(4+index));
        else if(!toggle)
            clcd_putch(' ',LINE2(4+index));
        
        if (index == 4)
        {
            new_pass[4] = '\0';
            re_enter = 0; // Switch to re-enter mode
            index = 0;    // Reset index for next entry
            clear_display();
        }
        // Cursor Blinking Effect
    }
    else 
    {
        //clear_display();
        clcd_print("RE-ENTER PASSWD", LINE1(0));

        if (key == SW4 && index < 4)     
        {
            clcd_putch('*', LINE2(4 + index));
            re_new_pass[index] = '1';
            index++;
            return_time = 5;
        }
        else if (key == SW5 && index < 4)
        {
            clcd_putch('*', LINE2(4 + index));
            re_new_pass[index] = '0';
            index++;
            return_time = 5;
        }
        
        //toggling cursor
        if(toggle)  //toggle = 1
            clcd_putch('_',LINE2(4+index));
        else if(!toggle)
            clcd_putch(' ',LINE2(4+index));

        if (index == 4)
        {
            re_new_pass[4] = '\0';
            compare_flag = 1;  // Set flag to compare
            clear_display();
        }
    }

    if (compare_flag)
    {
        if (!strcmp(new_pass, re_new_pass))
        {
            
            clcd_print("PASSWORD CHANGED", LINE2(0));
            ext_eeprom_24C02_str_write(0x00, new_pass);
        }
        else 
        {
            clcd_print("FAILURE PASSWORD", LINE1(0));
            clcd_print("NOT MATCHING", LINE2(2));
        }

        __delay_ms(2000);
        // Reseting  after operation
        once = 1;
        re_enter = 1;
        index = 0;
        return OP_SUCCESS;
    }

}

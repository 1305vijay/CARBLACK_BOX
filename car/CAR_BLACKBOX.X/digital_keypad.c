/*
 * File:   digital_keypad.c
 * Author: maluv
 *
 * Created on 6 January, 2025, 3:34 PM
 */


#include <xc.h>
#include "digital_keypad.h"
#include "main.h"

char flag;

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

/*
unsigned char detect_key_press()
{
    static char once;
    static int long_pressed;
    static unsigned char pre_key;
    unsigned char key = KEYPAD_PORT & INPUT_LINES;


    if (key != ALL_RELEASED && once == 0) 
    {
        once = 1; 
        long_pressed = 0; 
        pre_key = key; 
    }
    else if (key == ALL_RELEASED && once == 1) 
    {
        once = 0; 

        if (long_pressed < 15) 
        {
            return pre_key; 
        }
    }

   

    if (once == 1 && long_pressed < 16)
    {
        long_pressed++;
    }

    else if (once == 1 && long_pressed == 16 && key == SW4)
    {
        long_pressed++;
        return LONG_SW4;
    }

    else if (once == 1 && long_pressed == 16 && key == SW5)
    {
        long_pressed++;
        return LONG_SW5;
    }

    return ALL_RELEASED;
}
*/
//cor

unsigned char detect_key_press()
{
    static unsigned char once = 1, key_pressed = 0, timer = 0, prev_key = ALL_RELEASED, delay = 0;
    
    unsigned char key = KEYPAD_PORT & INPUT_LINES;  //On key press, each value will be given (SW1 SW2 SW3 SW4)
    
    if(key != ALL_RELEASED && once)   //if any key is pressed for 1st time
    {
        key_pressed = 1;                //raise the flag
        timer = 0;                      //set the timer 
        once = 0;                       //reset once flag
        prev_key = key;                 //storing key press into prev_key
            
    }
    else if(key == ALL_RELEASED && !once)
    {
        once = 1;
        key_pressed = 0;
        
        //when key released, and timer is <20 
        //return short press
        if(timer < 15)
            return prev_key;
    }
    
    if(timer < 20 && !once)
    {
        __delay_ms(10);
        timer++;
        
    }
    
    /*
    if(!once && timer == 15 && key == SW4)
        return LONG_SW5;
    else if(!once && timer == 15 && key == SW5)
        return LONG_SW5;
     * */
    // Detect long press
    if (!once && timer == 15)
    {
        if (key == SW4)
            return LONG_SW4; 
        else if (key == SW5)
            return LONG_SW5; 
    }
    
    return ALL_RELEASED;
}


/*
unsigned char detect_key_press()
{
    static unsigned char once = 1, key_pressed = 0, timer = 0, prev_key = ALL_RELEASED;
 
    unsigned char key = KEYPAD_PORT & INPUT_LINES;  //On key press, each value will be given (SW1 SW2 SW3 SW4)
    
    
    if(key != ALL_RELEASED && once)   //if any key is pressed for 1st time
    {
        key_pressed = 1;                //raise the flag
        timer = 0;                      //set the timer 
        once = 0;                       //reset once flag
        prev_key = key;                 //storing key press into prev_key
            
    }
    else if(key == ALL_RELEASED && !once)
    {
        once = 1;
        key_pressed = 0;
        
        //when key released, and timer is <20 
        //return short press
        if(timer < 20)
            return prev_key;
    }
    
    //incrementing timer when once flag set to zero
    if(timer < 20 && !once)
    {
        timer++;
    }
    
    //if timer and once are valid
    else if(!once && timer == 20 && key == SW4)//key != ALL_RELEASED))
    {
        
        //if SW4 is pressed morethan 20ms
        if(prev_key == SW4)
        {
            return LONG_SW4;
        }
            
        //if SW5 is pressed morethan 20ms
        if(prev_key == SW5)
        {
            return LONG_SW5;
        }
    }
    else if(!once && timer == 20 && key == SW5)
        return LONG_SW5;
    
    return ALL_RELEASED;
    
}*/

/*
unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    
    if (mode == LEVEL_DETECTION)
    {
        return KEYPAD_PORT & INPUT_LINES;
    }
    else
    {
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once)
        {
            once = 0;
            
            return KEYPAD_PORT & INPUT_LINES;
        }
        else if ((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED)
        {
            once = 1;
        }
    }
    
    return ALL_RELEASED;
}
*/
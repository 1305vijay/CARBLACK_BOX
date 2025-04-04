#include <xc.h>
#include "main.h"

extern char sec;
extern unsigned char return_time;

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
            if(sec >= 0 ) 
            {
                sec--;
            }
            if(return_time > 0)
                return_time--;
            
            count = 0;       
        }
        
        TMR2IF = 0;
    } 
}
/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    /* insert your hardware initialization here */
		DDRB = (1 << DDB0);
    for(;;){
        /* insert your main loop code here */
			char i;
			for(i = 0; i < 10; i++)
			{
				_delay_ms(30);
			}
			PORTB ^= (1 << PORTB0);
    }
    return 0;   /* never reached */
}

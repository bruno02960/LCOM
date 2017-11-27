#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"

unsigned int counter;
unsigned int hook_id=0x00;


int timer_subscribe_int(void ) {

	int ret = hook_id;

if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id)==OK)
		if(sys_irqenable(&hook_id)==OK)
			return ret;

	return -1;
}

int timer_unsubscribe_int() {

	if(sys_irqdisable(&hook_id)==OK)
		if(sys_irqrmpolicy(&hook_id)==OK)
		return 0;

	return 1;
}



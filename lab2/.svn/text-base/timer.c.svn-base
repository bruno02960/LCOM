#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"

unsigned int counter;
unsigned int hook_id=0x00;

int timer_set_square(unsigned long timer, unsigned long freq) {

	unsigned char config;

	timer_get_conf(timer,&config);
	config=config & 0x0F; //Preserves LSB

	unsigned char test=(TIMER_SEL0|TIMER_LSB_MSB|TIMER_SQR_WAVE);

	switch(timer)
	{case 0:
		sys_outb(TIMER_CTRL, (test|config));
		sys_outb(TIMER_0, (TIMER_FREQ/freq));
		sys_outb(TIMER_0, (TIMER_FREQ/freq)>>8);
		break;
	case 1:
		sys_outb(TIMER_CTRL, (test|config));
		sys_outb(TIMER_1, (TIMER_FREQ/freq));
		sys_outb(TIMER_1, (TIMER_FREQ/freq)>>8);
		break;
	case 2:
		sys_outb(TIMER_CTRL, (test|config));
		sys_outb(TIMER_2, (TIMER_FREQ/freq));
		sys_outb(TIMER_2, (TIMER_FREQ/freq)>>8);
		break;
	default:
		return 1;}


	return 0;
}

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

void timer_int_handler() {

	counter++;

}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	
	unsigned long var;


	switch(timer)
	{
	case 0://for timer 0
		sys_outb(TIMER_CTRL,(TIMER_RB_COUNT_|TIMER_RB_CMD|TIMER_RB_SEL(0)));
		sys_inb(TIMER_0,&var);
		*st=var;
		break;
	case 1://for timer 1
		sys_outb(TIMER_CTRL,(TIMER_RB_COUNT_|TIMER_RB_CMD|TIMER_RB_SEL(1)));
		sys_inb(TIMER_1,&var);
		*st=var;
		break;
	case 2://for timer 2
		sys_outb(TIMER_CTRL,(TIMER_RB_COUNT_|TIMER_RB_CMD|TIMER_RB_SEL(2)));
		sys_inb(TIMER_2,&var);
		*st=var;
		break;
	default:
		return 1;
	}

	return 0;
}

int timer_display_conf(unsigned char conf) {
	
	unsigned int num;

	//Counter selection
	num=(((BIT(7)|BIT(6))&conf)>>7);
	printf("Function   Value\n");
	printf("----------------\n");
	printf("%7s%5d\n", "Counter", num);

	//Type of access
	num=(((BIT(5)|BIT(4))&conf)>>5);
	switch(num)
	{case 1:
		printf("%6s%8s\n", "Access", "LSB");
		break;
	 case 2:
		printf("%6s%8s\n", "Access", "MSB");
		break;
	 case 3:
		printf("%6s%12s\n", "Access", "LSB/MSB");
		break;
	 default:
		{
			printf("Error\n");
			return 1;
		}
	}

	//Operating mode
	num=(((BIT(2)|BIT(1))&conf)>>2);
	if (num<=3)
		switch(num)
			{case 0:
			case 1:
			case 2:
			case 3:
			printf("%5s%5d\n", "Op Mode", num);
			break;
			}
	else
	{num=(((BIT(3)|BIT(2)|BIT(1))&conf)>>3);
	switch(num)
	{case 4:
	case 5:
		printf("%5s%5d\n", "Op Mode", num);
		break;
	default:
		{
		printf("Error\n");
		return 1;
		}
	}}

	//BCD/Binary
	num=(BIT(0)&conf);
	if (num)
		printf("%5s%7s\n", "Ct Mode", "Bin");
	else
		printf("%5s%7s\n", "Ct Mode", "BCD");

	return 0;
}

int timer_test_square(unsigned long freq) {
	
	if(timer_set_square(0, freq)==0)
		return 0;

	return 1;
}

int timer_test_int(unsigned long time) {
	
	int ipc_status, ans;
	message msg;
	unsigned int time_counter=0;

	timer_set_square(0,60);

	ans=timer_subscribe_int();

	if (ans==-1)
	{
		printf("Subscription failed!\n");
		return 1;
	}

	unsigned long irq_set=BIT(ans);

	while(counter<time){
		if (driver_receive(ANY,&msg,&ipc_status)!=0)
		{
			printf("driver_receive failed with: %d\n",driver_receive(ANY,&msg,&ipc_status));
			continue;
		}
		if(is_ipc_notify(ipc_status))
		{
			switch(_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
				{
					time_counter++;
					if ((time_counter/60)==1)
					{
						printf("Notification no. %u\n", counter+1);
						time_counter=0;
						timer_int_handler();
					}
				}
				break;
			default:
				break;

			}
		}
	}

	timer_unsubscribe_int();

	return 0;
}

int timer_test_config(unsigned long timer) {

	unsigned char st;

	if (timer_get_conf(timer,&st))
		return 1;
	else
		timer_display_conf(st);

	return 0;
}

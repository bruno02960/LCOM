#include<minix/syslib.h>
#include<minix/drivers.h>
#include<minix/com.h>

#include "keyboard.h"
#include "i8042.h"

int hook_id_kbd=0x01;

int kbd_subscribe(void){
	int ret=hook_id_kbd;

	if(sys_irqsetpolicy(1,IRQ_REENABLE|IRQ_EXCLUSIVE,&hook_id_kbd)==0) //View this later
		if (sys_irqenable(&hook_id_kbd)==0)
			return ret;
	return -1;
}

int check_ibf()
{
	unsigned long status = 0;
	sys_inb(OUT_BUF,&status);
	if(status&BIT(1))
		return -1;

	tickdelay(micros_to_ticks(DELAY_US));

	return 0;
}

int kbd_unsubscribe(void){
	if(sys_irqdisable(&hook_id_kbd)==0)
		if(sys_irqrmpolicy(&hook_id_kbd)==0)
			return 0;

	return -1;
}

unsigned long kbd_handler()
{
	unsigned long scan=0,code=0;
	unsigned long status;
	extern long codigoASM;

	sys_inb(OUT_BUF, &status);
	if(status&BIT(0))
		sys_inb(IN_BUF,&scan);
	if(scan==SPECIAL)
	{
		scan=scan<<8;
		scan=scan|code;
	}
	if(scan&BIT(7))
		printf("Breakcode: 0x%x\n",scan);
	else
		printf("Makecode: 0x%x\n", scan);

	tickdelay(micros_to_ticks(DELAY_US));

	return scan;
}

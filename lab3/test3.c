#include<minix/syslib.h>
#include<minix/drivers.h>
#include<minix/com.h>
#include"i8042.h"
#include"i8254.h"
#include"timer.h"

#define BIT(n) (0x01<<(n))
#define DELAY_US 20000

int hook_id_kbd=0x01;

//Falta ver
int switch_leds(unsigned short led, unsigned char onOff)
{
	unsigned long ans=kbd_subscribe();

	int n = 0;
	int ipc_status;
	message msg;

	unsigned long irq_set=BIT(ans);

	printf("a ");										//10 tentativas

	if (driver_receive(ANY,&msg,&ipc_status)!=0)
	{
		printf("driver_receive failed with: %d\n",driver_receive(ANY,&msg,&ipc_status));
		return 0;
	}

	if(is_ipc_notify(ipc_status))
	{
		switch(_ENDPOINT_P(msg.m_source))
		{
		case HARDWARE:
			if (msg.NOTIFY_ARG/* & irq_set*/)
			{
				if (!check_ibf())
				{


					sys_outb(IN_BUF,LED_CMD);			 		//Manda comando LED
					tickdelay(micros_to_ticks(DELAY_US));	//Espera
					sys_inb(IN_BUF,&ans);



					if(ans==CMD_ACK)							//Verifica ACK
					{sys_outb(IN_BUF,led);}
					if(ans==CMD_ERR)							//Se erro
						return 1;
					while(ans==CMD_RES && ans!=CMD_ACK)
					{
						sys_outb(IN_BUF,led);
					}

					if(check_ibf())							//Verifica se a input buffer esta cheia
						return 1;
					else
					{
						sys_inb(IN_BUF,&ans);				//Verifica ACK
						tickdelay(micros_to_ticks(DELAY_US));
					}

					if(ans==CMD_ACK)							//Se ACK
					{
						onOff=onOff^led;
						return 0;
					}
				}

		else
			return 1;}
		break;
		default:
			break;

		}}

	kbd_unsubscribe();

	return 1;
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

unsigned long kbd_handler(int ass)
{
	unsigned long scan=0,code=0;
	unsigned long status;
	extern long codigoASM;

	sys_inb(OUT_BUF, &status);
	if(status&BIT(0))
		if(ass==0)
			sys_inb(IN_BUF,&scan);
		else if(ass==1)
		{
			retornoASM();
			scan=codigoASM;
		}
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

int kbd_subscribe(void){
	int ret=hook_id_kbd;

	if(sys_irqsetpolicy(1,IRQ_REENABLE|IRQ_EXCLUSIVE,&hook_id_kbd)==0) //View this later
		if (sys_irqenable(&hook_id_kbd)==0)
			return ret;
	return -1;
}


int kbd_test_scan(unsigned short ass) {
	unsigned long code;
	int ipc_status, ans=kbd_subscribe();
	message msg;
	extern long codigoASM;

	if(ans==-1)
	{
		printf("Failed to subscribe keyboard!\n");
		return 1;
	}

	unsigned long irq_set=BIT(ans);

	while(code!=ESC_COD){											//Add macro
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
					code=kbd_handler(ass);
				}
				break;
			default:
				break;
			}
		}
	}

	ans=kbd_unsubscribe();
	if(ans==-1)
	{
		printf("Failed to unsubscribe keyboard!\n");
		return 1;
	}

	tickdelay(micros_to_ticks(DELAY_US));

	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	unsigned int time_counter=0;

	int ans = timer_subscribe_int();

	message msg;

	int ipc_status;

	unsigned long irq_set_timer=BIT(ans);

	if(ans==-1)
	{
		printf("Failed to subscribe timer!\n");
		return 1;
	}

	unsigned char onOff=0;

	int i;
	for (i = 0; i < n; i++) {
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
			{
				if (msg.NOTIFY_ARG & irq_set_timer)
				{
					time_counter++;
				}
				if ((time_counter%60)==0)
				{
					if (switch_leds(leds[i],onOff))
						return 1;
					else
						time_counter=0;
				}
				else
				{
					i--;
				}
			}
			break;
			default:
				break;

			}
		}
	}

	ans = timer_unsubscribe_int();

	if(ans==-1)
	{
		printf("Failed to unsubscribe timer!\n");
		return 1;
	}

	return 0;


}
int kbd_test_timed_scan(unsigned short n) {
	unsigned long code;
	int ipc_status, ans=kbd_subscribe();
	message msg;

	unsigned int counter;

	unsigned int time_counter;

	if(ans==-1)
	{
		printf("Failed to subscribe keyboard!\n");
		return 1;
	}

	unsigned long kbd_set=BIT(ans);

	ans=timer_subscribe_int();

	if(ans==-1)
		{
			printf("Failed to subscribe timer!\n");
			return 1;
		}

	unsigned long timer_set=BIT(ans);

	while(code!=ESC_COD && counter<n*60){											//Add macro
		if (driver_receive(ANY,&msg,&ipc_status)!=0)
		{
			printf("driver_receive failed with: %d\n",driver_receive(ANY,&msg,&ipc_status));
			continue;
		}
		if(is_ipc_notify(ipc_status))
		{
			if(msg.NOTIFY_ARG & timer_set)
			{
				counter++;
			}
			switch(_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & kbd_set)
				{	code=kbd_handler(0);
					counter=0;}
			break;
			default:
				break;
		}
	}
}

ans=kbd_unsubscribe();
	if(ans==-1)
	{
		printf("Failed to unsubscribe keyboard!\n");
		return 1;
	}

	tickdelay(micros_to_ticks(DELAY_US));

	return 0;
}

#include<minix/syslib.h>
#include<minix/drivers.h>
#include<minix/com.h>
#include"mouse.h"
#include"i8042.h"
#include"i8254.h"
#include "timer.h"

int hook_id_mouse=0x01;

int read_status()
{
	char packet[3]={0,0,0};
	int b;
	unsigned long res;

	for (b = 0; b < 3; b++)
	{
		sys_outb(IN_BUF,GET_STATUS);
		sys_inb(IN_BUF,&res);
		tickdelay(micros_to_ticks(DELAY_US));
		packet[b] = res;
	}

	//byte 1

	printf("%d\n",packet[1]);

	//Remote or Stream mode

	if ((packet[0]&BIT(6)) != 0)
		printf("Remote (polled) mode\n");
	else
		printf("Stream mode\n");

	//Data reporting
	if ((packet[0]&BIT(6)||packet[0]&BIT(5)) !=0 && (packet[0]&BIT(6)) == 0)
		printf("Data Reporting: Enabled\n");
	else
		printf("Data Reporting: Disabled\n");

	//Scaling
	if ((packet[0]&BIT(4)) != 0)
		printf("Scaling is 2:1\n");
	else
		printf("Scaling is 1:1\n");



	//Left Button
	if ((packet[0]&BIT(0)) != 0)
		printf("Left Button is pressed\n");
	else
		printf("Left Button is released\n");

	//Middle Button
	if ((packet[0]&BIT(2)) != 0)
		printf("Middle Button is pressed\n");
	else
		printf("Middle Button is released\n");

	//Right Button
	if ((packet[0]&BIT(1)) != 0)
		printf("Right Button is pressed\n");
	else
		printf("Right Button is released\n");


	//byte 2

	if (packet[1]&BIT(7) != 0  || packet[1]&BIT(6) != 0||
			packet[1]&BIT(5) != 0|| packet[1]&BIT(4) != 0 ||
			packet[1]&BIT(3) != 0 || packet[1]&BIT(2) != 0)
		printf("Corrupted Byte!\n");

	else
		printf("Resolution: %d count per mm\n", packet[1]);
	//byte 3

	if (packet[2] >= 10 && packet[2] <=200)
		printf("Sample Rate: %lu\n", packet[2]);
	else
		printf("Sample rate value error\n");

	return 0;
}

int mouse_command(unsigned long cmd)
{
	unsigned long ans;

	sys_outb(CTRL_REG ,DR_ENA);

	do{
		sys_outb(CTRL_REG, 0xD4);
		sys_outb(IN_BUF,cmd);
		sys_inb(IN_BUF,&ans);
		tickdelay(micros_to_ticks(DELAY_US));
	}while(ans!=ACK);

	return 0;
}

char twos_complement(char packetN)
{
	return -(~packetN+1);
}

int print_packet(char packet[3])
{
	if(!packet[0]&BIT(3))
		return 1;

	if((packet[0]&BIT(4))==BIT(4))
	{packet[1]=twos_complement(packet[1]);
	}

	if((packet[0]&BIT(5))==BIT(5))
	{packet[2]=twos_complement(packet[2]);
	}

	printf(" %3s%x", "B01=0x", (unsigned char) packet[0]);
	printf(" %3s%x", "B02=0x", (unsigned char) packet[1]);
	printf(" %3s%x", "B03=0x", (unsigned char) packet[2]);
	printf(" %3s%d", "LB=", packet[0]&BIT(0));
	printf(" %3s%d", "MB=", (packet[0]&BIT(2))>>2);
	printf(" %3s%d", "RB=", (packet[0]&BIT(1))>>1);
	printf(" %4s%d", "XOV=", (packet[0]&BIT(6))>>6);
	printf(" %4s%d", "YOV=", (packet[0]&BIT(7))>>7);
	printf(" %2s%d", "X=", packet[1]);
	printf(" %2s%d\n", "Y=",  packet[2]);

	return 0;
}


char mouse_handler()
{
	unsigned long status;
	char scan;

	sys_inb(CTRL_REG , &status);
	if(status&BIT(0))
		sys_inb(IN_BUF,&status);
	tickdelay(micros_to_ticks(DELAY_US));

	scan=status;

	return scan;
}

int mouse_subscribe(void){
	int ret=hook_id_mouse;

	if(sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&hook_id_mouse)==0) //View this later
		if (sys_irqenable(&hook_id_mouse)==0)
			return ret;
	return -1;
}

int mouse_unsubscribe(void){
	if(sys_irqdisable(&hook_id_mouse)==0)
		if(sys_irqrmpolicy(&hook_id_mouse)==0)
			return 0;

	return -1;
}

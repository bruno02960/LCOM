#include<minix/syslib.h>
#include<minix/drivers.h>
#include<minix/com.h>
#include"i8042.h"
#include"i8254.h"
#include"mouse.h"
#include"timer.h"
#include"test4.h"

int test_packet(unsigned short cnt){
	unsigned int counter=0;
	char packet[3]={0, 0, 0};
	int fst=0, snd=0;
	unsigned int ans=mouse_subscribe();
	message msg;
	unsigned long rsp;
	char code;
	int ipc_status;

	if(ans==-1)
	{
		printf("Failed to subscribe mouse!\n");
		return 1;
	}

	mouse_command(SET_STREAM);
	mouse_command(DR_ENA);

	unsigned int irq_set=BIT(ans);
	while(counter<=cnt){

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
					rsp=mouse_handler();
					code=(char) rsp;
					if(fst==0){
						if (BIT(3) == (BIT(3) & code)){
							fst=1;
							packet[0]= code;}
						else
							break;}
					else if(snd==0){
						snd=1;
						packet[1]=code;}
					else{
						fst=0;
						snd=0;
						packet[2]=code;
						counter++;
						if (counter!=1)
							if(print_packet(packet))
							{
								printf("Corrupted packet!");
								return 1;
							}
					}
				}
				break;
			default:
				break;
			}
		}
	}

	ans=mouse_unsubscribe();
	if(ans==-1)
	{
		printf("Failed to unsubscribe mouse!\n");
		return 1;
	}

	timer_unsubscribe_int();
	mouse_command(DR_DIS);
	mouse_command(SET_STREAM);
	return 0;
}	

int test_async(unsigned short idle_time) {
        unsigned long rsp;
        int ipc_status, ans=mouse_subscribe(), fst, snd;
        message msg;
        char code;
        unsigned int counter=0;
        char packet[3]={0, 0, 0};
        mouse_command(SET_STREAM);
        mouse_command(DR_ENA);
        unsigned int time_counter=0;
        if(ans==-1)
        {
                printf("Failed to subscribe mouse!\n");
                return 1;
        }
        unsigned long mouse_set=BIT(ans);
        ans=timer_subscribe_int();
        if(ans==-1)
        {
        	printf("Failed to subscribe timer!\n");
        	return 1;
        }
        unsigned long timer_set=BIT(ans);
        while(time_counter<(60*idle_time)){
        	if (driver_receive(ANY,&msg,&ipc_status)!=0)
        	{
        		printf("driver_receive failed with: %d\n",driver_receive(ANY,&msg,&ipc_status));
        		continue;
        	}
        	if(is_ipc_notify(ipc_status))
        	{
        		if(msg.NOTIFY_ARG & timer_set)
        		{
        			time_counter++;
        		}
        		switch(_ENDPOINT_P(msg.m_source))
        		{
        		case HARDWARE:
        			if (msg.NOTIFY_ARG & mouse_set)
        			{
        				rsp=mouse_handler();
        				code=(char) rsp;
        				if(fst==0){
        					if (BIT(3) == (BIT(3) & code)){
        						fst=1;
        						packet[0]=code;}
        					else
        						continue;}
        				else if(snd==0){
        					snd=1;
        					packet[1]=code;}
        				else{
        					fst=0;
        					snd=0;
        					packet[2]=code;
        					counter++;
        					if(print_packet(packet))
        					{
        						printf("Corrupted packet!");
        						return 1;
        					}
        					time_counter=0;
        				}
        			}
        			break;
        		default:
        			break;
        		}
        	}
        }

        ans=mouse_unsubscribe();
        if(ans==-1)
        {
        	printf("Failed to unsubscribe mouse!\n");
        	return 1;
        }
        tickdelay(micros_to_ticks(DELAY_US));
        timer_unsubscribe_int();
        mouse_command(DR_DIS);
        mouse_command(SET_STREAM);

        return 0;

}

int test_config(void) {
	unsigned int counter=0;
	unsigned int a = 0;
	char packet[3]={0, 0, 0};
	int n_bits = 8;
	char code;
	int fst=0, snd=0;
	unsigned long rsp;
	message msg;
	int ipc_status;
	unsigned int ans=mouse_subscribe();


	if(ans==-1)
	{
		printf("Failed to subscribe mouse!\n");
		return 1;
	}

	mouse_command(SET_STREAM);
	mouse_command(DR_DIS);
	mouse_command(DR_ENA);
	mouse_command(GET_STATUS);

	read_status();

	ans=mouse_unsubscribe();
	if(ans==-1)
	{
		printf("Failed to unsubscribe mouse!\n");
		return 1;
	}

	mouse_command(DR_DIS);
	mouse_command(SET_STREAM);
	return 0;
}

typedef enum{INITIAL,DRAWING,END}States;


int test_gesture(short length) {
	unsigned int counter=0;
	unsigned char packet[3]={0, 0, 0};
	int fst=0, snd=0;
	unsigned int ans=mouse_subscribe();
	message msg;
	unsigned long rsp;
	char code;
	int ipc_status;
	int dx = 0;
	int dy = 0;
	int xi;
	int yi;
	int r_button=0;

	if(ans==-1)
	{
		printf("Failed to subscribe mouse!\n");
		return 1;
	}

	mouse_command(SET_STREAM);
	mouse_command(DR_ENA);

	unsigned int irq_set=BIT(ans);

	static States state= INITIAL;   //keeping the first state

	while(dy < length){

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
					//checks for 3 packets
					rsp=mouse_handler();
					code=(char) rsp;
					if(fst==0){
						if (BIT(3) == (BIT(3) & code)){
							fst=1;
							packet[0]= code;
							r_button = ((packet[0]&BIT(1))>>1);}
						else
							continue;}
					else if(snd==0){
						snd=1;
						packet[1]=code;
						if((packet[0]&BIT(4))==BIT(4))
						{packet[1]=~packet[1]+1;
						}
						xi = packet[1];
					}
					else{
						fst=0;
						snd=0;
						packet[2]=code;
						if((packet[0]&BIT(5))==BIT(5))
						{packet[2]=~packet[2]+1;
						}
						yi = packet[2];
						if(print_packet(packet))
						{
							printf("Corrupted packet!");
							return 1;
						}
					}


					//state machine and drawing line

					if (state == INITIAL)
					{
						if (r_button) //right button is pressed
						{
							state = DRAWING;
						}
					}
					else if (state == DRAWING)
					{
						if (!(r_button))
						{
							state = INITIAL;
							dx=0;
							dy=0;
							break;
						}
						else
						{	int dxi=dx;
						int dyi=dy;

						//checks for event where there is a line being drawn
						dx += xi; //updates the x movement
						dy += yi; //updates the y movement

						if (((packet[0]&BIT(5))>>5)!=((packet[0]&BIT(4))>>4)) //negative slope
						{
							dx=0;
							dy=0;
						}
						if (dy >= length)
						{
							state = END;
							printf("Gesture detected\nExiting program\n");
							break;
						}
						}
					}

				}
				break;
			default:
				break;
			}
		}
	}


	ans=mouse_unsubscribe();
	if(ans==-1)
	{
		printf("Failed to unsubscribe mouse!\n");
		return 1;
	}


	mouse_command(DR_DIS);
	mouse_command(SET_STREAM);
	return 0;
}

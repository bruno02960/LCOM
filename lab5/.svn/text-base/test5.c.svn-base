#include "test5.h"
#include <stdlib.h>
#include <sys/types.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include "vbe.h"
#include "lmlib.h"
#include "timer.h"
#include "i8042.h"
#include "xpm.h"

void *test_init(unsigned short mode, unsigned short delay) {

	char *video_mem;

	video_mem=(char*)vg_init(mode); //enters graphics mode

	if(video_mem==NULL)
		return NULL;

	int ipc_status, ans;
	message msg;
	unsigned int time_counter=0;

	if ((ans=timer_subscribe_int())==-1)
	{
		printf("Subscription failed!\n");
		return NULL;
	}

	unsigned long irq_set=BIT(ans);

	while(time_counter<delay*60){
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
				}
				break;
			default:
				break;

			}
		}
	}

	if(timer_unsubscribe_int())
		return NULL;

	if(vg_exit())
		return NULL;
	else
	{
		printf("Physical Address = 0x%X\n", video_mem);
		return video_mem;
	}

	return NULL;
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	unsigned long code;

	char *video_mem;

	vg_init(VG_MODE);

	int x_size = x + size;
	int y_size = y + size;

	unsigned h_res=getHRes();		/* Horizontal screen resolution in pixels */
	unsigned v_res=getVRes();		/* Vertical screen resolution in pixels */

	if (x > h_res || y > v_res || (x_size) > h_res || (y_size) > v_res)
	{
		vg_exit();
		printf("Wrong coordinates!\n");
		return -1;
	}

	video_mem=(char*)getVideoMem(); //enters graphics mode

	if(video_mem==NULL)
		return 1;

	int ipc_status, ans;
	message msg;

	if ((ans=kbd_subscribe())==-1)
	{
		printf("Subscription failed!\n");
		return 1;
	}

	unsigned long irq_set=BIT(ans);

	int xn = 0;
	int yn = 0;

	for (xn = x; xn < (x_size); xn++)
	{
		for (yn = y; yn < (y_size); yn++)
		{
			*(video_mem + (xn + yn * h_res)) = (char)color;
		}
	}

	while(code != ESC_COD){
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
					code=kbd_handler();
				}
				break;
			default:
				break;
			}
		}
	}

	if(vg_exit())
		return 1;
	kbd_unsubscribe();
	return 0;
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color) {

	unsigned long code;

	char *video_mem;

	vg_init(VG_MODE);

	unsigned h_res=getHRes();		/* Horizontal screen resolution in pixels */
	unsigned v_res=getVRes();		/* Vertical screen resolution in pixels */

	if (xi > h_res || xf > h_res || yi > v_res || yf > v_res)
	{
		vg_exit();
		printf("Error, wrong x coordinates\n");
		return 1;
	}

	video_mem=(char*)getVideoMem(); //enters graphics mode-> necessary for all functions that enter graphic mode!!!

	if(video_mem==NULL)
		return 1;

	int ipc_status, ans;
	message msg;

	if ((ans=kbd_subscribe())==-1)
	{
		printf("Subscription failed!\n");
		return 1;
	}

	unsigned long irq_set=BIT(ans);

	//draw the line
	int xn;
	int yn;

	//vertical line
	if (xf == xi)
	{
		for (yn = yi; yn < yf; yn++)
		{
			*(video_mem + (xi + yn * h_res)) = (char)color;
		}
	}
	//horizontal line
	if (yf == yi)
	{
		for (xn = xi; xn < xf; xn++)
		{
			*(video_mem + (xn + yi * h_res)) = (char)color;
		}
	}

	//diagonal line
	float m = (float)(yf-yi)/(xf-xi);
	float b = yi-m*xi;


	//for negative slope
	int x_max;	int x_min;	int y_max;	int y_min;

	if (m < 0)
	{
		if (xi > xf)
		{
			x_min = xf;
			x_max = xi;
		}
		else
		{
			x_min = xi;
			x_max = xf;
		}

		if (yi < yf)
		{
			y_min = yi;
			y_max = yf;
		}
		else
		{
			y_min = yf;
			y_max = yi;
		}

		for (yn = y_min; yn < y_max; yn++)
		{
			xn = (yn - b)/m;
			*(video_mem + (xn + yn * h_res)) = (char)color;
		}

	}
	else if (m > 0)
	{
		if (xi > xf)
		{
			x_min = xf;
			x_max = xi;
		}
		else
		{
			x_min = xi;
			x_max = xf;
		}

		for (xn = x_min; xn < x_max; xn ++)
		{
			yn = m*xn + b;
			*(video_mem + (xn + yn * h_res)) = (char)color;
		}
	}


	while(code != ESC_COD){
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
					code=kbd_handler();
				}
				break;
			default:
				break;

			}
		}
	}


	if(vg_exit()==1)
		return 1;
	kbd_unsubscribe();
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	unsigned long code;

	char *video_mem;

	vg_init(VG_MODE);

	video_mem=(char*)getVideoMem(); //enters graphics mode-> necessary for all functions that enter graphic mode!!!

	if(video_mem==NULL)
		return 1;

	int ipc_status, ans;
	message msg;

	if ((ans=kbd_subscribe())==-1)
	{
		vg_exit();
		printf("Subscription failed!\n");
		return 1;
	}

	//parameters for creator and destructor, useful for test_move
	int width;
	int height;

	xpm_creator(&width, &height, xi, yi, xpm);

	unsigned long irq_set=BIT(ans);

	while(code != ESC_COD){
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
					code=kbd_handler();
				}
				break;
			default:
				break;

			}
		}
	}

	if(vg_exit())
		return 1;
	kbd_unsubscribe();
	return 0;
}

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
		unsigned short hor, short delta, unsigned short time) {


	unsigned long code;

	char *video_mem;
	int time_counter = 0;
	int ticks = 0;
	double distance = 0;

	vg_init(VG_MODE);

	video_mem=(char*)getVideoMem(); //enters graphics mode-> necessary for all functions that enter graphic mode!!!

	if(video_mem==NULL)
		return 1;

	int ipc_status, kbd_ans, timer_ans;
	message msg;

	timer_ans=timer_subscribe_int();

	if (timer_ans==-1)
	{
		printf("Subscription failed!\n");
		return 1;
	}

	unsigned long irq_set_timer=BIT(timer_ans);


	kbd_ans=kbd_subscribe();
	if (kbd_ans==-1)
	{
		printf("Subscription failed!\n");
		return 1;
	}

	unsigned long irq_set=BIT(kbd_ans);

	//parameters for creator and destructor
	int width;
	int height;

	//creates xpm
	xpm_creator(&width, &height, xi,yi,xpm);

	unsigned h_res=getHRes();		/* Horizontal screen resolution in pixels */
	unsigned v_res=getVRes();		/* Vertical screen resolution in pixels */

	//condition to solve out of bounds movement
	if(hor)
	{
		if((xi+delta)>(int)h_res)
			delta=h_res-xi-width;
		if((xi+delta)<0)
			delta=(0-xi);
	}

	if(!hor)
	{
		if((yi+delta)>(int)v_res)
			delta=v_res-yi-height;
		if((yi+delta)<0)
			delta=(0-yi);
	}

	//movement per frame
	double mpf = (double) delta/(double)(time*60);

	//animation
	while(ticks<time*60 && code!=ESC_COD){
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
				if (msg.NOTIFY_ARG & irq_set_timer)
				{
					ticks++;
					//movement
					distance += mpf;
					if ((distance >= 1) || (distance <= -1))
					{
						int movement=(int) distance;
						switch(hor){
						case 0:
							xpm_destructor(&width, &height, xi,yi);//deletes xpm
							yi += movement;//changes y value
							xpm_creator(&width, &height, xi,yi,xpm);//creates xpm in new coordinates
							distance -= movement;
							break;
						case 1:
							xpm_destructor(&width, &height, xi,yi);//deletes xpm
							xi += movement;//changes x value
							xpm_creator(&width, &height, xi,yi,xpm);//creates xpm in new coordinates
							distance -= movement;
							break;
						default:
							break;}
					}
				}
				if (msg.NOTIFY_ARG & irq_set)
				{
					code=kbd_handler();
				}
			default:
				break;

			}
		}
	}

	if(vg_exit())
		return 1;
	kbd_unsubscribe();
	timer_unsubscribe_int();
	return 0;
}

int test_controller() {

	VbeInfoBlock info;

	short* virtual_address=lm_init();

	if(vbe_get_controller_info(VG_MODE,&info))
		return 1;

	printf("\nCapabilities:\n");

	if (info.Capabilities[0]&BIT(0))
		printf("DAC width is switchable to 8 bits per primary color\n");
	else
		printf("DAC is fixed width, with 6 bits per primary color\n");

	if (info.Capabilities[1]&BIT(1))
		printf("Controller is not VGA compatible\n");
	else
		printf("Controller is VGA compatible\n");

	if (info.Capabilities[2]&BIT(1))
		printf("When programming large blocks of information to the RAMDAC, use the blank bit in Function 09h\n");
	else
		printf("Normal RAMDAC operation\n");

	printf("\nList of supported video modes:\n");
	short* ptr=(short*)info.VideoModePtr;

	int int_ptr=(int) ptr;
	int_ptr=((int_ptr>>12)&0xF0000)+(int_ptr&0xFFFF);
	int_ptr+=(int)virtual_address;

	ptr=(short*) int_ptr;

	for(ptr;*ptr!=-1;ptr++)
	{
		printf("0x%x\t",*ptr);
	}

	printf("\n\nVRAM size:\n");
	printf("%dkb\n",info.TotalMemory * 64);

	return 0;
}

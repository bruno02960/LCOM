#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = BIOS; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void *vg_init(unsigned short mode)
{
	vbe_mode_info_t info;
	if(vbe_get_mode_info(mode,&info)==1)
		return NULL;


	int r;
	struct mem_range mr;
	h_res = info.XResolution;
	v_res = info.YResolution;
	bits_per_pixel = info.BitsPerPixel;


	unsigned int vram_base = info.PhysBasePtr;	//VRAM’s physical addresss
	unsigned int vram_size = (info.XResolution * info.YResolution * info.BitsPerPixel) / 8;	//VRAM’s size, but you can use
	//the frame-buffer size, instead

	//this sets the mode
	struct reg86u reg;
	reg.u.b.ah = VBE;          	// VBE
	reg.u.b.al = SET_MODE;		// Set Mode
	reg.u.w.bx = 1<<LINEAR_MODEL_BIT|VG_MODE;	// set bit 14: linear framebuffer
	reg.u.b.intno = BIOS;
	if( sys_int86(&reg) != OK )
	{
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	//Allow memory mapping
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

	//Map memory
	video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);
	if(video_mem == MAP_FAILED)
		panic("couldn’t map video memory");

	if(video_mem==NULL)
	{
		printf("Error!\n");
		vg_exit();
		return NULL;
	}
	else
		return video_mem;
}

char* getVideoMem()
{
	return video_mem;
}

unsigned getHRes()
{
	return h_res;
}

unsigned getVRes()
{
	return v_res;
}

unsigned getBitsPerPixel()
{
	return bits_per_pixel;
}

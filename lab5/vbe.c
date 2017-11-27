#include "vbe.h"
#include <minix/drivers.h>
#include <machine/int86.h>
#include "lmlib.h"

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
  
	mmap_t buf;
	struct reg86u r;

	if(lm_alloc(sizeof(vbe_mode_info_t),&buf)==NULL)
		return 1;

	r.u.b.ah = VBE;					//VBE get mode info
	r.u.b.al = INFO_MODE;			//translate the buffer linear address to a far pointer
	r.u.w.es = PB2BASE(buf.phys);	//set a segment base
	r.u.w.di = PB2OFF(buf.phys);	//set the offset accordingly
	r.u.w.cx = mode;
	r.u.b.intno = BIOS;
	if( sys_int86(&r) == OK )
		if (r.u.w.ax==VBE)
		{
			*vmi_p=*((vbe_mode_info_t*)buf.virtual);
		}
		else
			return 1;

	if (r.u.b.ah == FAILURE)
	{
		printf("Function call mapping failed!\n");
		lm_free(&buf);
		return 1;
	}
	else if (r.u.b.ah == NOT_SUPPORTED)
	{
		printf("Function is not supported in current HW configuration!\n");
		lm_free(&buf);
		return 1;
	}
	else if (r.u.b.ah == INVALID)
	{
		printf("Function is invalid in current video mode!\n");
		lm_free(&buf);
		return 1;
	}

	lm_free(&buf);

	return 0;
}

int vbe_get_controller_info(unsigned short mode, VbeInfoBlock *vmi_p) {

	mmap_t buf;
	struct reg86u r;

	if(lm_alloc(sizeof(VbeInfoBlock),&buf)==NULL)
		return 1;

	r.u.b.ah = VBE;          		//VBE
	r.u.b.al = CONTROLLER_INFO;		//Get Controller Info
	r.u.w.es = PB2BASE(buf.phys);	//set a segment base
	r.u.w.di = PB2OFF(buf.phys);	//set the offset accordingly
	r.u.w.cx = mode;
	r.u.b.intno = BIOS;
	if( sys_int86(&r) != OK )
		{
			lm_free(&buf);
			return 1;
		}
	else
		*vmi_p=*(VbeInfoBlock*) buf.virtual;

	if (r.u.b.ah == FAILURE)
	{
		printf("Function call mapping failed!\n");
		lm_free(&buf);
		return 1;
	}
	else if (r.u.b.ah == NOT_SUPPORTED)
	{
		printf("Function is not supported in current HW configuration!\n");
		lm_free(&buf);
		return 1;
	}
	else if (r.u.b.ah == INVALID)
	{
		printf("Function is invalid in current video mode!\n");
		lm_free(&buf);
		return 1;
	}

	lm_free(&buf);

	return 0;
}

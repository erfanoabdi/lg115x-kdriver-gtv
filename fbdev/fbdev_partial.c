

/**
 *	Necessary includes for device drivers
 */



#include <linux/module.h>
#include <linux/kernel.h>	/**< printk() */
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>		/**< kmalloc() */
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>		/** ioremap_memory()	*/
#include <asm/io.h>
#include <linux/fs.h> 		/**< everything\ldots{} */

#include <base_types.h>
#include "fbdev_partial.h"
#include "fbdev_cfg.h"
#include "l9/base_addr_sw_l9.h"

#if PARTIAL_UPDATE
static struct sPartialUpdate gsPartialUpdate = {
	.state = PU_DISABLE,

	// will be used for OSD
	.back_buffer	= 0,
    .back_buffer	= 0,
    .front_buffer	= 0,
    .virt_back_buffer	= 0,
	.virt_front_buffer	= 0,

	// will be used for GFX
	.x_res	= 0,
	.y_res	= 0,
	.x_pos	= 0,
	.y_pos	= 0,
	.stride	= 0,
	.format	= 0,
};

static struct sSurface gSurf = {
    .xres	= 1920,
    .yres	= 1080,

  	.stride	= 1920*4,
    .format	= 0x1e,
};

unsigned long gGfxAddr = 0;
#endif

 /************************************************************************
 *	LOCAL Functions
 */



/****************************************************************************
*	function body
*/

#if PARTIAL_UPDATE

int FBDEV_InitializeGFX(void)
{

	gGfxAddr = (unsigned long)ioremap(GFX_REGISTER_ADDRESS , GFX_REGISTER_SIZE);

	{

		LX_ADDR_SW_CFG_T	addr_sw_cfg_gfx;
		BASE_L9_GetAddrSwCfg ( ADDR_SW_CFG_GFX  /*GFX*/, &addr_sw_cfg_gfx );

		__raw_writel(addr_sw_cfg_gfx.de_sav, gGfxAddr + 0x1e0);
		__raw_writel(addr_sw_cfg_gfx.cpu_gpu, gGfxAddr + 0x1e4);
		__raw_writel(addr_sw_cfg_gfx.cpu_shadow, gGfxAddr + 0x1e8);
	}


	__raw_writel(0x0, gGfxAddr + GFX_CMD_DLY);

	__raw_writel(0x00000000, gGfxAddr + 0x68);		//
	__raw_writel(0x00000000, gGfxAddr + 0x6c);		//
	__raw_writel(0x001b001b, gGfxAddr + 0x7c);		// R0 ordering
	__raw_writel(0x00000000, gGfxAddr + 0x134);		// WR CSC
	__raw_writel(0x001b001b, gGfxAddr + 0x138);		// WR ordering

	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);

	return 0;
}


int FBDEV_SetPartialSurfaceWindow(unsigned long xres, unsigned long yres, unsigned long format)
{
	int ret = 0;

	gSurf.xres = xres;
	gSurf.yres = yres;
	gSurf.format = format;

	if(format >= 0xe) gSurf.stride = xres*4;
	else if(format >= 0xc) gSurf.stride = xres*3;
	else if(format >= 0x8) gSurf.stride = xres*2;
	else
	{
		gSurf.stride = xres*4;
		printk("error : no format\n");
		ret = -1;
	}

	gSurf.format = 0x1e;

	return ret;
}

int FBDEV_GetPartialUpdateStatus(void)
{

	return gsPartialUpdate.state;
}

int FBDEV_SetPartialUpdateStatus(void)
{
	int ret = 0;

	return ret;

}

int FBDEV_SetFrontBuffer(unsigned long addr)
{
	unsigned long surface_offset, header_offset, base_addr ;

	surface_offset = 1920*1080*4;//gSurf.xres * gSurf.yres * 4 ;
	header_offset = 0x0;
	base_addr = gMemCfgFBDev[0].mem_frame_base;


	if(gsPartialUpdate.state == PU_DISABLE)
	{

		if(addr == 0)
		{
			gsPartialUpdate.front_buffer = base_addr + header_offset + addr;
			gsPartialUpdate.virt_front_buffer = (unsigned long)(gsPartialUpdate.front_buffer + surface_offset*2);
			gsPartialUpdate.back_buffer = (unsigned long)(base_addr + header_offset + surface_offset);
			gsPartialUpdate.virt_back_buffer = (unsigned long)(gsPartialUpdate.back_buffer + surface_offset*2);
		}
		else
		{
			gsPartialUpdate.front_buffer = base_addr + header_offset + addr;
			gsPartialUpdate.virt_front_buffer = (unsigned long)(gsPartialUpdate.front_buffer + surface_offset*2);
			gsPartialUpdate.back_buffer = (unsigned long)(base_addr + header_offset);
			gsPartialUpdate.virt_back_buffer = (unsigned long)(gsPartialUpdate.back_buffer + surface_offset*2);
		}
	}
	else
	{
		if(addr == 0)
		{
			gsPartialUpdate.front_buffer = base_addr + header_offset + addr;
			gsPartialUpdate.virt_front_buffer = (unsigned long)(gsPartialUpdate.front_buffer + surface_offset*2);
			gsPartialUpdate.back_buffer = (unsigned long)(base_addr + header_offset + surface_offset);
			gsPartialUpdate.virt_back_buffer = (unsigned long)(gsPartialUpdate.back_buffer + surface_offset*2);
		}
		else
		{
			gsPartialUpdate.front_buffer = base_addr + header_offset + addr;
			gsPartialUpdate.virt_front_buffer = (unsigned long)(gsPartialUpdate.front_buffer + surface_offset*2);
			gsPartialUpdate.back_buffer = (unsigned long)(base_addr + header_offset);
			gsPartialUpdate.virt_back_buffer = (unsigned long)(gsPartialUpdate.back_buffer + surface_offset*2);
		}
	}

#if 0
	printk(" start : %08x addr : %08x\n", gsPartialUpdate.state , addr);
	printk(" front buffer : %08x , back buffer : %08x\n", gsPartialUpdate.front_buffer , gsPartialUpdate.back_buffer);
	printk(" virt front buffer : %08x , virt back buffer : %08x\n", gsPartialUpdate.virt_front_buffer , gsPartialUpdate.virt_back_buffer);
#endif

	return 0;
}

int FBDEV_WaitForUpdateComplete(void)
{
	int ret = 0, count = 0;

	while(1)
    	{
       		volatile unsigned int temp;

			__raw_writel(0x1, gGfxAddr + 0x30);

        	temp = __raw_readl(gGfxAddr + 0x008);
	        if( (temp&0x3ff0000) >= 0x1000000 )	// until all command in stack are excuted
        	{
	            break;
	        }

		if(count++ > 17)
		{
			printk("GFX wait complete failed\n");
			return -1;
		}
		msleep(1);
    	}

	return ret;
}

int FBDEV_RunGFXClearSurface( unsigned long dst_addr , unsigned long width , unsigned long height , unsigned long color)
{

	if(gGfxAddr == 0)
	{
		FBDEV_InitializeGFX();
	}

	if(FBDEV_WaitForUpdateComplete() < 0)
	{
		return -1;
	}

	// mode setting
	__raw_writel(0x1, gGfxAddr + GFX_OP_MODE);
	__raw_writel(0x5, gGfxAddr + GFX_OUT_SEL);

	// WR setting
	__raw_writel(dst_addr, gGfxAddr + GFX_WR_BASE_ADDR);
	__raw_writel(width*4, gGfxAddr + GFX_WR_STRIDE);
	__raw_writel(0x1e, gGfxAddr + GFX_WR_FORMAT);
	__raw_writel( (height<<16 | width), gGfxAddr + GFX_WR_SIZE);
	__raw_writel(color, gGfxAddr + GFX_WR_GALPHA);
	__raw_writel(0, gGfxAddr + GFX_ROP_CTRL);
	__raw_writel(0, gGfxAddr + GFX_WR_CTRL);

	// run
	__raw_writel(0x1, gGfxAddr + GFX_START);
	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);
	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);
	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);

	if(FBDEV_WaitForUpdateComplete() < 0)
	{
		return -1;
	}

	__raw_writel(0x0, gGfxAddr + GFX_OP_MODE);
	__raw_writel(0x0, gGfxAddr + GFX_OUT_SEL);

	return 0;
}

int FBDEV_RunGFXBlit(unsigned long src_addr , unsigned long dst_addr , unsigned long width, unsigned long height, unsigned long stride)
{
	int ret = 0;


	// R0 setting
	__raw_writel(src_addr, gGfxAddr + GFX_R0_BASE_ADDR);
	__raw_writel(stride, gGfxAddr + GFX_R0_STRIDE);
	__raw_writel(gSurf.format, gGfxAddr + GFX_R0_FORMAT);

	// WR setting
	__raw_writel(dst_addr, gGfxAddr + GFX_WR_BASE_ADDR);
	__raw_writel(stride, gGfxAddr + GFX_WR_STRIDE);
	__raw_writel(gSurf.format, gGfxAddr + GFX_WR_FORMAT);
	__raw_writel( (height<<16 | width), gGfxAddr + GFX_WR_SIZE);

	// mode setting
	__raw_writel(0x1, gGfxAddr + GFX_OP_MODE);
	__raw_writel(0x0, gGfxAddr + GFX_OUT_SEL);

	// run
	__raw_writel(0x1, gGfxAddr + GFX_START);
	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);
	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);
	__raw_writel(0x1, gGfxAddr + GFX_BATCH_RUN);
	return ret;
}


int FBDEV_RunPartialBackSurfaceCopy(unsigned long *start)
{
	int ret = 0;
	unsigned long src_addr, dst_addr, width, height, stride;

	if(1)
	{
		if(FBDEV_WaitForUpdateComplete() < 0)
		{
			return -1;
		}
	}

	src_addr = gsPartialUpdate.back_buffer + gsPartialUpdate.x_pos*gsPartialUpdate.format + gsPartialUpdate.y_pos*gsPartialUpdate.stride;
	dst_addr = gsPartialUpdate.virt_back_buffer + gsPartialUpdate.x_pos*gsPartialUpdate.format + gsPartialUpdate.y_pos*gsPartialUpdate.stride;
	stride = gsPartialUpdate.stride;
	width = gsPartialUpdate.x_res;
	height = gsPartialUpdate.y_res;


	FBDEV_RunGFXBlit(src_addr , dst_addr , width, height, stride);

	if(1)
	{
		if(FBDEV_WaitForUpdateComplete() < 0)
		{
			return -1;
		}
		*start = gsPartialUpdate.virt_back_buffer;
	}
#if 0
	printk("back copy src %08x dst %08x\n", gsPartialUpdate.back_buffer, gsPartialUpdate.virt_back_buffer);
#endif
	return ret;
}


int FBDEV_RunFullSurfaceCopy(unsigned long src_addr , unsigned long dst_addr , unsigned long width, unsigned long height, unsigned long stride)
{

	if(gGfxAddr == 0)
	{
		FBDEV_InitializeGFX();
	}

	if(FBDEV_WaitForUpdateComplete() < 0)
		return -1;

	FBDEV_RunGFXBlit(src_addr , dst_addr , width, height, stride);
	return 0;
}
int FBDEV_RunPartialFrontSurfaceCopy(void)
{

	int ret = 0;
	unsigned long src_addr, dst_addr, width, height, stride;

	src_addr = gsPartialUpdate.virt_front_buffer + gsPartialUpdate.x_pos*gsPartialUpdate.format + gsPartialUpdate.y_pos*gsPartialUpdate.stride;
	dst_addr = gsPartialUpdate.virt_back_buffer + gsPartialUpdate.x_pos*gsPartialUpdate.format + gsPartialUpdate.y_pos*gsPartialUpdate.stride;
	stride = gsPartialUpdate.stride;
	width = gsPartialUpdate.x_res;
	height = gsPartialUpdate.y_res;
	FBDEV_RunGFXBlit(src_addr , dst_addr , width, height, stride);

#if 0
	printk("front copy src %08x dst %08x\n", gsPartialUpdate.virt_front_buffer, gsPartialUpdate.virt_back_buffer);
#endif

	return ret;
}


int FBDEV_SetPartialUpdate(struct  sPartialBlitParam  *sBlit)
{
	int ret = 0;

	unsigned long xres_set = gSurf.xres;	// FIXME : modify the hard coding value
	unsigned long yres_set = gSurf.yres;


	if( (sBlit->x_res == xres_set) && (sBlit->y_res == yres_set) )
	{
		gsPartialUpdate.state = PU_DISABLE;
	}
	else
	{
		switch(gsPartialUpdate.state)
		{
			case PU_DISABLE:
			FBDEV_RunFullSurfaceCopy(gsPartialUpdate.front_buffer , gsPartialUpdate.virt_back_buffer , gSurf.xres, gSurf.yres, gSurf.stride);
			gsPartialUpdate.state = PU_FIRST_UPDATE;
			break;
			case PU_FIRST_UPDATE:
			case PU_SECOND_UPDATE:
			FBDEV_RunFullSurfaceCopy(gsPartialUpdate.virt_front_buffer , gsPartialUpdate.virt_back_buffer , gSurf.xres, gSurf.yres, gSurf.stride);
			gsPartialUpdate.state = PU_UPDATING;
			break;
			case PU_UPDATING:

			break;
		}

		gsPartialUpdate.x_res = sBlit->x_res;
		gsPartialUpdate.y_res = sBlit->y_res;
		gsPartialUpdate.x_pos = sBlit->x_pos;
		gsPartialUpdate.y_pos = sBlit->y_pos;

		gsPartialUpdate.format = 0x4;		// 4byte per a pixel
		gsPartialUpdate.stride = gSurf.stride;
	}

	return ret;
}
#endif //PARTIAL_UPDATE



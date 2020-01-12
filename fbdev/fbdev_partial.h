

#define	PARTIAL_UPDATE	1

#if PARTIAL_UPDATE
struct sPartialBlitParam
{
	unsigned long x_res;	// partial update area
	unsigned long y_res;

	unsigned long x_pos;	// partial update position
	unsigned long y_pos;
};

// sPartialUpdate state
#define	PU_DISABLE		0
#define	PU_FIRST_UPDATE		1
#define	PU_SECOND_UPDATE	2
#define	PU_UPDATING		3

// 2 buffer, 2 virtual buffer
struct sPartialUpdate
{
	unsigned long state;

	// for OSD
	unsigned long back_buffer;
	unsigned long front_buffer;
	unsigned long virt_back_buffer;
	unsigned long virt_front_buffer;

	// for GFX
	unsigned long x_res;
	unsigned long y_res;
	unsigned long x_pos;
	unsigned long y_pos;
	unsigned long stride;
	unsigned long format;
};

struct sSurface
{
	unsigned long xres;
	unsigned long yres;

	unsigned long stride;
	unsigned long format;
};

// for GFX control
#define	GFX_REGISTER_ADDRESS	0xc001d000
#define	GFX_REGISTER_SIZE	0x000001f0

#define	GFX_STATUS1		0x008
#define	GFX_OP_MODE		0x020	// 1
#define	GFX_BATCH_RUN		0x030
#define	GFX_START		0x034
#define	GFX_CMD_DLY		0x02c
#define	GFX_R0_BASE_ADDR	0x050
#define	GFX_R0_STRIDE		0x054
#define	GFX_R0_FORMAT		0x058
#define	GFX_OUT_SEL		0x100
#define	GFX_ROP_CTRL	0x110
#define	GFX_WR_BASE_ADDR	0x120
#define	GFX_WR_STRIDE		0x124
#define	GFX_WR_FORMAT		0x128
#define	GFX_WR_SIZE		0x12c
#define	GFX_WR_GALPHA	0x130
#define	GFX_WR_CTRL		0x134


// function prototype

int FBDEV_SetPartialSurfaceWindow(unsigned long xres, unsigned long yres, unsigned long format);
int FBDEV_GetPartialUpdateStatus(void);
int FBDEV_SetPartialUpdate(struct  sPartialBlitParam  *sBlit);
int FBDEV_RunPartialBackSurfaceCopy(unsigned long *start);
int FBDEV_RunPartialFrontSurfaceCopy(void);
int FBDEV_SetFrontBuffer(unsigned long addr);

#endif	// PARTIAL_UPDATE


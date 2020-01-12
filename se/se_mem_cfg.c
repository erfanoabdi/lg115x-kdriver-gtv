/** @file
 *
 *  se memcfg data. this code is included by base_mem_cfg.c.
 *
 */

#if 1
LX_SE_MEM_CFG_T gMemCfgSE = {
	.name = "se_mem",
	.base = 0x7FD00000,
	.size = 0x00200000,
};
EXPORT_SYMBOL(gMemCfgSE);

LX_SE_MEM_CFG_T gMemCfgSE_TZFW = {
	.name = "se_tzfw",
	.base = 0x7FF00000,
	.size = 0x00100000,
};
EXPORT_SYMBOL(gMemCfgSE_TZFW);
#endif

#if 0
/*
 * The memory header is for COSMO only.
 */
LX_SE_MEM_CFG_T gMemCfgSE = {
	.name = "se_mem",
	.base = 0x2fd00000, /*INCLUDE_GTV*/
	.size = 0x00200000,
};
EXPORT_SYMBOL(gMemCfgSE);

LX_SE_MEM_CFG_T gMemCfgSE_TZFW = {
	.name = "se_tzfw",
	.base = 0x2ff00000, /*INCLUDE_GTV*/
	.size = 0x00100000,
};
EXPORT_SYMBOL(gMemCfgSE_TZFW);
#endif


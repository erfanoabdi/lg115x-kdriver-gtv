/*
	SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
	Copyright(c) 2010 by LG Electronics Inc.

	This program is free software; you can redistribute it and/or 
	modify it under the terms of the GNU General Public License
	version 2 as published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of 
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
	GNU General Public License for more details.
*/ 

#ifdef SUPPORT_LG_GP2_NAND
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define MTDINFO_DEBUG		(0)
#if (MTDINFO_DEBUG)
#define dprintk(fmt, args...)		printk(fmt, ## args)
#else
#define dprintk(fmt, args...)
#endif

#define NAND_MTDINFO_OFFSET 	0xc0000
#define MTD_STR_LEN 			32
#define MTD_MAP_MAX 			4
#define MTD_PARTITION_MAX		64

struct mtd_map_info {
	char name[MTD_STR_LEN];
	unsigned int size;
	unsigned int phys;
	void *virt;
	void *cached;
	int bankwidth;
	unsigned int used;
};

struct mtd_partition_info {
	char name[MTD_STR_LEN]; 	/* identifier string */
	unsigned int offset;		/* offset within the master MTD space */
	unsigned int size;			/* partition size */
	char filename[MTD_STR_LEN]; /* file name */
	unsigned int filesize;		/* file size */
	unsigned int sw_ver;		/* software version */
	unsigned char used; 	/* Is this partition is used? */
	unsigned char valid;		/* Is this partition is valid? */
	unsigned char mask_flags;		/* master MTD flags to mask out for this partition */
};

struct lg_mtd_info {
	unsigned int		magic;
	unsigned int		cur_epk_ver;
	unsigned int		old_epk_ver;
	unsigned char		nmap;
	unsigned char		npartition;
	struct mtd_map_info map[MTD_MAP_MAX];
	struct mtd_partition_info	partition[MTD_PARTITION_MAX];
} ;

struct mtd_partition lg_flash_parts[MTD_PARTITION_MAX];

int nandsim_get_mtd_partitions(struct mtd_info *mtd);

static struct mtd_info *nand_mtd = NULL;

static int nb_parts = 0;

#include <linux/proc_fs.h>

static struct proc_dir_entry *proc_mtdinfo = NULL;

static int mtdinfo_read_proc (char *page, char **start, off_t off, int count,
			  int *eof, void *data_unused)
{
	return 0;
}

static int mtdinfo_write_proc(struct file *file, const char __user *buffer,
		    unsigned long count, void *data)
{
#if 0
	char c;
	int rc;

	rc = get_user(c, buffer);
	if (rc)
		return rc;

	switch (c) {
		case 'a':
			nandsim_get_mtd_partitions(nand_mtd);
			break;
		default:
			break;
	}
	return count;
#endif
	return count;
}

int nandsim_proc_mtdinfo_init(struct mtd_info *mtd)
{
	if ((proc_mtdinfo = create_proc_entry( "mtdinfo", 0, NULL ))) {
		proc_mtdinfo->read_proc = mtdinfo_read_proc;
		proc_mtdinfo->write_proc = mtdinfo_write_proc;
	}

	return 0;
}

void nandsim_proc_mtdinfo_exit(void)
{
	if (proc_mtdinfo)
		remove_proc_entry("mtdinfo", NULL);
}

/*---------------------------------------------------------------------------------------
 * read map_file, parse flash map data and construct GP2 mtd maps
 *
 *
 *---------------------------------------------------------------------------------------*/
int nandsim_process_mtd_map_file(struct mtd_info* mtd, char* map_file )
{
	ssize_t						tx;
	struct file*				file_fd;
	struct lg_mtd_info* 		lg_mtdinfo;
	struct mtd_partition_info*	lg_ptt;
	int 						i;
	loff_t						pos = 0;

	if ( !map_file )
	{
		return 0;
	}

	/* open map file */
	file_fd = filp_open(map_file, O_RDONLY | O_LARGEFILE, 0600);

	if (IS_ERR(file_fd))
		return PTR_ERR(file_fd);

    if(!nand_mtd)
	{
		nand_mtd = mtd;
	}

	/* allocate lg_mtdinfo */
	lg_mtdinfo = (struct lg_mtd_info *)kmalloc (sizeof(struct lg_mtd_info), GFP_KERNEL);

	if(lg_mtdinfo == NULL)
	{
		printk("<error> %s : kmalloc error\n", __func__);
		return -1;
	}

	/* read mtdinfo file */
	{
		mm_segment_t 	old_fs;
		old_fs = get_fs();
		set_fs(get_ds());

		tx = vfs_read(file_fd, (char __user *)lg_mtdinfo, sizeof(struct lg_mtd_info), &pos );

		set_fs(old_fs);

		if ( tx != sizeof(struct lg_mtd_info) )
		{
			printk("<warning> insufficient data read. requested size = %d, actual read size = %d\n", sizeof(struct lg_mtd_info), tx );
		}
	}

	/* process map file */
	nb_parts = lg_mtdinfo->npartition;

	printk("mtd info : # of partition = %d\n", nb_parts);

	for(i=0; i<nb_parts; i++)
	{
		lg_ptt = (struct mtd_partition_info*)&(lg_mtdinfo->partition[i]);
		lg_flash_parts[i].name = lg_ptt->name;
        lg_flash_parts[i].offset = (uint64_t)lg_ptt->offset;
        lg_flash_parts[i].size = (uint64_t)lg_ptt->size;

#ifdef FLASH_DEBUG
        printk("{\n");
        printk("  .name   = \"%s\"\n", lg_flash_parts[i].name);
        printk("  .offset = 0x%llx\n", lg_flash_parts[i].offset);
        printk("  .size   = 0x%llx\n", lg_flash_parts[i].size);
        printk("}\n");
#endif
	}

	lg_flash_parts[i].name = "total";
	lg_flash_parts[i].offset = 0x0;
	lg_flash_parts[i].size = mtd->size;
	lg_flash_parts[i].mask_flags = 0;
	nb_parts++;

	add_mtd_partitions(mtd, lg_flash_parts, nb_parts);

	filp_close(file_fd, NULL);
    return 0;
}

/*---------------------------------------------------------------------------------------
 *
 * GP2 oritinal code
 *
 *---------------------------------------------------------------------------------------*/
int nandsim_get_mtd_partitions(struct mtd_info *mtd)
{
	struct lg_mtd_info * lg_mtdinfo;
	struct mtd_partition_info	*lg_ptt;
	int i=0, ret=0, retlen=0;
	unsigned long mtdinfo_offset = NAND_MTDINFO_OFFSET;

	if(!nand_mtd) {
		nand_mtd = mtd;
//		nandsim_proc_mtdinfo_init(mtd);
	}

	lg_mtdinfo = (struct lg_mtd_info *)kmalloc (sizeof(struct lg_mtd_info), GFP_KERNEL);
	if(lg_mtdinfo == NULL) {
		printk("%s : kmalloc error\n", __func__);
		return 1;
	}

	ret = mtd->read(mtd, mtdinfo_offset, sizeof(struct lg_mtd_info),
							&retlen, (u_char *)(lg_mtdinfo));
	if(ret) {
		printk("%s : mtdinfo read failed from nand flash\n", __func__);
		return ret;
	}

	nb_parts = lg_mtdinfo->npartition;

	printk("mtd info : # of partition = %d\n", nb_parts);
	for(i=0; i<nb_parts; i++) {
		lg_ptt = (struct mtd_partition_info*)&(lg_mtdinfo->partition[i]);
		lg_flash_parts[i].name = lg_ptt->name;
		lg_flash_parts[i].offset = (uint64_t)lg_ptt->offset;
		lg_flash_parts[i].size = (uint64_t)lg_ptt->size;
#ifdef FLASH_DEBUG
		printk("{\n");
		printk("  .name =		\"%s\"\n", lg_flash_parts[i].name);
		printk("  .offset = 0x%llx\n", lg_flash_parts[i].offset);
		printk("  .size =		0x%llx\n", lg_flash_parts[i].size);
		printk("}\n");
#endif
	}

	lg_flash_parts[i].name = "total";
	lg_flash_parts[i].offset = 0x0;
	lg_flash_parts[i].size = mtd->size;
	lg_flash_parts[i].mask_flags = 0;
	nb_parts++;

	add_mtd_partitions(mtd, lg_flash_parts, nb_parts);

//	kfree(lg_mtdinfo);
	return 0;
}

#endif //SUPPORT_LG_GP2_NAND

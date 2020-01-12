-------------------------------------------------------------------------------
	README.txt by raxis.lim@lge.com
-------------------------------------------------------------------------------

What is nandsim ?

	Nand simulator is originally supported by linux kernel.
	it is located at linux/driver/mtd/nand/nandsim.c

What patches are applied ?

	- orginal nandsim uses main memory for nand flash storage.
	  I found some useful patch to use file as flash storage.

	- simple code added to understand GP2's flash map
	- simele code added to fix data corruption program after unloading module.

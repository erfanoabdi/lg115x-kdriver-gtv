
#include "../adec_coredrv_l9.h"
#include "../adec_common/adec_mutex.h"
#include "adec_common_buf.h"
#include "adec_inter_buf_arm.h"

static void ARM_DCache_Invalidate(void* _addr, unsigned int _length, void* _adap);
static void ARM_DCache_Writeback(void* _addr, unsigned int _length, void* _adap);
static unsigned char* ARM_Mem_Translator(unsigned char* _addr, void* _adap);


ARM_PROCESSOR_ADAP gCPBM_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator
};

ARM_PROCESSOR_ADAP gCPBA_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator

};

ARM_PROCESSOR_ADAP gMIX0_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator

};

ARM_PROCESSOR_ADAP gMIX1_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator

};

ARM_PROCESSOR_ADAP gMIX2_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator

};

ARM_PROCESSOR_ADAP gMIX3_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator

};

ARM_PROCESSOR_ADAP gAENC_ArmReaderAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator
};

ARM_PROCESSOR_ADAP gAPCM_ArmReaderAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator
};

ARM_PROCESSOR_ADAP gLLB_ArmWriterAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator

};

ARM_PROCESSOR_ADAP gAACR_ArmReaderAdap =
{
	ARM_DCache_Invalidate,
	ARM_DCache_Writeback,
	ARM_Mem_Translator
};

static void ARM_DCache_Invalidate(void* _addr, unsigned int _length, void* _adap)
{
	return;
}

static void ARM_DCache_Writeback(void* _addr, unsigned int _length, void* _adap)
{
	return;
}

static unsigned char* ARM_Mem_Translator(unsigned char* _addr, void* _adap)
{
	ARM_PROCESSOR_ADAP* adap;
	int offset;

	adap = (ARM_PROCESSOR_ADAP*) _adap;

	if( adap->memBase > _addr || adap->memBase + adap->memSize < _addr )
	{
	    ADEC_ASSERT(0,
       	 (unsigned char*)NULL,
        	"exceed size(addr = 0x%X).\n", (unsigned int)_addr);
		return _addr;
	}

	offset = (unsigned char*)_addr - adap->memBase;

	return adap->dummyAddr + offset;/**/
}


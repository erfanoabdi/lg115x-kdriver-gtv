#ifndef _DE_XDP_h
#define _DE_XDP_h

#define XDP_SET_ID "DE"

int CLI_XDP_Start(void);
int CLI_XDP_DebugOn(int argc, char **argv);
int CL_XDP_DebugOn(char *pSetId, char *pOn);
int CLI_XDP_ReadRegister(int argc, char **argv);
int CL_XDP_ReadRegister(char *pSetId, UINT32 addr);
int CLI_XDP_WriteRegister(int argc, char **argv);
int CL_XDP_WriteRegister(char *pSetId, UINT32 addr, UINT32 value);

//BOOLEAN CL_XDP_SetStatus(BOOLEAN bValue);
//BOOLEAN CL_XDP_GetStatus(void);

#endif

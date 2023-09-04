#ifndef _pUCL_H_
#define _pUCL_H_
#ifdef GCOMP_UCL
typedef struct
{
	char chunk_crc[5]; //"UCLC"
	char file[64];
	unsigned int orgsze;
	unsigned int compsze;
} UCL_Chunk;

BOOL UCLGetOneFile(const char*,const char*,const char*);
BOOL UCLGetFile(const char*,const char*,const char*);
void UCLUnGetFile(const char*);
BOOL UCLLoadDLL(void);
void UCLFreeDLL(void);
#else
#define UCLGetOneFile
#define UCLGetFile
#define UCLUnGetFile
#define UCLLoadDLL
#define UCLFreeDLL
#endif
#endif

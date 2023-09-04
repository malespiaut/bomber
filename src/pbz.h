#ifndef _pBZ_H_
#define _pBZ_H_
#ifdef GCOMP_BZ
typedef struct
{
	char chunk_crc[5]; //"BZIP"
	char file[64];
	unsigned int orgsze;
	unsigned int compsze;
} BZ_Chunk;

BOOL BZGetOneFile(const char*,const char*,const char*);
BOOL BZGetFile(const char*,const char*,const char*);
void BZUnGetFile(const char*);
BOOL BZLoadDLL(void);
void BZFreeDLL(void);
#else
#define BZGetOneFile
#define BZGetFile
#define BZUnGetFile
#define BZLoadDLL
#define BZFreeDLL
#endif
#endif

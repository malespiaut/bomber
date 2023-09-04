#ifndef _pLZO_H_
#define _pLZO_H_
#ifdef GCOMP_LZO
typedef struct
{
	char chunk_crc[5]; //"LZOC"
	char file[64];
	unsigned int orgsze;
	unsigned int compsze;
} LZO_Chunk;

BOOL LZOGetOneFile(const char*,const char*,const char*);
BOOL LZOGetFile(const char*,const char*,const char*);
void LZOUnGetFile(const char*);
BOOL LZOLoadDLL(void);
void LZOFreeDLL(void);
#else
#define LZOGetOneFile
#define LZOGetFile
#define LZOUnGetFile
#define LZOLoadDLL
#define LZOFreeDLL
#endif
#endif

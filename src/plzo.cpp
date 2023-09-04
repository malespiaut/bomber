#include "General.h"
#ifdef GCOMP_LZO
#include <lzo1x.h>
#include "Debug.h"
#include "pLZO.h"

BOOL LZOLoadDLL()
{
	lzo_init();
	return TRUE;
}

void LZOFreeDLL()
{
	return;
}

BOOL LZOGetFile(const char* ArcName,const char* FileName,const char *Password)
{
	return LZOGetOneFile(ArcName,FileName,Password);
}

BOOL LZOGetOneFile(const char* ArcName,const char* FileName,const char *Password)
{
	LZO_Chunk chunk;
	char* File=new char[strlen(FileName)+1];
	strcpy(File,FileName);

	FILE* lzof=fopen(ArcName,"rb");
	if(lzof==0)
		return FALSE;
	while(1==fread(&chunk,sizeof(LZO_Chunk),1,lzof))
	{
		if(strcmp(strtoupper(chunk.file),strtoupper(File))==0)
		{
			if(strcmp(chunk.chunk_crc,"LZOC")!=0)
				break;
			lzo_bytep src=new lzo_byte[chunk.compsze];
			lzo_bytep dst=new lzo_byte[chunk.orgsze];
			fread(src,chunk.compsze,1,lzof);
			lzo_uint sze=chunk.orgsze;
			lzo1x_decompress_safe(src,chunk.compsze,dst,&sze,0);

			FILE* fdst=fopen(FileName,"wb");
			fwrite(dst,chunk.orgsze,1,fdst);
			fclose(fdst);

			delete[]src;
			delete[]dst;
			fclose(lzof);
			delete[]File;
			return TRUE;
		}
		else
			fseek(lzof,chunk.compsze,SEEK_CUR);
	}
	fclose(lzof);
	delete[]File;
	return FALSE;
}

void LZOUnGetFile(const char* FileName)
{
	remove(FileName);
	return;
}
#endif
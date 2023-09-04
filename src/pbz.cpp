#include "General.h"
#ifdef GCOMP_BZ
#include <bzlib.h>
#include "Debug.h"
#include "pBZ.h"

extern "C" void bz_internal_error(int e)
{
	return;
}

BOOL BZLoadDLL()
{
	return TRUE;
}

void BZFreeDLL()
{
	return;
}

BOOL BZGetFile(const char* ArcName,const char* FileName,const char *Password)
{
	return BZGetOneFile(ArcName,FileName,Password);
}

BOOL BZGetOneFile(const char* ArcName,const char* FileName,const char *Password)
{
	BZ_Chunk chunk;
	char* File=new char[strlen(FileName)+1];
	strcpy(File,FileName);

	FILE* bzf=fopen(ArcName,"rb");
	if(bzf==0)
		return FALSE;
	while(1==fread(&chunk,sizeof(BZ_Chunk),1,bzf))
	{
		if(strcmp(strtoupper(chunk.file),strtoupper(File))==0)
		{
			if(strcmp(chunk.chunk_crc,"BZIP")!=0)
				break;
			char* src=new char[chunk.compsze];
			char* dst=new char[chunk.orgsze];
			fread(src,chunk.compsze,1,bzf);
			unsigned int sze=chunk.orgsze;
			bzBuffToBuffDecompress(dst,&sze,src,chunk.compsze,0,0);

			FILE* fdst=fopen(FileName,"wb");
			fwrite(dst,chunk.orgsze,1,fdst);
			fclose(fdst);

			delete[]src;
			delete[]dst;
			fclose(bzf);
			delete[]File;
			return TRUE;
		}
		else
			fseek(bzf,chunk.compsze,SEEK_CUR);
	}
	fclose(bzf);
	delete[]File;
	return FALSE;
}

void BZUnGetFile(const char* FileName)
{
	remove(FileName);
	return;
}
#endif
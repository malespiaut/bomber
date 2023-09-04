#include "General.h"
#ifdef GCOMP_UCL
#include <ucl/ucl.h>
#include "Debug.h"
#include "pUCL.h"

BOOL UCLLoadDLL()
{
	ucl_init();
	return TRUE;
}

void UCLFreeDLL()
{
	return;
}

BOOL UCLGetFile(const char* ArcName,const char* FileName,const char *Password)
{
	return UCLGetOneFile(ArcName,FileName,Password);
}

BOOL UCLGetOneFile(const char* ArcName,const char* FileName,const char *Password)
{
	UCL_Chunk chunk;
	char* File=new char[strlen(FileName)+1];
	strcpy(File,FileName);

	FILE* uclf=fopen(ArcName,"rb");
	if(uclf==0)
		return FALSE;
	while(1==fread(&chunk,sizeof(UCL_Chunk),1,uclf))
	{
		if(strcmp(strtoupper(chunk.file),strtoupper(File))==0)
		{
			if(strcmp(chunk.chunk_crc,"UCLC")!=0)
				break;
			ucl_bytep src=new ucl_byte[chunk.compsze];
			ucl_bytep dst=new ucl_byte[chunk.orgsze];
			fread(src,chunk.compsze,1,uclf);
			ucl_uint sze=chunk.orgsze;
			ucl_nrv2d_decompress_safe_8(src,chunk.compsze,dst,&sze,0);

			FILE* fdst=fopen(FileName,"wb");
			fwrite(dst,chunk.orgsze,1,fdst);
			fclose(fdst);

			delete[]src;
			delete[]dst;
			fclose(uclf);
			delete[]File;
			return TRUE;
		}
		else
			fseek(uclf,chunk.compsze,SEEK_CUR);
	}
	fclose(uclf);
	delete[]File;
	return FALSE;
}

void UCLUnGetFile(const char* FileName)
{
	remove(FileName);
	return;
}
#endif
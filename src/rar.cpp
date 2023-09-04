#include "General.h"
#ifdef GCOMP_RAR
#include "Debug.h"
#include "RAR.h"

#define ERAR_END_ARCHIVE     10
#define ERAR_NO_MEMORY       11
#define ERAR_BAD_DATA        12
#define ERAR_BAD_ARCHIVE     13
#define ERAR_UNKNOWN_FORMAT  14
#define ERAR_EOPEN           15
#define ERAR_ECREATE         16
#define ERAR_ECLOSE          17
#define ERAR_EREAD           18
#define ERAR_EWRITE          19
#define ERAR_SMALL_BUF       20

#define RAR_OM_LIST           0
#define RAR_OM_EXTRACT        1

#define RAR_SKIP              0
#define RAR_TEST              1
#define RAR_EXTRACT           2

#define RAR_VOL_ASK           0
#define RAR_VOL_NOTIFY        1

#pragma pack(1)
struct RARHeaderData
{
  char ArcName[260];
  char FileName[260];
  UINT Flags;
  UINT PackSize;
  UINT UnpSize;
  UINT HostOS;
  UINT FileCRC;
  UINT FileTime;
  UINT UnpVer;
  UINT Method;
  UINT FileAttr;
  char *CmtBuf;
  UINT CmtBufSize;
  UINT CmtSize;
  UINT CmtState;
};


struct RAROpenArchiveData
{
  char *ArcName;
  UINT OpenMode;
  UINT OpenResult;
  char *CmtBuf;
  UINT CmtBufSize;
  UINT CmtSize;
  UINT CmtState;
};
#pragma pack()

typedef HANDLE (PASCAL* LPRAROpenArchive)(struct RAROpenArchiveData *ArchiveData);
typedef int (PASCAL* LPRARCloseArchive)(HANDLE hArcData);
typedef int (PASCAL* LPRARReadHeader)(HANDLE hArcData,struct RARHeaderData *HeaderData);
typedef int (PASCAL* LPRARProcessFile)(HANDLE hArcData,int Operation,char *DestPath,char *DestName);
typedef void (PASCAL* LPRARSetChangeVolProc)(HANDLE hArcData,int (*ChangeVolProc)(char *ArcName,int Mode));
typedef void (PASCAL* LPRARSetProcessDataProc)(HANDLE hArcData,int (*ProcessDataProc)(unsigned char *Addr,int Size));
typedef void (PASCAL* LPRARSetPassword)(HANDLE hArcData,char *Password);

LPRAROpenArchive RAROpenArchive;
LPRARCloseArchive RARCloseArchive;
LPRARReadHeader RARReadHeader;
LPRARProcessFile RARProcessFile;
LPRARSetChangeVolProc RARSetChangeVolProc;
LPRARSetProcessDataProc RARSetProcessDataProc;
LPRARSetPassword RARSetPassword;
HINSTANCE RARhInstDLL;
char LastRAR[BMAX_PATH];
char szRARDir[BMAX_PATH];
HANDLE LastRARHandle;

BOOL RARLoadDLL()
{
	RARhInstDLL=LoadLibrary("UnRAR.DLL");
	if(!RARhInstDLL) return FALSE;
	
	if(!(RAROpenArchive=(LPRAROpenArchive)GetProcAddress(RARhInstDLL,"RAROpenArchive"))) return -1;
	if(!(RARCloseArchive=(LPRARCloseArchive)GetProcAddress(RARhInstDLL,"RARCloseArchive"))) return -1;
	if(!(RARReadHeader=(LPRARReadHeader)GetProcAddress(RARhInstDLL,"RARReadHeader"))) return -1;
	if(!(RARProcessFile=(LPRARProcessFile)GetProcAddress(RARhInstDLL,"RARProcessFile"))) return -1;
	if(!(RARSetChangeVolProc=(LPRARSetChangeVolProc)GetProcAddress(RARhInstDLL,"RARSetChangeVolProc"))) return -1;
	if(!(RARSetProcessDataProc=(LPRARSetProcessDataProc)GetProcAddress(RARhInstDLL,"RARSetProcessDataProc"))) return -1;
	if(!(RARSetPassword=(LPRARSetPassword)GetProcAddress(RARhInstDLL,"RARSetPassword"))) return -1;

	return TRUE;
}

void RARFreeDLL()
{
	if(LastRARHandle!=0)
		RARCloseArchive(LastRARHandle);
	LastRARHandle=0;
	if(RARhInstDLL!=0) 
		FreeLibrary(RARhInstDLL);
	RARhInstDLL=0;
	return;
}

BOOL RARGetFile(const char* ArcName,const char* FileName,const char *Password)
{
	HANDLE hArcData;
	int RHCode,PFCode;
	struct RARHeaderData HeaderData;
	struct RAROpenArchiveData OpenArchiveData;

	_getcwd(szRARDir,BMAX_PATH);
	if(szRARDir[strlen(szRARDir)-1]!='\\')
		strcat(szRARDir,"\\");
	strcat(szRARDir,ArcName);
	if((strcmp(szRARDir,LastRAR)!=0)||(LastRARHandle==0))
	{
		if(LastRARHandle!=0)
			RARCloseArchive(LastRARHandle);
		LastRARHandle=0;
		LastRAR[0]='\0';

		memset(&OpenArchiveData,0,sizeof(RAROpenArchiveData));
		OpenArchiveData.ArcName=(char*)malloc(32);
		strcpy(OpenArchiveData.ArcName,ArcName);
		OpenArchiveData.CmtBuf=0;
		OpenArchiveData.CmtBufSize=0;
		OpenArchiveData.OpenMode=RAR_OM_EXTRACT;
		hArcData=RAROpenArchive(&OpenArchiveData);

		if (OpenArchiveData.OpenResult!=0)
		{
			if(hArcData!=0)
				RARCloseArchive(hArcData);
			return 0;
		}
		LastRARHandle=hArcData;
		strcpy(LastRAR,szRARDir);
	}
	else
		hArcData=LastRARHandle;


  if(Password!=NULL)
	  RARSetPassword(hArcData,(char*)Password);
  HeaderData.CmtBuf=NULL;

  BOOL Found=0;
  while ((RHCode=RARReadHeader(hArcData,&HeaderData))==0)
  {
	  if(strcmp(strtoupper(HeaderData.FileName),strtoupper((char*)FileName))==0)
	  {
		PFCode=RARProcessFile(hArcData,RAR_EXTRACT,
							  NULL,NULL);
		Found=TRUE;
		break;
	  }
	  else
		PFCode=RARProcessFile(hArcData,RAR_SKIP,
							  NULL,NULL);

    if (PFCode!=0)
      break;
  }

	if ((RHCode==ERAR_BAD_DATA)||(!Found))
	{
		RARCloseArchive(hArcData);
		memset(LastRAR,0,BMAX_PATH);
		LastRARHandle=0;
		return FALSE;
	}
	return TRUE;
}

void RARUnGetFile(const char* FileName)
{
	remove(FileName);
	return;
}

BOOL RARGetOneFile(const char* ArcName,const char* FileName,const char *Password)
{
	HANDLE hArcData;
	int RHCode,PFCode;
	struct RARHeaderData HeaderData;
	struct RAROpenArchiveData OpenArchiveData;

	_getcwd(szRARDir,BMAX_PATH);
	if(szRARDir[strlen(szRARDir)-1]!='\\')
		strcat(szRARDir,"\\");
	strcat(szRARDir,ArcName);

	if(LastRARHandle!=0)
		RARCloseArchive(LastRARHandle);
	LastRARHandle=0;
	LastRAR[0]='\0';

	memset(&OpenArchiveData,0,sizeof(RAROpenArchiveData));
	OpenArchiveData.ArcName=(char*)malloc(32);
	strcpy(OpenArchiveData.ArcName,ArcName);
	OpenArchiveData.CmtBuf=0;
	OpenArchiveData.CmtBufSize=0;
	OpenArchiveData.OpenMode=RAR_OM_EXTRACT;
	hArcData=RAROpenArchive(&OpenArchiveData);

	if (OpenArchiveData.OpenResult!=0)
	{
		if(hArcData!=0)
			RARCloseArchive(hArcData);
		return 0;
	}


  if(Password!=NULL)
	  RARSetPassword(hArcData,(char*)Password);
  HeaderData.CmtBuf=NULL;

	BOOL Found=0;
	while ((RHCode=RARReadHeader(hArcData,&HeaderData))==0)
	{
	  if(strcmp(strtoupper(HeaderData.FileName),strtoupper((char*)FileName))==0)
	  {
		PFCode=RARProcessFile(hArcData,RAR_EXTRACT,
							  NULL,NULL);
		Found=TRUE;
		break;
	  }
	  else
		PFCode=RARProcessFile(hArcData,RAR_SKIP,
							  NULL,NULL);

	if (PFCode!=0)
	  break;
	}

	RARCloseArchive(hArcData);

	if ((RHCode==ERAR_BAD_DATA)||(!Found))
		return FALSE;
	else
		return TRUE;
}
#endif

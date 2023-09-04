#include "General.h"
#ifdef GCOMP_ACE
#include "Debug.h"
#include "ACE.h"

#define ACEERR_MEM      1
#define ACEERR_FILES    2
#define ACEERR_FOUND    3
#define ACEERR_FULL     4
#define ACEERR_OPEN     5
#define ACEERR_READ     6
#define ACEERR_WRITE    7
#define ACEERR_CLINE    8
#define ACEERR_CRC      9
#define ACEERR_OTHER   10
#define ACEERR_EXISTS  11
#define ACEERR_END      128
#define ACEERR_HANDLE   129
#define ACEERR_CONSTANT 130
#define ACEERR_NOPASSW  131
#define ACEERR_METHOD   132
#define ACEERR_USER   255

#define ACEOPEN_LIST    0
#define ACEOPEN_EXTRACT 1

#define ACECMD_SKIP     0
#define ACECMD_TEST     1
#define ACECMD_EXTRACT  2

#define ACEVOL_REQUEST  0
#define ACEVOL_OPENED   1

#define ACESEL_YES      0
#define ACESEL_ALWAYS   1
#define ACESEL_NO       2
#define ACESEL_CANCEL   3

#define ACECMT_OK       0
#define ACECMT_SMALLBUF 1
#define ACECMT_NONE   255

#pragma pack(1)
struct ACEHeaderData
{
  char ArcName[260];
  char FileName[260];
  UINT Flags;
  UINT PackSize;
  UINT UnpSize;
  UINT FileCRC;
  UINT FileTime;
  UINT Method;
  UINT QUAL;
  UINT FileAttr;
  char *CmtBuf;
  UINT CmtBufSize;
  UINT CmtSize;
  UINT CmtState;
};

struct ACEOpenArchiveData
{
  char *ArcName;
  UINT OpenMode;
  UINT OpenResult;
  UINT Flags;
  UINT Host;
  char AV[51];
  char *CmtBuf;
  UINT CmtBufSize;
  UINT CmtSize;
  UINT CmtState;
  int i1;
  int i2;
//  int PASCAL (*ChangeVolProc)(char *ArcName,int Mode);
//  int PASCAL (*ProcessDataProc)(unsigned char *Addr,int Size);
};

#pragma pack()
/*
HANDLE __stdcall ACEOpenArchive(struct ACEOpenArchiveData *ArchiveData);
int __stdcall ACECloseArchive(HANDLE hArcData);
int __stdcall ACEReadHeader(HANDLE hArcData,struct ACEHeaderData *HeaderData);
int __stdcall ACEProcessFile(HANDLE hArcData,int Operation,char *DestPath);
int __stdcall ACESetPassword(HANDLE hArcData,char *Password);
*/
typedef HANDLE (__stdcall* LPACEOpenArchive)(struct ACEOpenArchiveData *ArchiveData);
typedef int (__stdcall* LPACECloseArchive)(HANDLE hArcData);
typedef int (__stdcall* LPACEReadHeader)(HANDLE hArcData,struct ACEHeaderData *HeaderData);
typedef int (__stdcall* LPACEProcessFile)(HANDLE hArcData,int Operation,char *DestPath);
typedef void (__stdcall* LPACESetPassword)(HANDLE hArcData,char *Password);

LPACEOpenArchive ACEOpenArchive;
LPACECloseArchive ACECloseArchive;
LPACEReadHeader ACEReadHeader;
LPACEProcessFile ACEProcessFile;
//LPACESetChangeVolProc ACESetChangeVolProc;
//LPACESetProcessDataProc ACESetProcessDataProc;
LPACESetPassword ACESetPassword;
HINSTANCE ACEhInstDLL;
char LastACE[BMAX_PATH];
char szACEDir[BMAX_PATH];
HANDLE LastACEHandle;

BOOL ACELoadDLL()
{
	ACEhInstDLL=LoadLibrary("UnACE.DLL");
	if(!ACEhInstDLL) return FALSE;
	
	if(!(ACEOpenArchive=(LPACEOpenArchive)GetProcAddress(ACEhInstDLL,"ACEOpenArchive"))) return -1;
	if(!(ACECloseArchive=(LPACECloseArchive)GetProcAddress(ACEhInstDLL,"ACECloseArchive"))) return -1;
	if(!(ACEReadHeader=(LPACEReadHeader)GetProcAddress(ACEhInstDLL,"ACEReadHeader"))) return -1;
	if(!(ACEProcessFile=(LPACEProcessFile)GetProcAddress(ACEhInstDLL,"ACEProcessFile"))) return -1;
//	if(!(ACESetChangeVolProc=(LPACESetChangeVolProc)GetProcAddress(ACEhInstDLL,"ACESetChangeVolProc"))) return -1;
//	if(!(ACESetProcessDataProc=(LPACESetProcessDataProc)GetProcAddress(ACEhInstDLL,"ACESetProcessDataProc"))) return -1;
	if(!(ACESetPassword=(LPACESetPassword)GetProcAddress(ACEhInstDLL,"ACESetPassword"))) return -1;
	return TRUE;
}

void ACEFreeDLL()
{
	if(LastACEHandle!=0)
		ACECloseArchive(LastACEHandle);
	LastACEHandle=0;
	if(ACEhInstDLL!=0) 
		FreeLibrary(ACEhInstDLL);
	ACEhInstDLL=0;
	return;
}

BOOL ACEGetFile(const char* ArcName,const char* FileName,const char *Password)
{
	HANDLE hArcData;
	int RHCode,PFCode;
	struct ACEHeaderData HeaderData;
	struct ACEOpenArchiveData OpenArchiveData;

	_getcwd(szACEDir,BMAX_PATH);
	if(szACEDir[strlen(szACEDir)-1]!='\\')
		strcat(szACEDir,"\\");
	strcat(szACEDir,ArcName);
	if((strcmp(szACEDir,LastACE)!=0)||(LastACEHandle==0))
	{
		if(LastACEHandle!=0)
			ACECloseArchive(LastACEHandle);
		LastACEHandle=0;
		LastACE[0]='\0';

		memset(&OpenArchiveData,0,sizeof(ACEOpenArchiveData));
		OpenArchiveData.ArcName=(char*)malloc(32);
		strcpy(OpenArchiveData.ArcName,ArcName);
		OpenArchiveData.CmtBuf=0;
		OpenArchiveData.CmtBufSize=0;
		OpenArchiveData.OpenMode=ACEOPEN_EXTRACT;
		hArcData=ACEOpenArchive(&OpenArchiveData);

		if (OpenArchiveData.OpenResult!=0)
		{
			if(hArcData!=0)
				ACECloseArchive(hArcData);
			return 0;
		}
		LastACEHandle=hArcData;
		strcpy(LastACE,szACEDir);
	}
	else
		hArcData=LastACEHandle;


	if(Password!=NULL)
	  ACESetPassword(hArcData,(char*)Password);
	HeaderData.CmtBuf=NULL;

	BOOL Found=0;
	while ((RHCode=ACEReadHeader(hArcData,&HeaderData))==0)
	{
	  if(strcmp(strtoupper(HeaderData.FileName),strtoupper((char*)FileName))==0)
	  {
		PFCode=ACEProcessFile(hArcData,ACECMD_EXTRACT,
							  NULL);
		Found=TRUE;
		break;
	  }
	  else
		PFCode=ACEProcessFile(hArcData,ACECMD_SKIP,
							  NULL);

	if (PFCode!=0)
	  break;
	}

	if ((RHCode!=0)||(!Found))
	{
		ACECloseArchive(hArcData);
		memset(LastACE,0,BMAX_PATH);
		LastACEHandle=0;
		return FALSE;
	}
	return TRUE;
}

BOOL ACEGetOneFile(const char* ArcName,const char* FileName,const char *Password)
{
	HANDLE hArcData;
	int RHCode,PFCode;
	struct ACEHeaderData HeaderData;
	struct ACEOpenArchiveData OpenArchiveData;

	_getcwd(szACEDir,BMAX_PATH);
	if(szACEDir[strlen(szACEDir)-1]!='\\')
		strcat(szACEDir,"\\");
	strcat(szACEDir,ArcName);

	if(LastACEHandle!=0)
		ACECloseArchive(LastACEHandle);
	LastACEHandle=0;
	LastACE[0]='\0';

	memset(&OpenArchiveData,0,sizeof(ACEOpenArchiveData));
	OpenArchiveData.ArcName=(char*)malloc(32);
	strcpy(OpenArchiveData.ArcName,ArcName);
	OpenArchiveData.CmtBuf=0;
	OpenArchiveData.CmtBufSize=0;
	OpenArchiveData.OpenMode=ACEOPEN_EXTRACT;
	hArcData=ACEOpenArchive(&OpenArchiveData);

	if (OpenArchiveData.OpenResult!=0)
	{
		if(hArcData!=0)
			ACECloseArchive(hArcData);
		return 0;
	}

	if(Password!=NULL)
	  ACESetPassword(hArcData,(char*)Password);
	HeaderData.CmtBuf=NULL;

	BOOL Found=0;
	while ((RHCode=ACEReadHeader(hArcData,&HeaderData))==0)
	{
	  if(strcmp(strtoupper(HeaderData.FileName),strtoupper((char*)FileName))==0)
	  {
		PFCode=ACEProcessFile(hArcData,ACECMD_EXTRACT,
							  NULL);
		Found=TRUE;
		break;
	  }
	  else
		PFCode=ACEProcessFile(hArcData,ACECMD_SKIP,
							  NULL);

	if (PFCode!=0)
	  break;
	}
	ACECloseArchive(hArcData);

	if ((RHCode!=0)||(!Found))
		return FALSE;
	else
		return TRUE;
}

void ACEUnGetFile(const char* FileName)
{
	remove(FileName);
	return;
}
#endif

#include "General.h"
#include "PAK.h"
#include "LoadFile.h"
#include "Debug.h"

#undef MODE_PAK
#define MODE_PAK 0x01
#undef MODE_EXIST
#define MODE_EXIST 0x02

char szPath[BMAX_PATH];
char szDefaultPath[BMAX_PATH];
char szArc[BMAX_PATH];
char* szPassword=NULL;
char LoadedFile[MAX_LOADEDFILES][BMAX_PATH];
char LoadedPath[MAX_LOADEDFILES][BMAX_PATH];
BOOL IsLoaded[MAX_LOADEDFILES];
int  LoadMode[MAX_LOADEDFILES];
BOOL bInit=0;

BOOL InitFileLoader(const char* szDefDir,const char* szDir,const char* ArcName,const char* szPass)
{
	if(bInit)
		CloseFileLoader();
	strcpy(szPath,szDir);
	strcpy(szDefaultPath,szDefDir);
	strcpy(szArc,ArcName);
	if(szPassword!=0)
		delete[] szPassword;
	if(szPass!=0)
	{
		szPassword=new char[MAX_LOADEDFILES];
		strcpy(szPassword,szPass);
	}
	else
		szPassword=NULL;
	_chdir(szDefaultPath);
	return bInit=PAKLoadDLL();
}

BOOL LoadFile(const char* szFile)
{
	int c;
	int t=-1;
	char File[BMAX_PATH];
	if(!bInit) return 0;
	strcpy(File,szFile);
	strtoupper(File);
	_chdir(szPath);

	for(c=0;c<MAX_LOADEDFILES;c++)
		if(!IsLoaded[c]) {t=c;break;}

	if(t==-1) return 0;
	strcpy(LoadedFile[t],File);
	strcpy(LoadedPath[t],szPath);
	if(!DoesFileExist(File))
	{
		if(!PAKGetFile(szArc,File,szPassword))
		{
			strcpy(LoadedPath[t],szDefaultPath);
			_chdir(szDefaultPath);
			if(!DoesFileExist(File))
			{
				if(!PAKGetFile(szArc,File,szPassword))
					return 0;
				IsLoaded[t]=TRUE;
				LoadMode[t]=MODE_PAK;
			}
			else
			{
				IsLoaded[t]=TRUE;
				LoadMode[t]=MODE_EXIST;
			}
		}
		else
		{
			IsLoaded[t]=TRUE;
			LoadMode[t]=MODE_PAK;
		}
	}
	else
	{
		IsLoaded[t]=TRUE;
		LoadMode[t]=MODE_EXIST;
	}
	return TRUE;
}

BOOL LoadRootFile(const char* szFile)
{
	int c;
	int t=-1;
	char File[BMAX_PATH];
	if(!bInit) return 0;
	strcpy(File,szFile);
	strtoupper(File);
	_chdir(szDefaultPath);

	for(c=0;c<MAX_LOADEDFILES;c++)
		if(!IsLoaded[c]) {t=c;break;}

	if(t==-1) return 0;
	strcpy(LoadedFile[t],File);
	strcpy(LoadedPath[t],szDefaultPath);
	if(!DoesFileExist(File))
	{
		if(!PAKGetOneFile(szArc,File,szPassword))
			return 0;
		else
		{
			IsLoaded[t]=TRUE;
			LoadMode[t]=MODE_PAK;
		}
	}
	else
	{
		IsLoaded[t]=TRUE;
		LoadMode[t]=MODE_EXIST;
	}
	return TRUE;
}

void UnloadFile(const char* szFile)
{
	if(!bInit) return;
	char File[BMAX_PATH];
	strcpy(File,szFile);
	strtoupper(File);
	for(int c=0;c<MAX_LOADEDFILES;c++)
		if(IsLoaded[c])
			if(strcmp(LoadedFile[c],File)==0)
			{
				if(LoadMode[c]!=MODE_EXIST)
				{
					_chdir(LoadedPath[c]);
					remove(File);
				}
				IsLoaded[c]=0;
				return;
			}

	return;
}

void CloseFileLoader()
{
	if(!bInit) return;
	if(szPassword!=0)
		delete[] szPassword;
	for(int c=0;c<MAX_LOADEDFILES;c++)
		if(IsLoaded[c])
			UnloadFile(LoadedFile[c]);
	bInit=FALSE;
	PAKFreeDLL();
	return;
}

BOOL DoesFileExist(const char* file)
{
	FILE* h;
	h=fopen(file,"rb");
	if(h==0)
		return FALSE;
	fclose(h);
	return TRUE;
}

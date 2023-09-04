#include "General.h"
#include "Skins.h"
HANDLE hFind=0;
char Skin[MAX_SKINS][BMAX_PATH];
int SkinCount;
char szSkinDir[BMAX_PATH];

void GetSkinName(int c,char* szSkinName)
{
	if(c<SkinCount)
		strcpy(szSkinName,Skin[c]);
	return;
}

int EnumSkins(const char* szSD)
{
	SkinCount=0;
	strcpy(szSkinDir,szSD);
	if(FindFirstSkin(szSkinDir,Skin[0]))
	{
		SkinCount++;
		while((SkinCount<MAX_SKINS-1)&&FindNextSkin(Skin[SkinCount++]));
		SkinCount--;
	}
	FindSkinClose();
	return SkinCount;
}

int GetSkinCount()
{
	return SkinCount;
}

BOOL DoesSkinExist(const char* szSkin)
{
	if(strlen(szSkin)==0) return TRUE;
	for(int c=0;c<SkinCount;c++)
		if(strcmp(Skin[c],szSkin)==0)
			return TRUE;
	return 0;
}

BOOL FindFirstSkin(const char* szDir,char* szFirstSkinDir)
{
	WIN32_FIND_DATA wfd;
	BOOL rt=TRUE;
	char szSearchDir[BMAX_PATH];

	strcpy(szSearchDir,szDir);
	if(szSearchDir[strlen(szSearchDir)-1]!='\\')
		strcat(szSearchDir,"\\");
	strcat(szSearchDir,"*.*");

	if(hFind)
		FindClose(hFind);
	memset(&wfd,0,sizeof(WIN32_FIND_DATA));
	hFind=FindFirstFile(szSearchDir,&wfd);
	if(hFind==INVALID_HANDLE_VALUE)
	{
		hFind=0;
		return FALSE;
	}

	while((rt&&((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0))||(wfd.cFileName[0]=='.'))
	{
		memset(&wfd,0,sizeof(WIN32_FIND_DATA));
		rt=FindNextFile(hFind,&wfd);
		if(rt==0) break;
	}

	if(rt)
		strcpy(szFirstSkinDir,wfd.cFileName);
	else
		return FALSE;
	return TRUE;
}

BOOL FindNextSkin(char* szNextSkinDir)
{
	WIN32_FIND_DATA wfd;
	BOOL rt;
	if(hFind==0)
		return FALSE;

	do
	{
		memset(&wfd,0,sizeof(WIN32_FIND_DATA));
		rt=FindNextFile(hFind,&wfd);
		if(rt==0) break;
	}while((rt&&((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0))||(wfd.cFileName[0]=='.'));

	if(rt)
		strcpy(szNextSkinDir,wfd.cFileName);
	else
		return FALSE;
	return TRUE;
}

void FindSkinClose()
{
	FindClose(hFind);
	return;
}

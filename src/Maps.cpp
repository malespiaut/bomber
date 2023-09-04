#include "General.h"
#include "Maps.h"
HANDLE hFindMap=0;
char Map[MAX_MAPS][BMAX_PATH];
int MapCount;
char szMapDir[BMAX_PATH];

void GetMapName(int c,char* szMapName)
{
	if(c<MapCount)
		strcpy(szMapName,Map[c]);
	return;
}

int EnumMaps(const char* szMD)
{
	MapCount=0;
	strcpy(szMapDir,szMD);
	if(FindFirstMap(szMapDir,Map[0]))
	{
		MapCount++;
		while((MapCount<MAX_MAPS-1)&&FindNextMap(Map[MapCount++]));
		MapCount--;
	}
	FindMapClose();
	return MapCount;
}

int GetMapCount()
{
	return MapCount;
}

BOOL DoesMapExist(const char* szMap)
{
	if(strlen(szMap)==0) return TRUE;
	for(int c=0;c<MapCount;c++)
		if(strcmp(Map[c],szMap)==0)
			return TRUE;
	return 0;
}

BOOL FindFirstMap(const char* szDir,char* szFirstMapDir)
{
	WIN32_FIND_DATA wfd;
	BOOL rt=TRUE;
	char szSearchDir[BMAX_PATH];

	strcpy(szSearchDir,szDir);
	if(szSearchDir[strlen(szSearchDir)-1]!='\\')
		strcat(szSearchDir,"\\");
	strcat(szSearchDir,"*.*");

	if(hFindMap)
		FindClose(hFindMap);
	memset(&wfd,0,sizeof(WIN32_FIND_DATA));
	hFindMap=FindFirstFile(szSearchDir,&wfd);
	if(hFindMap==INVALID_HANDLE_VALUE)
	{
		hFindMap=0;
		return FALSE;
	}

	char sd[BMAX_PATH];
	FILE* F;
	do
	{
		F=0;
		while((rt&&((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0))||(wfd.cFileName[0]=='.'))
		{
			memset(&wfd,0,sizeof(WIN32_FIND_DATA));
			rt=FindNextFile(hFindMap,&wfd);
			if(rt==0) break;
		}
		if(rt)
		{
			sprintf(sd,"Maps\\%s\\BMap.txt",wfd.cFileName);
			F=fopen(sd,"rb");
		}
	}
	while(rt&&(F==0));

	if(F!=0)
		fclose(F);
	else
		rt=0;

	if(rt)
		strcpy(szFirstMapDir,wfd.cFileName);
	else
		return FALSE;
	return TRUE;
}

BOOL FindNextMap(char* szNextMapDir)
{
	WIN32_FIND_DATA wfd;
	BOOL rt;
	if(hFindMap==0)
		return FALSE;

	char sd[BMAX_PATH];
	FILE* F;
	do
	{
		F=0;
		do
		{
			memset(&wfd,0,sizeof(WIN32_FIND_DATA));
			rt=FindNextFile(hFindMap,&wfd);
			if(rt==0) break;
		}while((rt&&((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0))||(wfd.cFileName[0]=='.'));

		if(rt)
		{
			sprintf(sd,"Maps\\%s\\BMap.txt",wfd.cFileName);
			F=fopen(sd,"rb");
		}
	}while(rt&&(F==0));

	if(F!=0)
		fclose(F);
	else
		rt=0;

	if(rt)
		strcpy(szNextMapDir,wfd.cFileName);
	else
		return FALSE;
	return TRUE;
}

void FindMapClose()
{
	FindClose(hFindMap);
	return;
}

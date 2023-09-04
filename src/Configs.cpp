#include "General.h"
#include "Configs.h"
HANDLE hFindConfig=0;
char Config[MAX_CONFIGS][BMAX_PATH];
int ConfigCount;

void GetConfigName(int c,char* szConfigName)
{
	if(c<ConfigCount)
		strcpy(szConfigName,Config[c]);
	return;
}

int EnumConfigs(const char* szConfigPath)
{
	ConfigCount=0;
	if(FindFirstConfig(szConfigPath,Config[0]))
	{
		ConfigCount++;
		while((ConfigCount<MAX_CONFIGS-1)&&FindNextConfig(Config[ConfigCount++]));
		ConfigCount--;
	}
	FindConfigClose();
	return ConfigCount;
}

int GetConfigCount()
{
	return ConfigCount;
}

BOOL DoesConfigExist(const char* szConfig)
{
	if(strlen(szConfig)==0) return 0;
	for(int c=0;c<ConfigCount;c++)
		if(strcmp(Config[c],szConfig)==0)
			return TRUE;
	return 0;
}

BOOL FindFirstConfig(const char* szConfigPath,char* szFirstConfig)
{
	WIN32_FIND_DATA wfd;
	BOOL rt=TRUE;
	char szSearchDir[BMAX_PATH];

	strcpy(szSearchDir,szConfigPath);
	if(szSearchDir[strlen(szSearchDir)-1]!='\\')
		strcat(szSearchDir,"\\");
	strcat(szSearchDir,"*.cfg");

	if(hFindConfig)
		FindClose(hFindConfig);

	memset(&wfd,0,sizeof(WIN32_FIND_DATA));
	hFindConfig=FindFirstFile(szSearchDir,&wfd);
	if(hFindConfig==INVALID_HANDLE_VALUE)
	{
		hFindConfig=0;
		return FALSE;
	}
	while(rt&&((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0))
	{
		memset(&wfd,0,sizeof(WIN32_FIND_DATA));
		rt=FindNextFile(hFindConfig,&wfd);
		if(rt==0) break;
	}

	if(rt)
		strcpy(szFirstConfig,wfd.cFileName);
	else
		return FALSE;
	return TRUE;
}

BOOL FindNextConfig(char* szNextConfig)
{
	WIN32_FIND_DATA wfd;
	BOOL rt;
	if(hFindConfig==0)
		return FALSE;

	do
	{
		memset(&wfd,0,sizeof(WIN32_FIND_DATA));
		rt=FindNextFile(hFindConfig,&wfd);
		if(rt==0) break;
	}while(rt&&((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0));

	if(rt)
		strcpy(szNextConfig,wfd.cFileName);
	else
		return FALSE;
	return TRUE;
}

void FindConfigClose()
{
	FindClose(hFindConfig);
	return;
}

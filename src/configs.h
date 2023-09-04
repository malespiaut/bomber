#ifndef _CONFIGS_H_
#define _CONFIGS_H_
BOOL FindFirstConfig(const char*,char*);
BOOL FindNextConfig(char*);
void FindConfigClose(void);
BOOL DoesConfigExist(const char*);
int GetConfigCount(void);
int EnumConfigs(const char*);
void GetConfigName(int,char*);
#endif

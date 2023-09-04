#ifndef _MAPS_H_
#define _MAPS_H_
BOOL FindFirstMap(const char*,char*);
BOOL FindNextMap(char*);
BOOL DoesMapExist(const char*);
void FindMapClose(void);
int GetMapCount(void);
int EnumMaps(const char*);
void GetMapName(int,char*);
#endif

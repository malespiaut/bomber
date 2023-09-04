#ifndef _SKINS_H_
#define _SKINS_H_
BOOL FindFirstSkin(const char*,char*);
BOOL FindNextSkin(char*);
void FindSkinClose(void);
BOOL DoesSkinExist(const char*);
int GetSkinCount(void);
int EnumSkins(const char*);
void GetSkinName(int,char*);
#endif
